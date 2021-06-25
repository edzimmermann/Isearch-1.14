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
File:		simple.cxx
Version:	1.00
Description:	Class SIMPLE - Simple headline document type
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <ctype.h>
#include "simple.hxx"

#define HTML_OID "1.2.840.10003.5.1000.34.1"

SIMPLE::SIMPLE(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
	// Read doctype options
	STRLIST StrList;
	STRING S;
	Db->GetDocTypeOptions(&StrList);
	StrList.GetValue("LINES", &S);
	NumLines = S.GetInt();
	if (NumLines < 1) {
		NumLines = 1;
	}
}
void SIMPLE::BeforeRset(const STRING& RecordSyntax) {
  if (RecordSyntax.Equals(HTML_OID))
    cout << "<pre>" << endl;
}

void SIMPLE::Present(const RESULT& ResultRecord, const STRING& ElementSet, 
		PSTRING StringBuffer) {
  if (ElementSet.Equals("B")) {
    // Return first non-empty line of text
    *StringBuffer = "";
    ResultRecord.GetRecordData(StringBuffer);
    INT x = 1;
    while (isspace(StringBuffer->GetChr(x))) {
      x++;	// Loop past non-alphanumeric characters
    }
    STRING *Headline;
    Headline = new STRING;
    INT y, z = x;
    STRING S;
    UCHR c;
    for (y=1; y<=NumLines; y++) {
      while( ( c = StringBuffer->GetChr(z) ) && c != '\n' ) {
	z++;
	*Headline += c;
      }
    }
    *StringBuffer = *Headline;
    delete Headline;
  } else {
    DOCTYPE::Present(ResultRecord, ElementSet, StringBuffer);
  }
}

void SIMPLE::AfterRset(const STRING& RecordSyntax) {
  if (RecordSyntax.Equals(HTML_OID))
    cout << "</pre>" << endl;
}

SIMPLE::~SIMPLE() {
}
