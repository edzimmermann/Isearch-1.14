/***********************************************************************
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
File:          	isrch_fetch.cxx
Version:        1.02
Description:    CGI app that searches against Iindex-ed databases 
Author:         Kevin Gamiel, kgamiel@cnidr.org
@@@*/

#include <iostream.h>
#include "gdt.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "idb.hxx"
#include "string.hxx"
#include "config.hxx"

int main(int argc, char **argv)
{
	STRING DBPathName, DBRootName, Record;
	IDB *pdb;
	RESULT RsRecord;
	STRING RecordKey, ESet;

	cout << "Content-type: text/html\n\n";

	cout << "<a href=\"http://www.cnidr.org/\"><i>CNIDR</a> Isearch-cgi ";
	cout << VERSION << "</i> ";

	if(argc < 4) {
		cout << "<p>Usage:  isrch_fetch &lt;dbpath&gt; &lt;dbname&gt; &lt;key&gt; &lt;es&gt;\n";
		exit(0);
	}

	STRING File;
	DBPathName = argv[1];
	DBRootName = argv[2];
	RecordKey = argv[3];
	ESet = argv[4];

	// Open database
	pdb = new IDB(DBPathName, DBRootName);
	
	// Is the database valid?
	if(pdb->GetTotalRecords() <= 0) {
		cout << "<p>Database " << DBRootName;
		cout << " does not exist or is corrupted\n";
		return -1;
	}

	pdb->KeyLookup(RecordKey, &RsRecord);
	RsRecord.GetFileName(&File);

	cout << "<i>(File: " << File << ")</i><p>" << endl;

	PCHR name;
	name=File.NewCString();
	if ((strstr(name,".html")!=name+strlen(name)-5) && (strstr(name,".htm")!=name+strlen(name)-4))
		cout << "\n<pre>" << endl;

	pdb->Present(RsRecord, ESet, &Record);
	cout << Record;

	if ((strstr(name,".html")!=name+strlen(name)-5) && (strstr(name,".htm")!=name+strlen(name)-4))
		cout << "\n</pre>" << endl;

	return 0;
}

