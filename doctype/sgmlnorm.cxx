/************************************************************************
Copyright Notice

This software was based upon a concept from CNIDR substantially extended,
expanded and modified by BSn. This implementation is from and the
intellectual property of BSn and is covered by one or more copyrights.

----
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

_________________________________________________________________________
(*)Basis Systeme netzwerk, Brecherspitzstr. 8, 81541 Muenchen, Germany 

--

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

/*-@@@
File:		sgmlnorm.cxx
Version:	$Revision: 1.3 $
Description:	Class SGMLNORM - Normalized SGML Document Type
Authors:   	Kevin Gamiel, Kevin.Gamiel@cnidr.org
       		Edward C. Zimmermann, edz@bsn.com
Copyright:	BSn/CNIDR
@@@-*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "common.hxx"
#include "doctype.hxx"
#include "sgmlnorm.hxx"

#define ACCEPT_EMPTY_TAGS	0	/* 1 ==> Accept Empty tags per Annex C.1.1.1 SGML Handbook */


SGMLNORM::SGMLNORM (PIDBOBJ DbParent): DOCTYPE (DbParent)
{
}

void SGMLNORM::ParseRecords (const RECORD& FileRecord)
{
  DOCTYPE::ParseRecords (FileRecord);
}



PCHR SGMLNORM::UnifiedName (PCHR tag) const
{
  return tag;
}


void SGMLNORM::ParseFields (PRECORD NewRecord)
{
  PFILE fp;
  STRING fn;

  if (NewRecord == NULL) return; // Error

  // Open the file
  NewRecord->GetFullFileName (&fn);
  if (!(fp = fopen (fn, "rb")))
    return;			// ERROR

  GPTYPE RecStart = NewRecord->GetRecordStart ();
  GPTYPE RecEnd = NewRecord->GetRecordEnd ();
  if (RecEnd == 0)
    {
      fseek (fp, 0, SEEK_END);
      RecStart = 0;
      RecEnd = ftell (fp);
    }
  fseek (fp, RecStart, SEEK_SET);

  // Read the whole record in a buffer
  GPTYPE RecLength = RecEnd - RecStart;
  PCHR RecBuffer = new CHR[RecLength + 1];
  GPTYPE ActualLength = (GPTYPE) fread (RecBuffer, 1, RecLength, fp);
  RecBuffer[ActualLength] = '\0';	// ASCIIZ

  fclose (fp);

  STRING FieldName;
  FC fc;
  DF df;
  DFD dfd;

  STRING doctype;
  NewRecord->GetDocumentType(&doctype);

  PCHR *tags = parse_tags (RecBuffer, ActualLength);
  if (tags == NULL)
    {
      cout << "Unable to parse `" << doctype << "' tags in file " << fn << "\n";
      // Clean up
      delete[]RecBuffer;
      return;
    }


  PDFT pdft = new DFT ();
  for (PCHR *tags_ptr = tags; *tags_ptr; tags_ptr++)
    {
      if ((*tags_ptr)[0] == '/')
	continue;

      const PCHR p = find_end_tag (tags_ptr, *tags_ptr);
      size_t tag_len = strlen (*tags_ptr);
      int have_attribute_val = (NULL != strchr (*tags_ptr, '='));

      if (p != NULL)
	{
	  // We have a tag pair
	  size_t val_start = (*tags_ptr + tag_len + 1) - RecBuffer;
	  int val_len = (p - *tags_ptr) - tag_len - 2;

	  // Skip leading white space
	  while (isspace (RecBuffer[val_start]))
	    val_start++, val_len--;
	  // Leave off trailing white space
	  while (val_len > 0 && isspace (RecBuffer[val_start + val_len - 1]))
	    val_len--;

	  // Don't bother storing empty fields
	  if (val_len > 0)
	    {
	      // Cut the complex values from field name
	      CHR orig_char = 0;
	      char* tcp;
	      for (tcp = *tags_ptr; *tcp; tcp++)
		if (isspace (*tcp))
		  {
		    orig_char = *tcp;
		    *tcp = '\0';
		    break;
		  }
#if ACCEPT_EMPTY_TAGS
	      if (*tags_ptr[0] == '\0')
		{
		  // Tag name is the name of the last element
		  if (FieldName.GetLength () == 0)
		    {
		      // Give some information
		      cout << doctype << " Warning: \""
			<< fn << "\" offset " << (*tags_ptr - RecBuffer) << ": "
			<< "Bad use of empty tag feature, skipping field.\n";
		      continue;
		    }
		}
	      else
		{
#endif
		  PCHR unified_name = UnifiedName(*tags_ptr);
		  // Ignore "unclassified" fields
		  if (unified_name == NULL) continue; // ignore these
		  FieldName = unified_name;
//		  FieldName.UpperCase(); // Store SGML tags uppercase (not needed yet)
#if ACCEPT_EMPTY_TAGS
		}
#endif
	      if (orig_char)
		*tcp = orig_char;

	      dfd.SetFieldName (FieldName);
	      Db->DfdtAddEntry (dfd);
	      fc.SetFieldStart (val_start);
	      fc.SetFieldEnd (val_start + val_len - 1);
	      PFCT pfct = new FCT ();
	      pfct->AddEntry (fc);
	      df.SetFct (*pfct);
	      df.SetFieldName (FieldName);
	      pdft->AddEntry (df);
	      delete pfct;
	    }
	}
      if (have_attribute_val)
	{
	  store_attributes (pdft, RecBuffer, *tags_ptr);
	}
      else if (p == NULL)
	{
#if 1
	  // Give some information
	  cout << doctype << " Warning: \""
	    << fn << "\" offset " << (*tags_ptr - RecBuffer) << ": "
	    << "No end tag for <" << *tags_ptr << "> found, skipping field.\n";
#endif
	}
    }

  NewRecord->SetDft (*pdft);

  // Clean up;
  delete tags;
  delete pdft;
  delete[]RecBuffer;
}


void SGMLNORM::
Present (const RESULT& ResultRecord, const STRING& ElementSet, PSTRING StringBuffer)
{
#if 0 /* Not needed yet since Isearch still stores field names uppercase for all doctypes  */
  STRING ESet = ElementSet;
  ESet.UpperCase();
  DOCTYPE::Present (ResultRecord, ESet, StringBuffer);
#else
  DOCTYPE::Present (ResultRecord, ElementSet, StringBuffer);
#endif
}

SGMLNORM::~SGMLNORM ()
{
}

/*----------------------------------------------------------------------------------*/
/*------------- Support Functions used by both SGMLTAG and HTML classes ------------*/
/*----------------------------------------------------------------------------------*/

/* Collect all the <tag foo=bar ..> stuff as tag:foo ...  */
void SGMLNORM::store_attributes (PDFT pdft, PCHR base_ptr, PCHR tag_ptr) const
{
#if BSN_EXTENSIONS
  const CHR Sep = ATTRIB_SEP;	// Magic sep. tag:attribute
#else
  const CHR Sep = '@';
#endif

  PCHR name, val, attribute;
  enum STATES
    {
      INIT, OK, COLLECT, QUOTE, SQUOTE
    }
  State = INIT, OldState = INIT;
  size_t val_start, val_end;

  if (tag_ptr == NULL)
    return;

  // Skip Leading Space
  while (isspace (*tag_ptr))
    tag_ptr++;
  name = tag_ptr;
  if (!isalpha (*name))
    return;			// Not NAMEFIRST character

  val = attribute = NULL;

  DFD dfd;
  DF df;
  FC fc;
  STRING FieldName;

  while (*tag_ptr)
    {
      if (*tag_ptr == '"')
	{
	  if (State == QUOTE)
	    {
	      State = OldState;
	    }
	  else
	    {
	      OldState = State;
	      State = QUOTE;
	    }
	}
      else if (*tag_ptr == '\'')
	{
	  if (State == SQUOTE)
	    {
	      State = OldState;
	    }
	  else
	    {
	      OldState = State;
	      State = SQUOTE;
	    }
	}
      else if (*tag_ptr == '=')
	{
	  if (State == OK)
	    {
	      *tag_ptr++ = '\0';
	      State = COLLECT;
	      /* Skip White space */
	      while (isspace (*tag_ptr))
		tag_ptr++;
	      val = tag_ptr--;	// Backup

	    }
	}
      else if (isspace (*tag_ptr))
	{
	  if (State == INIT)
	    {
	      // Store whole bit
	      *tag_ptr++ = '\0';
	      State = OK;
	      val_start = tag_ptr - base_ptr;
	      val_end = val_start + strlen (tag_ptr) - 1;
	      FieldName = name;
	      FieldName.Cat (Sep);
//	      FieldName.UpperCase(); // Store SGML tags uppercase (not needed yet)
	      dfd.SetFieldName (FieldName);
	      Db->DfdtAddEntry (dfd);
	      fc.SetFieldStart (val_start);
	      fc.SetFieldEnd (val_end);
	      PFCT pfct = new FCT ();
	      pfct->AddEntry (fc);
	      df.SetFct (*pfct);
	      df.SetFieldName (FieldName);
	      pdft->AddEntry (df);
	      delete pfct;
	    }
	  else if (State == COLLECT)
	    {
	      *tag_ptr++ = '\0';
	      State = OK;
	      val_start = val - base_ptr;
	      val_end = strlen (val) + val_start - 1;
	      FieldName = name;
	      FieldName.Cat (Sep);
	      FieldName.Cat (attribute);
//	      FieldName.UpperCase(); // Store SGML tags uppercase (not needed yet)
	      dfd.SetFieldName (FieldName);
	      Db->DfdtAddEntry (dfd);
	      fc.SetFieldStart (val_start);
	      fc.SetFieldEnd (val_end);
	      PFCT pfct = new FCT ();
	      pfct->AddEntry (fc);
	      df.SetFct (*pfct);
	      df.SetFieldName (FieldName);
	      pdft->AddEntry (df);
	      delete pfct;
	    }
	  /* Skip Repeated White space */
	  while (isspace (*tag_ptr))
	    tag_ptr++;
	  if (State != QUOTE && State != SQUOTE)
	    val = attribute = tag_ptr--;
	}
      tag_ptr++;
    }				/* while (*tag_ptr) */
  /* Rest attribute/value pair */
  if (attribute && attribute != val)
    {
      val_start = val - base_ptr;
      val_end = val_start + strlen (val) - 1;
      FieldName = name;
      FieldName.Cat (Sep);
      FieldName.Cat (attribute);
//    FieldName.UpperCase(); // Store SGML tags uppercase (not needed yet)
      dfd.SetFieldName (FieldName);
      Db->DfdtAddEntry (dfd);
      fc.SetFieldStart (val_start);
      fc.SetFieldEnd (val_end);
      PFCT pfct = new FCT ();
      pfct->AddEntry (fc);
      df.SetFct (*pfct);
      df.SetFieldName (FieldName);
      pdft->AddEntry (df);
      delete pfct;
    }
}


/*
   What:        Given a buffer of sgml-tagged data:
   It searches for "tags" and
   (1) returns a list of char* to all characters immediately
   following each '<' character in the buffer.

   Pre: b = character buffer with valid sgml marked-up text
   len = length of b
   tags = Empty char**

   Post:        tags is filled with char pointers to first character of every sgml 
   tag (first character after the '<').  The tags array is 
   terminated by a NULL.
   Returns the total number of tags found or -1 if out of memory
 */
PCHR *SGMLNORM::parse_tags (PCHR b, GPTYPE len) const
{
  PCHR *t;			/* array of pointers to first char of tags */
  size_t tc = 0;		/* tag count */
  size_t max_num_tags;		/* max num tags for which space is allocated */
  int bracket = 0;		/* Declaration bracket depth */
  enum
    {
      OK, NEED_END, IN_DECL
    }
  State = OK;

#ifndef TAG_GROW_SIZE
#define TAG_GROW_SIZE 128
#endif
  const int grow_size = TAG_GROW_SIZE;
#undef TAG_GROW_SIZE

  // You should allocate character pointers (to tags) as you need them.
  // Start with TAG_GROW_SIZE of them.
  max_num_tags = grow_size;
  t = new PCHR [max_num_tags];

  // Step though every character in the buffer looking for '<' and '>'
  for (GPTYPE i = 0; i < len; i++)
    {
      switch (b[i])
	{
	case '[':
	  if (State == IN_DECL)
	    bracket++;
	  break;
	case ']':
	  if (State == IN_DECL)
	    if (--bracket <= 0)
	      bracket = 0;
	  break;

	case '>':
	  if (State == IN_DECL && bracket == 0)
	    State = OK;
	  else if (State == NEED_END)
	    {
	      State = OK;
	      b[i] = '\0';
	      // Expand memory if needed
	      if (++tc == max_num_tags - 1)
		{
		  // allocate more space
		  max_num_tags += grow_size;
		  PCHR *New = new PCHR [max_num_tags];
		  if (New == NULL)
		    {
		      delete[]t;
		      return NULL;	// NO MORE CORE!
		    }
		  memcpy (New, t, tc * sizeof (PCHR));
		  delete[]t;
		  t = New;
		}
	    }
	  break;

	case '<':
	  // Is the tag a comment or control?
	  if (b[i + 1] == '!')
	    {
	      /* The SGML was not parsed! */
	      i++;
	      if (b[i + 1] == '-' && b[i + 2] == '-')
		{
		  // SGML comment <!-- blah blah ... -->
		  while (i < len)
		    {
		      if (b[i++] != '-') continue;
		      if (b[i++] != '-') continue;
		      if (b[i++] != '>') continue;
		      break;	// End of comment found
		    }		// while
		}
	      else		// Declaration <!XXX [ <...> <...> ]>
		{
		  State = IN_DECL;
		}
	    }			// if <!

	  else if (State == OK)
	    {
	      // Skip over leading white space
	      do
		{
		  i++;
		}
	      while (isspace (b[i]));
	      t[tc] = &b[i];	// Save tag
#if ACCEPT_EMPTY_TAGS
	      if (b[i] == '>')
		i--;		// empty tag so back up.. 
#endif
	      State = NEED_END;
	    }
	  break;

	default:
	  break;
	}			// switch

    }				// for

  // Any Errors underway?
  if (State != OK)
    {
      delete[]t;
      return NULL;		// Parse ERROR
    }

  t[tc] = (PCHR) NULL; // Mark end of list
  return t;
}


/*
   Searches through string list t look for "/" followed by tag, e.g. if
   tag = "TITLE REL=XXX", looks for "/TITLE" or a empty end tag (</>).

   Pre: t is is list of string pointers each NULL-terminated.  The list
   should be terminated with a NULL character pointer.

   Post: Returns a pointer to found string or NULL.
 */
const PCHR SGMLNORM::find_end_tag (char *const *t, const char *tag) const
{
  size_t len;
  if (t == NULL || *t == NULL)
    return NULL;		// Error

  if (*t[0] == '/')
    return NULL;		// I'am confused!

  // Look for "real" tag name
  for (len = 0; tag[len]; len++)
    if (isspace (tag[len]))
      break;			// Have length

  size_t i = 0;
  const char *tt = *t;
  do
    {
      if (tt[0] == '/')
	{
#if ACCEPT_EMPTY_TAGS
	  // for empty end tags: see SGML Handbook Annex C.1.1.1, p.68
	  if (tt[1] == '\0' || tag[0] == '\0')
	    return (const PCHR) tt;
#endif

	  if (tt[1 + len] != '\0' && !isspace (tt[1 + len]))
	    continue;		// Nope

	  // SGML tags are case INDEPENDENT
	  if (0 == StrNCaseCmp (&tt[1], tag, len))
	    return (const PCHR) tt;	// Found it

	}
    }
  while ((tt = t[++i]) != NULL);

#if 0
  // No end tag, assume that the document was valid
  // and the end-tag is implicit with the start of the
  // next tag
  return t[1];
#else
  return NULL;			// No end tag found
#endif
}
