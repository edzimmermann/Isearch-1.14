/**************************************************************************
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
File:		sgmlgils.cxx
Version:	1.00
Description:	Class SGMLGILS
Authors:   	Kevin Gamiel, Kevin.Gamiel@cnidr.org
Copyright:	CNIDR
@@@-*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "common.hxx"
#include "doctype.hxx"
#include "sgmlgils.hxx"

#define GILSRECORD_PATH "/home1/kgamiel/dev/GILS/records"

//#include "../../zdist-1.09/ztags.hxx"

SGMLGILS::SGMLGILS (PIDBOBJ DbParent): SGMLNORM::SGMLNORM (DbParent)
{
}

void SGMLGILS::
Present (const RESULT& ResultRecord, const STRING& ElementSet, 
	const STRING & RecordSyntax, PSTRING StringBuffer)
{

	if(RecordSyntax == SUTRS_OID)
		GetSUTRSRecord(ResultRecord, ElementSet, StringBuffer);
	else if(RecordSyntax == GRS1_OID)
		GetGRS1Record(ResultRecord, ElementSet, StringBuffer);
	else {
		*StringBuffer = "Unsupported record syntax of ";
		*StringBuffer += RecordSyntax;
		*StringBuffer += ".  Try SUTRS (1.2.840.10003.5.101)";
	}
}

void SGMLGILS::GetSUTRSRecord (const RESULT& ResultRecord, 
	const STRING& ElementSet, PSTRING StringBuffer)
{
	*StringBuffer = "";

	if(ElementSet == "B") {
		
	}
}

void SGMLGILS::GetSUTRSRecord (const RESULT& ResultRecord, 
	const STRING& ElementSet, PSTRING StringBuffer)
{
	if(ElementSet == "B") {
		/*
		GILS suggests these fields:
			Title
			Control Identifier (optional)
			Originator
			Local Control Number (optional)

		Line should be no longer than 79 characters.	
		*/

		//
		// Since we mandate the name of the fields in the sgml-tagged
		// file, we know which ones to look for.
		//
		STRING TempResult,title, ci, org, lcn;
		DOCTYPE::Present(ResultRecord, "Title", &title);
		DOCTYPE::Present(ResultRecord, "Control-Identifier", &ci);
		DOCTYPE::Present(ResultRecord, "Originator", &org);
		DOCTYPE::Present(ResultRecord, "Local-Control-Number", &lcn);
		STRINGINDEX title_len, ci_len, org_len, lcn_len;
		
		TempResult = title;
		title_len = title.GetLength();
		ci_len = ci.GetLength();
		org_len = org.GetLength();
		lcn_len = lcn.GetLength();
		if((title_len+ci_len+org_len+lcn_len+3) < 79) {
			TempResult = title;
			TempResult += "-";
			TempResult += ci;
			TempResult += "-";
			TempResult += org;
			TempResult += "-";
			TempResult += lcn;
		} else if((title_len+ci_len+org_len+2) < 79) {
			TempResult = title;
			TempResult += "-";
			TempResult += ci;
			TempResult += "-";
			TempResult += org;
		} else if((title_len+org_len+1) < 79) {
			TempResult = title;
			TempResult += "-";
			TempResult += org;
		}
		if(TempResult.GetLength() > 79) {
			TempResult.EraseAfter(76);
			TempResult += "...";
		}
		*StringBuffer = TempResult;
	} else if(ElementSet == "F") {
		// All fields.  Each field should be labelled with its
		// full field title followed by colon, then the field data.

		*StringBuffer = "Full Record";
	} else if(ElementSet == "G") {
		*StringBuffer = "G Record";
	} else if(ElementSet == "HTML HTML 0") {
		STRING Path, Fn;
		ResultRecord.GetPathName(&Path);
		ResultRecord.GetFileName(&Fn);
		Path += Fn;
		STRINGINDEX len = Path.GetLength();
		Path.EraseAfter(len-3);
		Path += "htm";
		StringBuffer->ReadFile(Path);
	} else {
		*StringBuffer = "Unsupported element set name of ";
		*StringBuffer += ElementSet;
		*StringBuffer += " requested";
	}
}

SGMLGILS::~SGMLGILS ()
{
}
