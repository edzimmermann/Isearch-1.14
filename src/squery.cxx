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
File:		squery.cxx
Version:	1.00
Description:	Class SQUERY - Search Query
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "strlist.hxx"
#include "sterm.hxx"
#include "attrlist.hxx"
#include "squery.hxx"
#include "operator.hxx"
#include "tokengen.hxx"

SQUERY::SQUERY() {
}

SQUERY& SQUERY::operator=(const SQUERY& OtherSquery) {
	Opstack = OtherSquery.Opstack;
//	Term = OtherSquery.Term;
	return *this;
}

void SQUERY::SetOpstack(const OPSTACK& NewOpstack) {
	Opstack = NewOpstack;
}

void SQUERY::GetOpstack(POPSTACK OpstackBuffer) const {
	*OpstackBuffer = Opstack;
}

void SQUERY::SetTerm(const STRING& NewTerm) {
	TOKENGEN *TermList;
	TermList = new TOKENGEN(NewTerm);
	TermList->SetQuoteStripping(GDT_TRUE);
	INT x, y;
	y = TermList->GetTotalEntries();
	STRLIST ListTemp;
	STRING StrTemp;
	STERM Sterm;
	OPSTACK Stack;
	OPERATOR Operator;
	Operator.SetOperatorType(OperatorOr);
	PATTRLIST AttrlistPtr;
	for (x=1; x<=y; x++) {
		AttrlistPtr = new ATTRLIST();
		TermList->GetEntry(x, &StrTemp);
		ListTemp.Split("/", StrTemp);
		if (ListTemp.GetTotalEntries() >= 2) {
			ListTemp.GetEntry(1, &StrTemp);	// get field name
			AttrlistPtr->AttrSetFieldName(StrTemp);
			ListTemp.GetEntry(2, &StrTemp);	// get remaining string
		}
		ListTemp.Split(":", StrTemp);
		if (ListTemp.GetTotalEntries() >= 2) {
			ListTemp.GetEntry(2, &StrTemp);	// get term weight
			AttrlistPtr->AttrSetTermWeight(StrTemp);
			ListTemp.GetEntry(1, &StrTemp);	// get remaining string
		}
		if (StrTemp.GetChr(StrTemp.GetLength()) == '*') {
 			AttrlistPtr->AttrSetRightTruncation(GDT_TRUE);
			StrTemp.EraseAfter(StrTemp.GetLength()-1);
		}
       		Sterm.SetTerm(StrTemp);
		Sterm.SetAttributes(*AttrlistPtr);
		delete AttrlistPtr;
		Stack << Sterm;
		if (x > 1) {
			// if this is not the first term, push an OR
			Stack << Operator;
		}
	}
	SetOpstack(Stack);
	delete TermList;
}

void SQUERY::SetRpnTerm(const STRING& NewTerm) {
	TOKENGEN *TermList;
	TermList = new TOKENGEN(NewTerm);
	TermList->SetQuoteStripping(GDT_TRUE);
	INT x, y;
	y = TermList->GetTotalEntries();
	STRLIST ListTemp;
	STRING StrTemp;
	STERM Sterm;
	OPSTACK Stack;
	OPERATOR Operator;
	PATTRLIST AttrlistPtr;
	for (x=1; x<=y; x++) {
		TermList->GetEntry(x, &StrTemp);
		//quick hack fix -jem.
		if ( (StrTemp == "") || (StrTemp == " ") )
			continue;
		if ( (StrTemp ^= "OR") || (StrTemp ^= "AND") 
		    || (StrTemp ^= "ANDNOT") || (StrTemp ^= "NEAR") ) {
			if (StrTemp ^= "OR") {
				Operator.SetOperatorType(OperatorOr);
			}
			if (StrTemp ^= "AND") {
				Operator.SetOperatorType(OperatorAnd);
			}
			if (StrTemp ^= "ANDNOT") {
				Operator.SetOperatorType(OperatorAndNot);
			}
			if (StrTemp ^= "NEAR") {
			        Operator.SetOperatorType(OperatorNear);
                        }	
			Stack << Operator;
		} else {
			AttrlistPtr = new ATTRLIST();
			ListTemp.Split("/", StrTemp);
			if (ListTemp.GetTotalEntries() >= 2) {
				ListTemp.GetEntry(1, &StrTemp);	// get field name
				AttrlistPtr->AttrSetFieldName(StrTemp);
				ListTemp.GetEntry(2, &StrTemp);	// get remaining string
			}
			ListTemp.Split(":", StrTemp);
			if (ListTemp.GetTotalEntries() >= 2) {
				ListTemp.GetEntry(2, &StrTemp);	// get term weight
				AttrlistPtr->AttrSetTermWeight(StrTemp);
				ListTemp.GetEntry(1, &StrTemp);	// get remaining string
			}
			if (StrTemp.GetChr(StrTemp.GetLength()) == '*') {
	 			AttrlistPtr->AttrSetRightTruncation(GDT_TRUE);
				StrTemp.EraseAfter(StrTemp.GetLength()-1);
			}
	       		Sterm.SetTerm(StrTemp);
			Sterm.SetAttributes(*AttrlistPtr);
			delete AttrlistPtr;
			Stack << Sterm;
		}
	}
	SetOpstack(Stack);
	delete TermList;
}

void SQUERY::GetTerm(PSTRING StringBuffer) const {
	*StringBuffer = "";
	OPSTACK Stack;
	GetOpstack(&Stack);
	POPOBJ OpPtr;
	ATTRLIST Attrlist;
	STRING S;
	INT Count = 0;
	while (Stack >> OpPtr) {
		if (OpPtr->GetOpType() == TypeOperand) {
			if (Count > 0) {
				StringBuffer->Cat(" ");
			}
			Count++;
			OpPtr->GetAttributes(&Attrlist);
			if (Attrlist.AttrGetFieldName(&S)) {
				S += "/";
				*StringBuffer += S;
			}
			OpPtr->GetTerm(&S);
			*StringBuffer += S;
			if (Attrlist.AttrGetRightTruncation()) {
				*StringBuffer += "*";
			}
			if (Attrlist.AttrGetTermWeight(&S)) {
				*StringBuffer += ":";
				*StringBuffer += S;
			}
		}
		delete OpPtr;
	}
}

SQUERY::~SQUERY() {
}
