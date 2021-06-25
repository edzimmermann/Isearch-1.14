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
File:		rset.cxx
Version:	1.00
Description:	Class RSET - Search Result Set
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdio.h>
#include "rset.hxx"
#include "common.hxx"

RSET::RSET() {
	Table = new RESULT[100];
	TotalEntries = 0;
	MaxEntries = 100;
	HighScore = 0;
	LowScore = 0;
}

void RSET::LoadTable(const STRING& FileName) {
	PFILE fp = fopen(FileName, "rb");
	if (!fp) {
		perror(FileName);
		exit(1);
	}
	else {
		LONG FSize = GetFileSize(fp);
		SIZE_T TSize = sizeof(RESULT);
		SIZE_T ArraySize = (FSize / TSize) + 1;
		Resize(ArraySize);
		TotalEntries = fread(Table, 1, FSize, fp) / TSize;
		fclose(fp);
		MaxEntries = ArraySize;
	}
}

void RSET::SaveTable(const STRING& FileName) {
	PFILE fp = fopen(FileName, "wb");
	if (!fp) {
		perror(FileName);
		exit(1);
	}
	else {
		fwrite(Table, 1, sizeof(RESULT)*TotalEntries, fp);
		fclose(fp);
	}
}

void RSET::AddEntry(const RESULT& ResultRecord) {
	DOUBLE S;
	S = ResultRecord.GetScore();
	if (S > HighScore) {
		HighScore = S;
	}
	if (S < LowScore) {
		LowScore = S;
	}
	if (TotalEntries == MaxEntries)
		Expand();
	Table[TotalEntries] = ResultRecord;
	TotalEntries = TotalEntries + 1;
}

void RSET::GetEntry(const INT Index, PRESULT ResultRecord) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*ResultRecord = Table[Index-1];
	}
}

static int RsetCompareKeys(const void* ResultPtr1, const void* ResultPtr2) {
	static STRING Key1;
	static STRING Key2;
	((RESULT*)ResultPtr1)->GetKey(&Key1);
	((RESULT*)ResultPtr2)->GetKey(&Key2);
	return ( Key1 == Key2 );
}

void RSET::SortByKey() {
	qsort(Table, TotalEntries, sizeof(RESULT), RsetCompareKeys);
}

static int RsetCompareScores(const void* ResultPtr1, const void* ResultPtr2) {
	return ( (int)( ((RESULT*)ResultPtr1)->GetScore()*100 - ((RESULT*)ResultPtr2)->GetScore()*100 ) );
}

void RSET::SortByScore() {
	qsort(Table, TotalEntries, sizeof(RESULT), RsetCompareScores);
}

INT RSET::GetScaledScore(const DOUBLE UnscaledScore, const INT ScaleFactor) {
	DOUBLE Diff = HighScore - LowScore;
	if (Diff == 0) {
		Diff = 1;
	}
	return ( (INT)( ((UnscaledScore - LowScore) * ScaleFactor) / Diff ) );
}

void RSET::Expand() {
	Resize(TotalEntries+100);
}

void RSET::CleanUp() {
	Resize(TotalEntries);
}

void RSET::Resize(const SIZE_T Entries) {
	PRESULT Temp = new RESULT[Entries];
	SIZE_T RecsToCopy;
	SIZE_T x;
	if (Entries >= TotalEntries) {
		RecsToCopy = TotalEntries;
	} else {
		RecsToCopy = Entries;
		TotalEntries = Entries;
	}
	for (x=0; x<RecsToCopy; x++) {
		// Not sure if Temp[x] = Table[x] is good enough.
		Temp[x] = Table[x];
	}
	if (Table)
		delete [] Table;
	Table = Temp;
	MaxEntries = Entries;
}

SIZE_T RSET::GetTotalEntries() {
	return TotalEntries;
}

//void RSET::Dump() const {
/*
	INT x, w;
	for (x=0; x<TotalEntries; x++) {
		w = Table[x].GetMdtIndex();
		cout<<w<<", ";
	}
	cout<<"\n";
*/
//}

RSET::~RSET() {
	if (Table)
		delete [] Table;
}
