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
File:		strlist.cxx
Version:	1.00
Description:	Class STRLIST - String List
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <string.h>
#include "strlist.hxx"

STRLIST::STRLIST() : VLIST() {
}

STRLIST& STRLIST::operator=(const STRLIST& OtherStrlist) {
	Clear();
	SIZE_T x;
	SIZE_T y = OtherStrlist.GetTotalEntries();
	STRLIST* NodePtr;
	STRLIST* NewNodePtr;
	for (x=1; x<=y; x++) {
		NodePtr = (STRLIST*)(OtherStrlist.GetNodePtr(x));
		NewNodePtr = new STRLIST();
		NewNodePtr->String = NodePtr->String;
		VLIST::AddNode(NewNodePtr);
	}
	return *this;
}

void STRLIST::AddEntry(const STRING& StringEntry) {
	STRLIST* NodePtr = new STRLIST();
	NodePtr->String = StringEntry;
	VLIST::AddNode(NodePtr);
}

void STRLIST::SetEntry(const SIZE_T Index, const STRING& StringEntry) {
	STRLIST* NodePtr = (STRLIST*)(VLIST::GetNodePtr(Index));
	if (NodePtr) {
		NodePtr->String = StringEntry;
	} else {
		if (Index > 0) {
			// Add filler nodes
			SIZE_T y = Index - GetTotalEntries();
			SIZE_T x;
			for (x=1; x<=y; x++) {
				NodePtr = new STRLIST();
				VLIST::AddNode(NodePtr);
			}
			NodePtr->String = StringEntry;
		}
	}
}

void STRLIST::GetEntry(const SIZE_T Index, STRING* StringEntry) const {
	STRLIST* NodePtr = (STRLIST*)(VLIST::GetNodePtr(Index));
	if (NodePtr) {
		*StringEntry = NodePtr->String;
	}
}

void STRLIST::Split(const CHR* Separator, const STRING& TheString) {
	STRINGINDEX Position;
	STRLIST NewList;
	STRING S, T;
	SIZE_T SLen = strlen(Separator);
	S = TheString;
	// parse S and build list of terms
	while ( (Position=S.Search(Separator)) != 0) {
		T = S;
		T.EraseAfter(Position - 1);
		S.EraseBefore(Position + SLen);
		NewList.AddEntry(T);
	}
	NewList.AddEntry(S);	// add the remaining entry
	*this = NewList;
}

void STRLIST::Join(const CHR* Separator, STRING* StringBuffer) {
	STRING NewString;
	SIZE_T x;
	SIZE_T y = GetTotalEntries();
	for (x=1; x<=y; x++) {
		NewString += ((STRLIST*)(VLIST::GetNodePtr(x)))->String;
		if (x < y) {
			NewString += Separator;
		}
	}
	*StringBuffer = NewString;
}

SIZE_T STRLIST::SearchCase(const STRING& SearchTerm) {
	SIZE_T x = 1;
	STRLIST* p = (STRLIST*)(this->GetNextNodePtr());
	while (p != this) {
		if (p->String ^= SearchTerm) {
			return x;
		}
		x++;
		p = (STRLIST*)(p->GetNextNodePtr());
	}
	return 0;
}

void STRLIST::GetValue(const CHR* Title, STRING* StringBuffer) {
	*StringBuffer = "";
	SIZE_T x;
	STRINGINDEX Position, y;
	STRING S;
	STRING S2;
	SIZE_T TotalEntries = GetTotalEntries();
	for (x=1; x<=TotalEntries; x++) {
		S2 = ((STRLIST*)(VLIST::GetNodePtr(x)))->String;
		if ( (Position=S2.Search('=')) ) {
			S = S2;
			S.EraseAfter(Position - 1);
			while (S.GetChr(1) == ' ') {	// get rid of leading spaces
				S.EraseBefore(2);
			}
			while (S.GetChr((y=S.GetLength())) == ' ') {	// get rid of trailing spaces
				S.EraseAfter(y-1);
			}
			if (S ^= Title) {
				S = S2;
				S.EraseBefore(Position + 1);
				*StringBuffer = S;
				return;
			}
		}
	}
}

/*
ostream& operator<<(ostream& os, const STRLIST& str) {
	INT x;
	for (x=0; x<str.TotalEntries; x++) {
		os << str.Table[x] << endl;
	}
	return os;
}
*/
