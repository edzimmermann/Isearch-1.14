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
File:		result.cxx
Version:	1.00
Description:	Class RESULT - Search Result
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "result.hxx"

RESULT::RESULT() {
	HitTable = new FCT();
}

RESULT& RESULT::operator=(const RESULT& OtherResult) {
	Key = OtherResult.Key;
	DocumentType = OtherResult.DocumentType;
	PathName = OtherResult.PathName;
	FileName = OtherResult.FileName;
	RecordStart = OtherResult.RecordStart;
	RecordEnd = OtherResult.RecordEnd;
	Score = OtherResult.Score;
	*HitTable = *(OtherResult.HitTable);
	HitTable->SortByFc();
	return *this;
}

void RESULT::SetKey(const STRING& NewKey) {
	Key = NewKey;
}

void RESULT::GetKey(PSTRING StringBuffer) const {
	*StringBuffer = Key;
}

void RESULT::SetDocumentType(const STRING& NewDocumentType) {
	DocumentType = NewDocumentType;
}

void RESULT::GetDocumentType(PSTRING StringBuffer) const {
	*StringBuffer = DocumentType;
}

void RESULT::SetPathName(const STRING& NewPathName) {
	PathName = NewPathName;
}

void RESULT::GetPathName(PSTRING StringBuffer) const {
	*StringBuffer = PathName;
}

void RESULT::SetFileName(const STRING& NewFileName) {
	FileName = NewFileName;
}

void RESULT::GetFileName(PSTRING StringBuffer) const {
	*StringBuffer = FileName;
}
void RESULT::GetFullFileName(PSTRING StringBuffer) const {
	*StringBuffer = PathName;
	StringBuffer->Cat(FileName);
}

void RESULT::SetRecordStart(const GPTYPE NewRecordStart) {
	RecordStart = NewRecordStart;
}

GPTYPE RESULT::GetRecordStart() const {
	return RecordStart;
}

void RESULT::SetRecordEnd(const GPTYPE NewRecordEnd) {
	RecordEnd = NewRecordEnd;
}

GPTYPE RESULT::GetRecordEnd() const {
	return RecordEnd;
}

void RESULT::SetScore(const DOUBLE NewScore) {
	Score = NewScore;
}

DOUBLE RESULT::GetScore() const {
	return Score;
}

void RESULT::SetHitTable(const FCT& NewHitTable) {
	*HitTable = NewHitTable;
	HitTable->SortByFc();
}

void RESULT::GetHitTable(PFCT HitTableBuffer) const {
	*HitTableBuffer = *HitTable;
}

LONG RESULT::GetRecordSize() const {
	return (RecordEnd - RecordStart + 1);
}

void RESULT::GetRecordData(PSTRING StringBuffer) const {
	*StringBuffer = "";
	STRING fn;
	GetFullFileName(&fn);
	PFILE fp = fopen(fn, "rb");
	if (!fp) {
		perror(fn);
		exit(1);
	}
	else {
		INT rs = GetRecordStart();
		INT re = GetRecordEnd();
		INT size = re - rs + 1;
		fseek(fp, rs, 0);
		PCHR p = new CHR[size+1];
		p[fread(p, 1, size, fp)] = '\0';
		fclose(fp);
		*StringBuffer = p;
		delete [] p;
	}
}

void RESULT::GetHighlightedRecord(const STRING& BeforeTerm,
		const STRING& AfterTerm, PSTRING StringBuffer) const {
	GetRecordData(StringBuffer);
	INT z = HitTable->GetTotalEntries();
	INT x;
	FC Fc;
	for (x=z; x>=1; x--) {	// process terms backwards
		HitTable->GetEntry(x, &Fc);
		StringBuffer->Insert(Fc.GetFieldEnd() + 2, AfterTerm);
		StringBuffer->Insert(Fc.GetFieldStart() + 1, BeforeTerm);
	}
}

RESULT::~RESULT() {
	delete HitTable;
}
