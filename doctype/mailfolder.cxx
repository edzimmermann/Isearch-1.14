static const char RCS_Id[]="$Id: mailfolder.cxx,v 1.2 1996/07/10 19:24:49 cnidr Exp $";
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
File:		mailfolder.cxx
Version:	$Revision: 1.2 $
Description:	Class MAILFOLDER - Unix mail folder Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Distribution:   Isite modifications by A. Warnock (warnock@clark.net)
@@@-*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "mailfolder.hxx"
#include "doc_conf.hxx"
#if BSN_EXTENSIONS
// BSn's Isearch supports date in the Multiple Document Table.
#include <time.h>
extern time_t date_parse(char *);
#endif



/*-
 MAILFOLDER expects the folder formats:

 From blah ... or Article NN of XXX.XXX.XXX 
 XXX: Blah, Blah, Blah
 .
 .
 .
 <blank line>
 <message body>
 <blank line>
 From blah ...  or Article NNN of XXX.XXX.XXX
 XXX: Blah, Blah, Blah
 .
 .
 .
 <blank link>
 <message body>
 .
 .
 .

 Applicable XXX: fields are stored under XXX and the
 message body is stored under the "message-body" field
 name.

Please Note:
-----------

Right now only RFC822 is (I hope) correctly handled.
The following are not yet correctly handled:
  1) Base64, BinHex, quoted-printable,.. encodings
  2) Character sets (assumed same as locale)
  3) Multiple record messages (Sun Attachments, MIME etc)
  4) UUencoded, btoa'd etc binary messages
  5) Face signatures.
In other words, right now the body of messages is assumed
to contain text in the character set of the host platform.

-*/


// Loads of possible tags are in mail headers
// but we will ONLY use the following for
// tags.
GDT_BOOLEAN MAILFOLDER::accept_tag(const PCHR tag) const
{
#if RESTRICT_MAIL_FIELDS
  // Mail tags that we want, if it is not
  // here then we igonre it.
  static char * Keywords[] = {
    /* Must be sorted! */
    "Bcc",
    "Cc",
    "Comments", 	// added Thu Jul 27 02:23:22 MET DST 1995
    "Content-Transfer-Encoding", // added Wed Oct  4 23:07:32 MET 1995
    "Content-Type",
    "Date",
    "Encrypted",	// added Thu Jul 27 02:23:22 MET DST 1995
    "Expires",
    "Followup-To",
    "From",
    "In-Reply-To",
    "Keywords",
    "Message-ID",
    "Message-Id",
    "Newsgroup",	// Sometimes used in Mail
    "Newsgroups",	// This is used in Usenet News
    "Organisation",
    "Organization",
    "Originator", // Added to support some mailing lists
    "Priority",
    "References",
    "Reply-To",
    "Sender",
    "Subject",
    "Summary",
    "To",
    "X-Sun-Charset", // added Wed Oct  4 23:07:32 MET 1995
    "X-To",
    "X-URL"
  };

  int n = 0, i = 0;

  if (tag && *tag)
    do {
      n = strcmp(tag, Keywords[i]);
    } while (++i < sizeof(Keywords)/sizeof(Keywords[0]) && n > 0);
  return n == 0 ? GDT_TRUE : GDT_FALSE;
#else
  // Anything goes
  return GDT_TRUE;
#endif
}



MAILFOLDER::MAILFOLDER (PIDBOBJ DbParent): DOCTYPE (DbParent)
{
}

void MAILFOLDER::AddFieldDefs ()
{
  DOCTYPE::AddFieldDefs ();
}

void MAILFOLDER::ParseRecords (const RECORD& FileRecord) 
{
  // Break up the document into Mail message records
  GPTYPE Start = 0;
  GPTYPE Position = 0;
  GPTYPE SavePosition = 0;
  GPTYPE RecordEnd;

  STRING Fn;
  FileRecord.GetFullFileName (&Fn);
  PFILE Fp = fopen (Fn, "rb");
  if (!Fp)
    {
      cout << "Could not access '" << Fn << "'\n";
      return;			// File not accessed

    }

  char buf[512]; // was 256

  // Read first line of folder
  if (fgets(buf, sizeof(buf)/sizeof(char)-1, Fp) == NULL)
    {
      fclose(Fp);
      return; // EMPTY Folder
    }
  // Try to guess folder type
  enum {MAIL, NEWS, MAIL_OR_NEWS} folder;
  if (IsMailFromLine(buf))
    folder = MAIL; // Expect mail folder
  else if (IsNewsLine(buf))
    folder = NEWS; // Expect News folder
  else
    folder = MAIL_OR_NEWS;

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

  // Read lines from file and search for record seperation
  GDT_BOOLEAN Look=GDT_TRUE;
  do
    {
      /*
	Search for "magic" lines: A blank line followed by a legal
	Email "from ", resp. News "Article ", line
       */
      if (Look && ((folder != NEWS && IsMailFromLine(buf)) || (folder != MAIL && IsNewsLine(buf))))
	{
	  SavePosition = Position;
	  Record.SetRecordStart (Start);
	  RecordEnd = (SavePosition == 0) ? 0 : SavePosition - 1;

	  if (RecordEnd > Start)
	    {
	      Record.SetRecordEnd (RecordEnd);
	      Db->DocTypeAddRecord(Record);
	      Start = SavePosition;
	    }
	}
      Look = (buf[0] == '\n' || buf[0] == '\r') ? GDT_TRUE : GDT_FALSE;
      Position += strlen(buf);
    }
  while (fgets(buf,sizeof(buf)/sizeof(char)-1,Fp) != NULL);

  fclose (Fp);

  Record.SetRecordStart (Start);
  RecordEnd = Position - 1;

  if (RecordEnd > Start)
    {
      Record.SetRecordEnd (RecordEnd);
      Db->DocTypeAddRecord(Record);
    }
}


void MAILFOLDER::ParseFields (PRECORD NewRecord)
{
  STRING fn;
  // TODO: Cache file name and stream pointer
  NewRecord->GetFullFileName (&fn);
  PFILE fp = fopen (fn, "rb");
  if (!fp)
    {
      return;		// ERROR
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
      if (!accept_tag(*tags_ptr))
	continue;
#if BSN_EXTENSIONS
      if (strncmp(*tags_ptr, "Date", 4) == 0) {
	time_t date = date_parse(*tags_ptr + 5);
	NewRecord->SetDate(date);
      }
#endif
#if 0
      // Some warning...
      if (strncmp(*tags_ptr, "Content-Type", 12) == 0)
	{
	  if (strstr(*tags_ptr, "X-sun-attachment")) 
	    cout << "MAILFOLDER: record in \"" << fn << "\" is " << *tags_ptr << "\n";
	}
      else if (strncmp(*tags_ptr, "Content-Transfer-Encoding", 12) == 0)
	{
	  cout << "MAILFOLDER: record in \"" << fn << "\" is " << *tags_ptr << "\n";
	}
#endif
      PCHR p = tags_ptr[1];
      if (p == NULL)
	p = &RecBuffer[RecLength];
      // eg "From:"
      int off = strlen (*tags_ptr) + 1;
      INT val_start = (*tags_ptr + off) - RecBuffer;
      // Skip while space 
      while (isspace (RecBuffer[val_start]))
	val_start++, off++;
      // Also leave off the \n
      INT val_len = (p - *tags_ptr) - off - 1;
      // Strip potential trailing while space
      while (val_len > 0 && isspace (RecBuffer[val_len + val_start]))
	val_len--;

      if ((*tags_ptr)[0] == '\0' || (*tags_ptr)[0] == '\n')
	FieldName = "Message-body";
      else if (strcmp("Organization", *tags_ptr) == 0)
	FieldName = "Organisation"; // Ocean kludge
      else if (strcmp("Newsgroup", *tags_ptr) == 0)
	FieldName = "Newsgroups"; // Compat. kludge
      else
	FieldName = *tags_ptr;
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
  delete pdft;
  delete[]RecBuffer;
  delete tags;
}

/*-
 * Find Author's name in mail address
 * In "XXX (YYY)" or YYY <XXX>" return "YYY"
 * Find Author's address in mail address
 * In "XXX (YYY)" or YYY <XXX>" return "XXX"
 */
PCHR MAILFOLDER::NameKey (PCHR buf, GDT_BOOLEAN name) const
{
  PCHR s = NULL;
  PCHR e = NULL;
  char email[256];
  char p1, p2, b1, b2;

  if (name)
    {
      p1 = '('; p2 = ')';
      b1 = '<'; b2 = '>';
    }
  else
    {
      p1 = '<'; p2 = '>';
      b1 = '('; b2 = ')';
    }

  strcpy (email, buf);
  if (((s = strchr (email, p1)) != NULL) && ((e = strchr (email, p2)) != NULL))
    {
      if (e > s)
	{
	  *e = '\0';		/* Chop off everything after p2 (')' or '>') */
	  strcpy (email, s + 1);
	}
    }
  else if (((s = strchr (email, b1)) != NULL) && ((e = strchr (email, b2)) != NULL))
    {
      if (e > s)
	strcpy (s, e + 1);	/* Remove <...> or (...) */
    }
  s = email;
  while(isspace(*s) || *s == '"') s++; // Skip leading space
  strcpy (buf, s);
  s = buf + strlen(buf) - 1;
  while (s > buf && (isspace(*s) || *s == '"')) *s-- = '\0'; // Trim trailing space
  return buf;
}


void MAILFOLDER::Present (const RESULT& ResultRecord,
  const STRING& ElementSet, const STRING& RecordSyntax,
  PSTRING StringBuffer)
{
  if (ElementSet.Equals(BRIEF_MAGIC))
    {
      // Brief Headline is "From: Subject"
      STRING SubjectTag = "Subject";
      STRING SubjectValue;
      DOCTYPE::Present (ResultRecord, SubjectTag, RecordSyntax, &SubjectValue);
      STRING FromTag = "From";
      STRING FromValue;
      DOCTYPE::Present (ResultRecord, FromTag, RecordSyntax, &FromValue);
      char buf[256];
      FromValue.GetCString(buf, sizeof(buf)/sizeof(char) - 1);
      *StringBuffer = NameKey(buf);
#if SHOW_MAIL_DATE
#if BSN_EXTENSIONS
      time_t date = (time_t)ResultRecord.GetDate();
      strftime(buf, sizeof(buf)/sizeof(char)-1, " %D", gmtime(&date));
      StringBuffer->Cat(buf);
#endif
#endif
      StringBuffer->Cat(": ");
      StringBuffer->Cat(SubjectValue);
    }
  else
    {
      DOCTYPE::Present (ResultRecord, ElementSet, RecordSyntax, StringBuffer);
    }
}

MAILFOLDER::~MAILFOLDER ()
{
}



/*-
 * IsMailFromLine - Is this a legal unix mail "From " line?
 *
 * We check to make sure that the line has the format:
 * From <addr> <Day> <Month> <Day#> <hour>:<min.>[:<sec.>] [<TZ>] [<TZ mod>] <year>
 *
 * Examples of acceptable "from " lines:
 *    From foo@bar Fri Sep 30 21:15:05 1994
 *    From foo@bar Fri Sep 30 21:15:05 MET 1994
 *    From foo@bar Fri Sep 30 21:15:05 MET DST 1994
 *    From foo@bar Fri Sep 30 21:15 MET 1994
 *    From foo@bar Fri Sep 30 21:15 1994
 * as well as these odd-balls (never seen 'em):
 *    From foo@bar Fri Sep 30 21:15:05 +0200 1994
 *    From foo@bar Fri Sep 30 21:15:05 UTC +0200 1994
 *    From foo@bar Fri Sep 30 21:15:05 EET -0100 1994
 *
 */
GDT_BOOLEAN MAILFOLDER::IsMailFromLine (const char *line) const
{
  static char magic[] = "From "; // Mail magic

#define MAX_FIELDS 10
  char *fields[MAX_FIELDS];
  const char *sender_tail;
  register const char *lp;
  register char **fp;
  register int n, i;
  // Email (RFC822) has English language dates from 1 Jan 1970 on
  static char legal_day[] = "SunMonTueWedThuFriSat";
  static char legal_month[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  static int legal_numbers[] = {1, 31, 0, 23, 0, 59, 0, 60, 1969, 2199};
  //                           Month   Hour   Min    Sec     Year

  // Looking at a "magic" line?
  if (strncmp(line, magic, sizeof(magic)/sizeof(char) - 1) != 0)
    return GDT_FALSE;

  // Move past magic
  lp = line + (sizeof(magic)/sizeof(char) - 1);

  /*
     Break up the line into fields
  */ 

  /* sender day mon dd hh:mm:ss year */
  for (n = 0, fp = fields; n < MAX_FIELDS; n++)
    {
      while (*lp && *lp != '\n' && isspace (*lp))
	lp++;
      if (*lp == '\0' || *lp == '\n')
	break;
      *fp++ = (char *)lp;
      while (*lp && !isspace (*lp))
	if (*lp++ == ':' && (n == 4 || n == 5))
	  break;
      if (n == 0)
	sender_tail = lp;
    }

  // Minimum number of fields?
  if (n < 7) return GDT_FALSE;

  fp = fields;

  /* 
     Expect:
	field 0: sender
	field 1: day_of_week
	field 2: month_name
	field 3: day_of_month
	field 4: dd
	field 5: hh
	field 6: ss
	field 7: year
     So we must mangle a bit...
  */ 

  if (n == 7)
    {
      if (!isdigit(fp[6][0])) return GDT_FALSE;
      // From someone Tue Sep 19 15:33 1995
      fp[7] = fp[6]; // year
      fp[6] = "00"; // missing seconds
    }
  else if (n == 8 && !isdigit(fp[6][0]))
    fp[6] = "00";		/* ... hh:mm TZ year */
  if (n > 8 && !isdigit (fp[7][0]))
    fp[7] = fp[8];		/* ... hh:mm:ss TZ year */
  if (n > 9 && !isdigit (fp[7][0]))
    fp[7] = fp[9];		/* ... hh:mm:ss TZ DST year */

  // Check the day (Field 1)-- 7 days in a week
  fp++;
  const size_t dlen = (sizeof(legal_day)/sizeof(char) -1);
  const size_t dtoklen = dlen/7; /* Length of a day name */
  for (i = 0; i < dlen; i += dtoklen)
    if (strncmp (*fp, &legal_day[i], dtoklen) == 0)
      break;
  if (i == dlen) return GDT_FALSE; // Not a legal day

  // Check the month (Field 2)-- 12 months in a year
  fp++;
  const size_t mlen = (sizeof(legal_month)/sizeof(char) - 1);
  const size_t mtoklen = mlen/12; /* Length of a month name */
  for (i = 0; i < mlen; i += mtoklen)
    if (strncmp (*fp, &legal_month[i], mtoklen) == 0)
      break;
  if (i == mlen) return GDT_FALSE; // Not a legal month

  // Check the numbers (bounds condition)-- field 3 to 7
  for (i = 0; i < (sizeof(legal_numbers)/sizeof(legal_numbers[0])); i += 2)
    {
      lp = *++fp;
      if (!isdigit (*lp)) {
	return GDT_FALSE;
      }
      n = atoi (lp);
      if (n < legal_numbers[i] || legal_numbers[i + 1] < n) {
	 return GDT_FALSE;
      }
    }

  // Looks good
  return GDT_TRUE;
#undef MAX_FIELDS
}

/*-
 * Start of News:
 * "Article <Number> of <Newsgroup>:"
 */
GDT_BOOLEAN MAILFOLDER::IsNewsLine (const char *line) const
{
  static char magic[] = "Article ";

  // News folder Magic?
  if (strncmp (line, magic, sizeof(magic)/sizeof(char) - 1) != 0)
    return GDT_FALSE;

  // Move past magic
  line += (sizeof(magic)/sizeof(char) - 1);
  /* Skip white space */
  while (isspace (*line)) line++;

  if (!isdigit (*line)) return GDT_FALSE; // No #
  if (atoi (line) < 1 ) return GDT_FALSE; // Illegal Number
  /* skip number data */
  while (isdigit (*line)) line++;

  if (!isspace (*line)) return GDT_FALSE;
  /* Skip white space */
  while (isspace (*line)) line++;

  if (line[0] != 'o' || line[1] != 'f') return GDT_FALSE; // Missing "of"
  /* Skip the of */
  line += 2;

  if (!isspace (*line)) return GDT_FALSE;
  /* Skip white space */
  while (isspace (*line)) line++;

  if (*line == '\0') return GDT_FALSE; // Missing newsgroup name

  /* OK, if was "Article NNN of XXX.XXX.XXXXX:" */
  return GDT_TRUE;
}


PCHR * MAILFOLDER::parse_tags (PCHR b, GPTYPE len) const
{
  PCHR *t;			// array of pointers to first char of tags
  size_t tc = 0;		// tag count
#define TAG_GROW_SIZE 32
  size_t max_num_tags = TAG_GROW_SIZE;	// max num tags for which space is allocated
  enum { HUNTING, STARTED, CONTINUING } State = HUNTING;

  // Skip leading bogus white space
  while (isspace(*b)) b++;
  // Is it a mail or news folder?
  if (IsMailFromLine(b) || IsNewsLine(b)) { 
    // Now skip the From/Article line
    while (*b != '\n') b++; // looking at end of line
    while (*b == '\n') b++; // Looking at first character
    // Should now be looking at fist tag line
  }

  /* You should allocate these as you need them, but for now... */
  max_num_tags = TAG_GROW_SIZE;
  t = new PCHR[max_num_tags];
  for (GPTYPE i = 0; i < len; i++)
    {
      if (State == HUNTING)
	{
	  t[tc] = &b[i];
	  State = STARTED;
	  if (b[i] == '\n' || b[i] == '\r')
	   {
	      b[i] = '\0';
	      tc++;
	      break; // looking at body, done
	   }

	}
      else if ((State == STARTED) && (b[i] == ' ' || b[i] == '\t'))
	{
	  State = CONTINUING;
	}
      else if ((State == STARTED) && (b[i] == ':'))
	{
	  b[i] = '\0';
	  // Expand memory if needed
	  if (++tc == max_num_tags - 1)
	    {
  	      // allocate more space
  	      max_num_tags += TAG_GROW_SIZE;
	      PCHR *New = (CHR**)(new PCHR[max_num_tags]);
	      if (New == NULL)
		{
		  delete [] t;
		  return NULL; // NO MORE CORE!
		}
	      memcpy(New, t, tc*sizeof(PCHR));
 	      delete [] t;
	      t = New;
	    }
	  State = CONTINUING;
	}
      else if ((State == CONTINUING) && (b[i] == '\n'))
	{
	  State = HUNTING;
	}
    }
  t[tc] = (PCHR) NULL;
  return t;
}
