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
File:		iresult.cxx
Version:	1.00
Description:	Class IRESULT - Internal Search Result
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "iresult.hxx"

IRESULT::IRESULT() {
	MdtIndex = 0;
	HitCount = 0;
	Score = 0;
	HitTable = new FCT();
}

IRESULT& IRESULT::operator=(const IRESULT& OtherIresult) {
	MdtIndex = OtherIresult.MdtIndex;
	HitCount = OtherIresult.HitCount;
	Score = OtherIresult.Score;
	*HitTable = *(OtherIresult.HitTable);
	return *this;
}

void IRESULT::SetMdtIndex(const INT NewMdtIndex) {
	MdtIndex = NewMdtIndex;
}

INT IRESULT::GetMdtIndex() const {
	return MdtIndex;
}

void IRESULT::SetHitCount(const INT NewHitCount) {
	HitCount = NewHitCount;
}

void IRESULT::IncHitCount() {
	HitCount++;
}

void IRESULT::IncHitCount(const INT AddCount) {
	HitCount += AddCount;
}

INT IRESULT::GetHitCount() const {
	return HitCount;
}

void IRESULT::SetScore(const DOUBLE NewScore) {
	Score = NewScore;
}

void IRESULT::IncScore(const DOUBLE AddScore) {
	Score += AddScore;
}

DOUBLE IRESULT::GetScore() const {
	return Score;
}

void IRESULT::SetHitTable(const FCT& NewHitTable) {
	*HitTable = NewHitTable;
}

void IRESULT::GetHitTable(PFCT HitTableBuffer) const {
	*HitTableBuffer = *HitTable;
}

void IRESULT::AddToHitTable(const IRESULT& ResultRecord) {
	FCT Fct;
	ResultRecord.GetHitTable(&Fct);
	INT z = Fct.GetTotalEntries();
	INT x;
	FC Fc;
	for (x=1; x<=z; x++) {
		Fct.GetEntry(x, &Fc);
		HitTable->AddEntry(Fc);
	}
}

IRESULT::~IRESULT() {
	delete HitTable;
}
