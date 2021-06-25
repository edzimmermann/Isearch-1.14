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
File:		iafadoc.cxx
Version:	$Revision: 1.2 $
Description:	Class IAFADOC - IAFA documents
Author:		Edward C. Zimmermann, edz@bsn.com
@@@-*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "iafadoc.hxx"

/* #define BSN_EXTENSIONS 1 */

#if BSN_EXTENSIONS < 1 
# define BRIEF_MAGIC "B"
#endif


IAFADOC::IAFADOC (PIDBOBJ DbParent) : COLONDOC (DbParent)
{
}

/* Based upon the IAFA guides to FTP publication from the IAFA-WG
  IAFA DOC I:	Draft 92.10.14
  IAFA DOC II:	Draft 92.10.19
*/
void IAFADOC::
Present (const RESULT & ResultRecord, const STRING & ElementSet, PSTRING StringBuffer)
{
  if (ElementSet.Equals(BRIEF_MAGIC))
    {
      *StringBuffer = "";

      // Brief Headline is some tag (a journey to find one...)
      STRING Tag = "Title";
      STRING name;
      DOCTYPE::Present (ResultRecord, Tag, &name);
      if (name.GetLength () == 0)
	{
	  Tag = "Package-Name";
	  DOCTYPE::Present (ResultRecord, Tag, &name);
	  if (name.GetLength ())
	    StringBuffer->Cat ("Package: ");
	  else
	    {
	      Tag = "Service-Name";
	      DOCTYPE::Present (ResultRecord, Tag, &name);
	      if (name.GetLength())
		StringBuffer->Cat("Service: ");
	      else
		{
		  Tag = "Preferred-Name";
		  DOCTYPE::Present (ResultRecord, Tag, &name);
		  if (name.GetLength())
		    StringBuffer->Cat("Site-Info: ");
		  else
		    {
		      Tag = "Mailinglist-Name";
		      DOCTYPE::Present (ResultRecord, Tag, &name);
		      if (name.GetLength())
			StringBuffer->Cat ("Mailing-list: ");
		      else
			{
			  Tag = "Newsgroup-Name";
			  DOCTYPE::Present (ResultRecord, Tag, &name);
			  if (name.GetLength())
			    StringBuffer->Cat ("Newsgroup: ");
			}
		    }
		}
	    }
	}

      if (name.GetLength ())
	{
	  Tag = "Author";
	  STRING Author;
	  DOCTYPE::Present (ResultRecord, Tag, &Author);
	  if (Author.GetLength() == 0)
	    {
	      Tag = "Authors";
	      DOCTYPE::Present (ResultRecord, Tag, &Author);
	    }
#if BSN_EXTENSIONS
	  Author.CompressSpaces ();
#endif

	  StringBuffer->Cat (name);
	  if (Author.GetLength ())
	    {
	      StringBuffer->Cat (" (");
	      StringBuffer->Cat (Author);
	      StringBuffer->Cat (") ");
	    }
	}
      else
	{
	  // No Name so use bits of the Description...
	  Tag = "Description";
	  DOCTYPE::Present (ResultRecord, Tag, &name);
	  if (name.GetLength())
	    {
	      STRINGINDEX x = name.Search('\n');
	      if (x) name.EraseAfter(x-1);
#if BSN_EXTENSIONS
	      name.CompressSpaces();
#endif
#define CUT_OFF 67
	      if (name.GetLength() >= CUT_OFF)
		{
		  name.EraseAfter(CUT_OFF-3); 
		  name.Cat ("...");
		}
	      else if (x)
		name.Cat ("...");
	      StringBuffer->Cat("Description: ");
	      StringBuffer->Cat(name);
	    }
	  else
	    {
	      // Ugh... just present anything...
	      DOCTYPE::Present (ResultRecord, ElementSet, StringBuffer);
	    }
	}
    }
  else
    {
      DOCTYPE::Present (ResultRecord, ElementSet, StringBuffer);
    }

}

IAFADOC::~IAFADOC ()
{
}
