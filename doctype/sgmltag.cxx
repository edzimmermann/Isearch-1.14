/************************************************************************
Copyright Notice

Copyright (c) MCNC, Clearinghouse for Networked Information Discovery and
Retrieval, 1994. 

Permission to use, copy, modify, distribute, and sell this software and
its documentation, in whole or in part, for any purpose is hereby granted
without fee, provided that

1. The above copyright notice and this permission notice appear in all
copies of the software and related documentation. Notices of copyright
and/or attribution which appear at the beginning of any file included in
this distribution must remain intact. 

2. Users of this software agree to make their best efforts (a) to return
to MCNC any improvements or extensions that they make, so that these may
be included in future releases; and (b) to inform MCNC/CNIDR of noteworthy
uses of this software. 

3. The names of MCNC and Clearinghouse for Networked Information Discovery
and Retrieval may not be used in any advertising or publicity relating to
the software without the specific, prior written permission of MCNC/CNIDR. 

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 

IN NO EVENT SHALL MCNC/CNIDR BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
************************************************************************/

/*@@@
File:		sgmltag.cxx
Version:	1.03
Description:	Class SGMLTAG - SGML-like Text
Author:		Kevin Gamiel, Kevin.Gamiel@cnidr.org
Changes:	1.02
			Thanks to Jae Chang (jae+@CMU.EDU) for these fixes:
			- RecBuffer was being overwritten
			- OrigRecBuffer was being overwritten
			- Use all new and deletes instead of mixing with
				mallocs and frees.
			- Free memory if parse fails
		1.03
			- Misc Error checking
@@@*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "sgmltag.hxx"

// Local prototypes
char *find_end_tag(char **t, char *tag);
char **sgml_parse_tags(char *b, int len);

SGMLTAG::SGMLTAG(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
}

/*
- Open the file.
- Read two copies of the file into memory (implementation feature;-)
- Build an index into the SGML-like tag pairs.  An SGML-like tag pair is one
	that begins and ends with precisely the same text, excluding the
	closing tag's slash.  For example:

		<title> </title>	- sgml-like
		<dog> </dog>		- sgml-like
		<!-- test>		- NOT sgml-like
		<a href=> </a>		- NOT sgml-like
- For each valid tag pair, hence field, add the field to the Isearch record
	structure.
- Cleanup
*/
void SGMLTAG::ParseFields(PRECORD NewRecord) {
	PFILE 	fp;
	STRING 	fn;
	GPTYPE 	RecStart, 
		RecEnd, 
		RecLength, 
		ActualLength;
	PCHR 	RecBuffer, 
		OrigRecBuffer;
	PCHR 	file;

	// Open the file
	NewRecord->GetFullFileName(&fn);
	file = fn.NewCString();
	fp = fopen(fn, "rb");
	if (!fp) {
		cout << "SGMLTAG::ParseRecords(): Failed to open file\n\t";
		perror(file);
		return;
	}

	// Determine the start and size of the record
	RecStart = NewRecord->GetRecordStart();
	RecEnd = NewRecord->GetRecordEnd();
	if (RecEnd == 0) {
		if(fseek(fp, 0, 2) == -1) {
			cout << "SGMLTAG::ParseRecords(): Seek failed - ";
			cout << fn << "\n";
			fclose(fp);
			return;	
		}
		RecStart = 0;
		RecEnd = ftell(fp);
		if(RecEnd == 0) {
			cout << "SGMLTAG::ParseRecords(): Skipping ";
			cout << " zero-length record -" << fn << "...\n";
			fclose(fp);
			return;
		}
		//RecEnd -= 1;
	}

	// Make two copies of the record in memory
	if(fseek(fp, RecStart, 0) == -1) {
		cout << "SGMLTAG::ParseRecords(): Seek failed - " << fn << "\n";
		fclose(fp);
		return;	
	}
	RecLength = RecEnd - RecStart;
	
	RecBuffer = new CHR[RecLength + 1];
	if(!RecBuffer) {
		cout << "SGMLTAG::ParseRecords(): Failed to allocate ";
		cout << RecLength + 1 << " bytes - " << fn << "\n";
		fclose(fp);
		return;
	}
	OrigRecBuffer = new CHR[RecLength + 1];
	if(!OrigRecBuffer) {
		cout << "SGMLTAG::ParseRecords(): Failed to allocate ";
		cout << RecLength + 1 << " bytes - " << fn << "\n";
		delete [] RecBuffer;
		fclose(fp);
		return;
	}

	ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
	if(ActualLength == 0) {
		cout << "SGMLTAG::ParseRecords(): Failed to fread\n\t";
		perror(file);
		delete [] RecBuffer;
		delete [] OrigRecBuffer;
		fclose(fp);
		return;
	}
	fclose(fp);
	if(ActualLength != RecLength) {
		cout << "SGMLTAG::ParseRecords(): Failed to fread ";
		cout << RecLength << " bytes.  Actually read " << ActualLength;
		cout << " bytes - " << fn << "\n";
		delete [] RecBuffer;
		delete [] OrigRecBuffer;
		return;
	}
	memcpy(OrigRecBuffer, RecBuffer, RecLength);
	OrigRecBuffer[RecLength] = '\0';

	// Parse the record and add fields to record structure
	STRING FieldName;
	FC fc;
	PFCT pfct;
	DF df;
	PDFT pdft;
	PCHR *tags;
	PCHR *tags_ptr;
	PCHR p;
	INT val_start;
	INT val_len;
	DFD dfd;

	pdft = new DFT();
	if(!pdft) {
		cout << "SGMLTAG::ParseRecords(): Failed to allocate DFT - ";
		cout << fn << "\n";
		delete [] RecBuffer;
		delete [] OrigRecBuffer;
		return;
	}
	tags = sgml_parse_tags(RecBuffer, RecLength);
	if(tags == NULL) {
		cout << "Unable to parse SGML file " << fn << "\n";
		delete pdft;
		delete [] RecBuffer;
		delete [] OrigRecBuffer;
		return;
	}
	tags_ptr = tags;	
	while(*tags_ptr) {
		p = find_end_tag(tags_ptr, *tags_ptr);
		if(p) {
			// We have a tag pair
			val_start = (*tags_ptr + strlen(*tags_ptr) + 1) - 
				RecBuffer;
			val_len = (p - *tags_ptr) - strlen(*tags_ptr) - 2;
			FieldName = *tags_ptr;
			dfd.SetFieldName(FieldName);
			Db->DfdtAddEntry(dfd);
			fc.SetFieldStart(val_start);
			fc.SetFieldEnd(val_start + val_len -1);
			pfct = new FCT();
			pfct->AddEntry(fc);
			df.SetFct(*pfct);
			df.SetFieldName(FieldName);
			pdft->AddEntry(df);
			delete pfct;
		}
		tags_ptr++;
	}

	NewRecord->SetDft(*pdft);
	delete pdft;
	delete [] RecBuffer;
	delete [] OrigRecBuffer;
	delete tags;
}

SGMLTAG::~SGMLTAG() {
}

/*
What:	Given a buffer of sgml-tagged data:
		returns a list of char* to all characters immediately following
			each '<' character in the buffer.
		replaces all '>' with '\0' character.
		
Pre:	b = character buffer with valid sgml marked-up text
	len = length of b
	tags = Empty char**

Post:	On success, return value is filled with char pointers to first 
		character of every sgml tag (first character after the '<').  
		Each string is NULL-terminated (where '>' used to reside).
		The tags array is terminated by a NULL char*.
	On failure, returns NULL.
*/
#define TAG_GROW_SIZE 128	// allocate memory in chunks
#define OK 0			// two state conditions for parser
#define NEED_END 1
char **sgml_parse_tags(char *b, int len)
{
	char **t, **u;		// array of pointers to first char of tags
	int tc=0;		// tag count
	int i,j;			// iterator
	int max_num_tags;	// max num tags for which space is allocated
	int State = OK;		// initialize the state

	// You should allocate character pointers (to tags) as you need them.  
	// Start with TAG_GROW_SIZE of them.

	t = new PCHR[128];
	if(!t) {
		cout << "SGMLTAG::sgml_parse_tags(): Out of memory\n";
		return NULL;
	}
	max_num_tags = TAG_GROW_SIZE;

	// Step through every character in the buffer looking for '<' and '>'
	for(i=0;i < len;i++) {
		switch(b[i]) {
			case '>':
				if(State != NEED_END)
					break;
				b[i] = '\0';
				tc += 1;
				State = OK;
				break;
			case '<':
				State = NEED_END;
				t[tc] = &b[i+1];
				break;
			default:
				break;
		}
		if(tc == max_num_tags - 1) {
			// allocate more space
			max_num_tags += TAG_GROW_SIZE;
			u = new PCHR[max_num_tags];
			if(u == NULL) {
				delete [] t;
				return NULL;
			}
			for (j=0; j<=tc; j++)
				u[j] = t[j];
			delete [] t;
			t = u;
		}
	}
	t[tc] = (PCHR)NULL;
	return t;
}

/*
What:	Searches through string list t looking for "/" followed by tag, e.g. 
	if tag = "TITLE", looks for "/TITLE".

Pre:	t is list of string pointers each NULL-terminated.  The list itself
	should be terminated with a NULL character pointer.
	
Post:	Returns a pointer to found string or NULL.
*/
char *find_end_tag(char **t, char *tag)
{
	char *tt;
	int i;

	if(*t == NULL)
		return NULL;

	if(*t[0] == '/')
		return NULL;

	for(i=0, tt = *t; tt ; tt = t[++i]) {
		if(tt[0] == '/') {
			if(!(strcasecmp(&tt[1], tag)))
				return tt;
		}
	}
	return NULL;
}
