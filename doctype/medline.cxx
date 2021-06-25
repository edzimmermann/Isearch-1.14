/************************************************************************
Copyright (c) 1994,1995 Basis Systeme netzwerk, Munich
              Brecherspitzstr. 8
              D-81541 Munich, Germany

              ISRCH-LIC-1B EXPORT: Tue Aug 15 14:20:42 MET DST 1995

              Public Software License Agreement:
              ----------------------------------

Basis Systeme netzwerk(*) (herein after referred to as BSn) hereby
provides COMPANY (herein after referred to as "Licensee") with a
non-exclusive, royalty-free, worldwide license to use, reproduce,
modify and redistribute this software and its documentation (hereafter
referred to as "Materials"), in whole or in part, with Licensee's
products under the following conditions:

1. All copyrights and restrictions in the source files of the Software
Materials will be honored.

2. The above copyright notice and this permission notice appear in all
copies of the software and related documentation. Notices of copyright
and/or attribution which appear at the beginning of any file included
in this distribution must remain intact.

3. The origin of these Materials will be explicitly stated in Licensee's
accompanying documentation as developed by Basis Systeme netzwerk (BSn)
and its collaborators.

4. The name of the author(s) or BSn may not be used to endorse or promote
products derived from these Materials without specific prior written
permission.

5. Versions of the Software Materials or documentation that are altered
or changed will be marked as such.

6. Licensee shall make reasonable efforts to provide BSn with all
enhancements and modifications made by Licensee to the Software
Materials, for a period of three years from the date of execution of
this License. BSn shall have the right to use and/or redistribute the
modifications and enhancements without accounting to Licensee.

Enhancements and Modifications shall be defined as follows:
    i) Changes to the source code, support files or documentation.
   ii) Documentation directly related to Licensee's distribution of the
       software.
  iii) Licensee software modules that actively solicit services from
       the software and accompanying user documentation.

7. Users of this software agree to make their best efforts to inform
BSn of noteworthy uses of this software.

8. You agree that neither you, nor your customers, intend to, or will,
export these Materials to any country which such export or transmission
is restricted by applicable law without prior written consent of the
appropriate government agency with jurisdiction over such export or
transmission.

8. BSn makes no representation on the suitability of the Software
Materials for any purpose.  The SOFTWARE IS PROVIDED "AS IS" AND
WITHOUT EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.

9. Licensee agrees to indemnify, defend and hold harmless BSn from any
loss, claim, damage or liability of any kind, including attorney's fees
and court costs, arising out of or in connection with any use of the
Materials under this License.

10. In no event shall BSn be liable to Licensee or third parties
licensed by licensee for any indirect, special, incidental, or
consequential damages (including lost profits).

11. BSn has no knowledge of any conditions that would impair its right
to license the Materials.  Notwithstanding the foregoing, BSn does
not make any warranties or representations that the Materials are
free of claims by third parties of patent, copyright infringement
or the like, nor does BSn assume any liability in respect of any
such infringement of rights of third parties due to Licensee operation
under this license.

12. IN NO EVENT SHALL BSN OR THE AUTHORS BE LIABLE FOR ANY SPECIAL,
INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
OF THIS SOFTWARE.

13. The place of execution of this agreement is Munich and the applicable
laws are those of the Federal Republic of Germany. The agreement also
remains in force even in states/jurisdictions that exclude one or more
clauses. For these cases the applicable clauses are to be replaced by
other agreements that come as close as possible to the original intent.

"Diese Vereinbarung unterliegt dem Recht der Bundesrepublik Deutschland.
Sie enthaelt saemtliche Vereinbarungen, welche die Parteien hinsichtlich
des Vereinbarungsgegenstandes getroffen haben, und ersetzt alle
vorhergehenden muendlichen oder schriftlichen Abreden. Diese Vereinbarung
bleibt in Zweifel auch bei rechtlicher Unwirksamkeit enzelner Bestimmungen
in seinen uebrigen Teilen verbindlich. Unwirksame Bestimmungen sind
durch Regulungen zu ersetzen, die dem angestrebten Erfolg moeglichst nahe
kommen."

___________________________________________________________________________________
(*)Basis Systeme netzwerk, Brecherspitzstr. 8, 81541 Muenchen, Germany 

************************************************************************/


/*-@@@
File:		medline.cxx
Version:	$Revision: 1.2 $
Description:	Class MEDLINE - Medline Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

// TODO: Clean-up Record parser and fix to leave off junk between records

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "medline.hxx"

/* A few user-customizable parser features... */
#ifndef USE_UNIFIED_NAMES
# define USE_UNIFIED_NAMES 1 
#endif

/* #define BSN_EXTENSIONS 1 */
#if BSN_EXTENSIONS < 1
# define BRIEF_MAGIC "B"
#endif

// Local prototypes
static PCHR *parse_tags (PCHR b, GPTYPE len);

MEDLINE::MEDLINE (PIDBOBJ DbParent): DOCTYPE (DbParent)
{
}

void MEDLINE::AddFieldDefs ()
{
  DOCTYPE::AddFieldDefs ();
}

void MEDLINE::ParseRecords (const RECORD& FileRecord)
{
  // Break up the document into Medline records
  GPTYPE Start = FileRecord.GetRecordStart();
  GPTYPE End = FileRecord.GetRecordEnd();
  GPTYPE Position = 0;
  GPTYPE SavePosition = 0;
  GPTYPE RecordEnd;

  STRING Fn;
  FileRecord.GetFullFileName (&Fn);
  PFILE Fp = fopen (Fn, "rb");
  if (!Fp)
    {
      cout << "Could not access " << Fn << "\n";
      return;			// File not accessed

    }

#if BSN_EXTENSIONS
  RECORD Record (FileRecord); // Easy way
#else
  /* CNIDR must do it the hard way! (see above ) */
  RECORD Record;
  STRING s;
  FileRecord.GetPathName(&s);
  Record.SetPathName( s );
  FileRecord.GetFileName(&s);
  Record.SetFileName( s );
  FileRecord.GetDocumentType(&s);
  Record.SetDocumentType ( s );
#endif

  // Search for Medline Seperator
  enum { LOOK, HUNT, FOUND, START } State = HUNT;
  CHR Ch;
  CHR buf[7];
  int pos = 0;

  // Move to start if defined
  if (Start > 0) fseek(Fp, Start, SEEK_SET);

  while ((Ch = getc (Fp)) != EOF)
    {
      Position++;
      if (End && Position > End) break; // End of Subrecord
      if (Ch == '\n')
	{
	  // New Line
	  if (State == LOOK)
	    {
	      if (pos < 5)
		State = FOUND;
	      else if (!isspace (buf[0]) && !isalpha (buf[0]))
		State = FOUND;	// not space and not letter
	    }
	  else
	    State = LOOK;
	  if (State == FOUND)
	    SavePosition = Position - pos - 1;
	  else
	    SavePosition = Position;
	  pos = 0;
	}
      else
	{
	  // A token
	  if (pos < 6)
	    {
	      buf[pos] = Ch;
	      // Do we have the start of a record with a new field?
	      if (State == FOUND
		  && pos > 4 
		  && (buf[4] == '-' || buf[4] == ' ')
		  && isalpha (buf[0])
		  && isalnum (buf[1])
		  && (buf[2] == ' ' || isalnum (buf[2]))
		  && (buf[3] == ' ' || isalnum (buf[3]))
		  && (buf[2] != ' ' || buf[3] == ' ' ) )
		{
		  State = START;
		}
	    }
	  pos++;		// Line Position;

	}

      if (State == START)
	{
	  State = HUNT;
	  Record.SetRecordStart (Start);
	  RecordEnd = (SavePosition == 0) ? 0 : SavePosition - 1;

	  if (RecordEnd > Start)
	    {
	      Record.SetRecordEnd (RecordEnd);
	      Db->DocTypeAddRecord(Record);
	      Start = SavePosition;
	    }
	}
    }				// while

  fclose (Fp);

  Record.SetRecordStart (Start);
  RecordEnd = (SavePosition == 0) ? 0 : Position - 1;

  if (RecordEnd > Start)
    {
      Record.SetRecordEnd (RecordEnd);
      Db->DocTypeAddRecord(Record);
    }
}


/*
 TODO: Add support for all "known" medline tags
 --- if need be subclass medline into different
 flavours.
*/

PCHR MEDLINE::UnifiedName (PCHR tag) const
{
printf("Medline:UnifiedName called\n");
#if USE_UNIFIED_NAMES
  // For now
  return tag;
#else
  return tag; // Identity
#endif
}


void MEDLINE::ParseFields (PRECORD NewRecord)
{
  STRING fn;
  NewRecord->GetFullFileName (&fn);
  PFILE fp = fopen (fn, "rb");
  if (!fp)
    {
      return;			// ERROR
    }

  GPTYPE RecStart = NewRecord->GetRecordStart ();
  GPTYPE RecEnd = NewRecord->GetRecordEnd ();
  if (RecEnd == 0)
    {
      fseek (fp, 0, 2);
      RecStart = 0;
      RecEnd = ftell (fp) - 1;
    }
  fseek (fp, RecStart, 0);
  GPTYPE RecLength = RecEnd - RecStart;
  PCHR RecBuffer = new CHR[RecLength + 1];
  GPTYPE ActualLength = fread (RecBuffer, 1, RecLength, fp);
  fclose (fp);
  RecBuffer[ActualLength] = '\0';

  PCHR *tags = parse_tags (RecBuffer, ActualLength);
  if (tags == NULL || tags[0] == NULL)
    {
      STRING doctype;
      NewRecord->GetDocumentType(&doctype);
      if (tags)
	{
	  delete tags;
	  cout << "Warning: No `" << doctype << "' fields/tags in \"" << fn << "\" record.\n";
	}
       else
	{
	  cout << "Unable to parse `" << doctype << "' record in \"" << fn << "\".\n";
	}
      delete [] RecBuffer;
      return;
    }

  FC fc;
  DF df;
  PDFT pdft;
  pdft = new DFT ();
  DFD dfd;
  STRING FieldName;
  // Walk though tags
  for (PCHR * tags_ptr = tags; *tags_ptr; tags_ptr++)
    {
      PCHR p = tags_ptr[1];
      if (p == NULL)
	p = &RecBuffer[RecLength];
      // eg "AB  -" XXXXX
      int off = 5; // Medline format constant
      INT val_start = (*tags_ptr + off) - RecBuffer;
      // Skip ' 's 
      while (isspace(RecBuffer[val_start]))
	val_start++, off++;
      // Also leave off the \n
      INT val_len = (p - *tags_ptr) - off - 1;
      // Strip potential trailing while space
      while (val_len > 0 && isspace (RecBuffer[val_len + val_start]))
	val_len--;

      // Do we have some content?
      if (val_len <= 0) continue; // Skip blank fields

      PCHR unified_name = UnifiedName(*tags_ptr);
#if WANT_MISC
      // Throw "unclassified" into Misc
      FieldName = unified_name ? unified_name: "Misc";
#else
      // Ignore "unclassified" fields
      if (unified_name == NULL) continue; // ignore these
      FieldName = unified_name;
#endif
      dfd.SetFieldName (FieldName);
      Db->DfdtAddEntry (dfd);
      fc.SetFieldStart (val_start);
      fc.SetFieldEnd (val_start + val_len);
      PFCT pfct = new FCT ();
      pfct->AddEntry (fc);
      df.SetFct (*pfct);
      df.SetFieldName (FieldName);
      pdft->AddEntry (df);
      delete pfct;
    }

  NewRecord->SetDft (*pdft);

  // Clean up
  delete pdft;
  delete tags;
  delete [] RecBuffer;
}

void MEDLINE::
Present (const RESULT& ResultRecord,
	 const STRING& ElementSet, PSTRING StringBuffer)
{
  if (ElementSet.Equals(BRIEF_MAGIC))
    {
      // Brief Headline is "Title (Source) Author"
      STRING Tag = UnifiedName("TI");
      STRING Title;
      DOCTYPE::Present (ResultRecord, Tag, &Title);
#if BSN_EXTENSIONS
      Title.CompressSpaces();
#endif

      Tag = UnifiedName("SO");
      STRING Source;
      DOCTYPE::Present (ResultRecord, Tag, &Source);
#if BSN_EXTENSIONS
      Source.CompressSpaces();
#endif

      Tag = UnifiedName("AU");
      STRING Author;
      DOCTYPE::Present (ResultRecord, Tag, &Author);
#if BSN_EXTENSIONS
      Author.CompressSpaces();
#endif

      *StringBuffer = Title;

      if (Source.GetLength())
	{
	  StringBuffer->Cat(" (");
	  StringBuffer->Cat(Source);
	  StringBuffer->Cat(") ");
	}
      StringBuffer->Cat(Author);
    }
  else
    {
      DOCTYPE::Present (ResultRecord, ElementSet, StringBuffer);
#if BSN_EXTENSIONS
      if (! (ElementSet.Equals(FULLTEXT_MAGIC) || ElementSet.Equals(SOURCE_MAGIC)))
	StringBuffer->CompressSpaces();
#endif
    }
}

MEDLINE::~MEDLINE ()
{
}


/*-
   What:        Given a buffer of MEDLINE data:
   returns a list of char* to all characters pointing to the TAG

   Medline Records:
A<tok><tok><tok><sep> ...
| |                 |_____________ Value of field
| |___ A - Z, 0 - 9 or space
|____ A - Z

Tags are left aligned and can be 2, 3 or 4 characters long!

Fields are continued when the line has no tag. A single space
is sufficient in this implementation to guarantee a continuation.

The <sep> (' ' or '-') character is MANDATORY!
-*/
static PCHR *parse_tags (PCHR b, GPTYPE len)
{
  GPTYPE i;
  PCHR *t;			// array of pointers to first char of tags

  size_t tc = 0;		// tag count
#define TAG_GROW_SIZE 64
  size_t max_num_tags = TAG_GROW_SIZE;	// max num tags for which space is allocated

  enum { HUNTING, STARTED, CONTINUING } State = HUNTING;

  /* You should allocate these as you need them, but for now... */
  max_num_tags = TAG_GROW_SIZE;
  t = new PCHR [max_num_tags];

  // Skip leading White space and sep number
  for (i = 0; i < len - 4; i++)
    if (!isspace(b[i]) && !isdigit(b[i])) break;

  while (i < len - 4)
    {
      // Do we have a field?
      if ((State == HUNTING)
	  && (b[i + 4] == '-' || b[i + 4] == ' ')
	  && isalpha (b[i])
	  && isalnum (b[i + 1])
	  && (isalnum (b[i + 2]) || b[i + 2] == ' ')
	  && (isalnum (b[i + 3]) || b[i + 3] == ' ')
	  && (b[i + 2] != ' ' || b[i + 3] == ' ') )
	{

	  t[tc] = &b[i];
	  State = STARTED;
	  // Expand if needed
	  if (++tc == max_num_tags - 1)
	    {
  	      // allocate more space
  	      max_num_tags += TAG_GROW_SIZE;
	      PCHR *New = new PCHR [max_num_tags];
	      if (New == NULL)
		{
		  delete [] t;
		  return NULL; // NO MORE CORE!
		}
	      memcpy(New, t, tc*sizeof(PCHR));
 	      delete [] t;
	      t = New;
	    }
	}
      else if ((State == STARTED) && (b[i] == ' ' || b[i] == '-'))
	{
	  b[i] = '\0';
	  State = CONTINUING;
	}
      else if ((State == CONTINUING) && (b[i] == '\n'))
	{
	  State = HUNTING;
	}
      else if (State == HUNTING)
	State = CONTINUING;
      i++; // increment
    }
  t[tc] = (PCHR) NULL;
  return t;
}
