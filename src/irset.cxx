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
File:		irset.cxx
Version:	1.00
Description:	Class IRSET - Internal Search Result Set
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include "irset.hxx"
#include "common.hxx"

int IrsetIndexCompare(const void* x, const void* y) {
  INT4 Difference = ( (*((PIRESULT)y)).GetMdtIndex() - (*((PIRESULT)x)).GetMdtIndex() );
  if (Difference < 0) {
    return (-1);
  } else {
    if (Difference == 0) {
      return(0);
    } else {
      return 1;
    }
  }
}

IRSET::IRSET(const PIDBOBJ DbParent) {
	Init(DbParent);
}

void IRSET::Init(const PIDBOBJ DbParent) {
	Table = new IRESULT[100];
	TotalEntries = 0;
	MaxEntries = 100;
	Parent = DbParent;
}

OPOBJ& IRSET::operator=(const OPOBJ& OtherIrset) {
	if (Table) {
		delete [] Table;
	}
	Init(OtherIrset.GetParent());
	INT y = OtherIrset.GetTotalEntries();
	INT x;
	IRESULT iresult;
	for (x=1; x<=y; x++) {
		OtherIrset.GetEntry(x, &iresult);
		AddEntry(iresult, 0);
	}
	return *this;
}

OPOBJ* IRSET::Duplicate() const {
	IRSET* Temp = new IRSET(Parent);
	*Temp = *((OPOBJ*)this);
	return (OPOBJ*)Temp;
}

/*
void IRSET::LoadTable(const STRING& FileName) {
	PFILE fp = fopen(FileName, "r");
	if (fp) {
		INT FSize = GetFileSize(fp);
		INT TSize = sizeof(IRESULT);
		INT ArraySize = (FSize / TSize) + 1;
		Resize(ArraySize);
		TotalEntries = fread(Table, 1, FSize, fp) / TSize;
		fclose(fp);
		MaxEntries = ArraySize;
	}
}
*/

/*
void IRSET::SaveTable(const STRING& FileName) {
	PFILE fp = fopen(FileName, "w");
	if (fp) {
		fwrite(Table, 1, sizeof(IRESULT)*TotalEntries, fp);
		fclose(fp);
	}
}
*/

void IRSET::AddEntry(const IRESULT& ResultRecord, const INT AddHitCounts) {
	INT x;
	// linear!
	for (x=0; x<TotalEntries; x++) {
		if (Table[x].GetMdtIndex() == ResultRecord.GetMdtIndex()) {
			if (AddHitCounts) {
				Table[x].IncHitCount(
						ResultRecord.GetHitCount());
				Table[x].AddToHitTable(ResultRecord);
			}
			Table[x].IncScore(ResultRecord.GetScore());
			return;
		}
	}

	if (TotalEntries == MaxEntries)
		Expand();
	Table[TotalEntries] = ResultRecord;
	TotalEntries = TotalEntries + 1;
}

void IRSET::GetEntry(const INT Index, PIRESULT ResultRecord) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*ResultRecord = Table[Index-1];
	}
}

PRSET IRSET::GetRset() {
	PRSET prset = new RSET();
	RESULT result;
	MDTREC mdtrec;
	FCT Fct;
	STRING s;
	INT x;
	for (x=0; x<TotalEntries; x++) {
		Parent->GetMainMdt()->GetEntry(Table[x].GetMdtIndex(), &mdtrec);
		if (mdtrec.GetDeleted() == GDT_FALSE) {
			mdtrec.GetKey(&s);
			result.SetKey(s);
			mdtrec.GetDocumentType(&s);
			result.SetDocumentType(s);
			mdtrec.GetPathName(&s);
			result.SetPathName(s);
			mdtrec.GetFileName(&s);
			result.SetFileName(s);
			result.SetRecordStart(mdtrec.GetLocalRecordStart());
			result.SetRecordEnd(mdtrec.GetLocalRecordEnd());
			result.SetScore(Table[x].GetScore());
			Table[x].GetHitTable(&Fct);
			Fct.SubtractOffset(mdtrec.GetGlobalFileStart() + mdtrec.GetLocalRecordStart());
			result.SetHitTable(Fct);
			prset->AddEntry(result);
		}
	}
	return prset;
}

void IRSET::Expand() {
	Resize(TotalEntries+100);
}

void IRSET::CleanUp() {
	Resize(TotalEntries);
}

void IRSET::Resize(const INT Entries) {
	PIRESULT Temp = new IRESULT[Entries];
	INT RecsToCopy;
	INT x;
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

INT IRSET::GetTotalEntries() const {
	return TotalEntries;
}

INT IRSET::GetHitTotal() const {
	INT x;
	INT Total = 0;
	for (x=0; x<TotalEntries; x++) {
		Total += Table[x].GetHitCount();
	}
	return Total;
}

void IRSET::Or(const OPOBJ& OtherIrset) {
	INT x;
	INT t = OtherIrset.GetTotalEntries();
	IRESULT OtherIresult;
	for (x=1; x<= t; x++) {
		OtherIrset.GetEntry(x, &OtherIresult);
		AddEntry(OtherIresult, 0);
	}
}

// AndNOT added by Glenn MacStravic
void IRSET::AndNot(const OPOBJ& OtherIrset) {
	IRESULT OtherIresult;
	IRSET MyResult(Parent);
	INT x = 1;
	INT count=0;
	INT t = OtherIrset.GetTotalEntries();
	IRESULT* match;
	SortByIndex();

	while (x <= t) {
	  OtherIrset.GetEntry(x, &OtherIresult);
	  match = (IRESULT*)bsearch(&OtherIresult, Table,
				    TotalEntries, sizeof(IRESULT), IrsetIndexCompare);
	  if (match == NULL) {
	    MyResult.AddEntry(OtherIresult, 0);
	    count++;
	  }
	  x++;
	}

	delete [] Table;
	TotalEntries=count;
	MaxEntries=MyResult.MaxEntries;
	Table = MyResult.StealTable();
}
// CharProx added by Kevin Gamiel
//
// Example:  'dog' within 5 characters of 'cat'
//
// For(all entries in dog result-list)
//   For(all entries in cat result-list)
//     if(in same record)
//       for(each hit in dog hit table)
//         for(each hit in cat hit table)
//           if(hits within 5 characters of each other)
//             add current dog item to final result-list
//
void IRSET::CharProx(const OPOBJ& OtherIrset, const INT Distance) {
  IRESULT OtherIresult, MyIresult;
  FCT MyHitTable, OtherHitTable;
  IRSET MyResult(Parent);
  INT i,j,k,p;
  INT OtherSetTotalEntries = OtherIrset.GetTotalEntries();
  IRESULT* match;

  for(i=1;i <= TotalEntries;i++) {
    GetEntry(i, &MyIresult);
    for(j=1;j <= OtherSetTotalEntries;j++) {
      OtherIrset.GetEntry(j, &OtherIresult);
      if(MyIresult.GetMdtIndex() == 
	 OtherIresult.GetMdtIndex()) {
	MyIresult.GetHitTable(&MyHitTable);
	OtherIresult.GetHitTable(&OtherHitTable);
	INT MyNumHits, OtherNumHits;
	MyNumHits = MyHitTable.GetTotalEntries();       
	OtherNumHits = OtherHitTable.GetTotalEntries(); 
	FC MyFc, OtherFc;
	GDT_BOOLEAN IsMatch;
	for(k=1;k <= MyNumHits;k++) {
	  MyHitTable.GetEntry(k, &MyFc);
	  for(p=1;p<=OtherNumHits;p++) {
	    IsMatch = GDT_FALSE;
	    OtherHitTable.GetEntry(p, &OtherFc);
	    if(MyFc.GetFieldStart() < OtherFc.GetFieldStart())
	      {
		if((OtherFc.GetFieldStart() - MyFc.GetFieldEnd() - 1) 
		   <= Distance)
		  IsMatch = GDT_TRUE;
	      } else {
		if((MyFc.GetFieldStart() - OtherFc.GetFieldEnd() - 1)
		   <= Distance)
		  IsMatch = GDT_TRUE;
	      }
	    if(IsMatch)
	      MyResult.AddEntry(OtherIresult, 0);
	  }
	}
      }
    }
  }
  
  delete [] Table;
  TotalEntries=MyResult.GetTotalEntries();
  MaxEntries=MyResult.MaxEntries;
  Table = MyResult.StealTable();
}

#ifndef MULTI
// Faster AND implementation added by Glenn MacStravic
void IRSET::And(const OPOBJ& OtherIrset) {
	IRESULT OtherIresult;
	IRSET MyResult(Parent);
	INT x = 1;
	INT count=0;
	INT t = OtherIrset.GetTotalEntries();
	IRESULT* match;

	SortByIndex();
	while (x <= t) {
	  OtherIrset.GetEntry(x, &OtherIresult);
	  match = (IRESULT*)bsearch(&OtherIresult, Table,
				    TotalEntries, sizeof(IRESULT), IrsetIndexCompare);

	  if (match != NULL) {
    	    match->AddToHitTable(OtherIresult);
	    match->IncHitCount(OtherIresult.GetHitCount());
	    match->IncScore(OtherIresult.GetScore());
	    MyResult.AddEntry(*match, 0);
	    count++;
	  }
	  x++;
	}
	
	
	delete [] Table;
	TotalEntries=count;
	MaxEntries=MyResult.MaxEntries;
	Table = MyResult.StealTable();

      }
#else

void IRSET::And(const OPOBJ& OtherIrset) {
  // not a very fast implementation
  INT y, found;
  IRESULT OtherIresult;
  INT x = 0;
  PIRSET pTempIrset;

  PRSET Prset,OtherPrset;
  RESULT MyResultRecord, OtherResultRecord;
  STRING MyPath,OtherPath;

  pTempIrset = (PIRSET) &OtherIrset;
  OtherPrset = pTempIrset->GetRset();
  Prset = GetRset();

  while (x < TotalEntries) {
    found = 0;
    Prset->GetEntry(x+1, &MyResultRecord);
    MyResultRecord.GetPathName(&MyPath);

    for (y=1; y<=OtherIrset.GetTotalEntries(); y++) {
      OtherPrset->GetEntry(y, &OtherResultRecord);
      OtherResultRecord.GetPathName(&OtherPath);
      OtherIrset.GetEntry(y, &OtherIresult);

      if (MyPath == OtherPath) {
	found = 1;
//	AddEntry(OtherIresult, 0);
      }
    }
    if (!found) {
	Table[x].SetMdtIndex(0);
    } 
    x++;

  }

  INT in, out, last;
  out = 0;
  last = TotalEntries;
  for (in=0;in<last;in++) {
    if (Table[in].GetMdtIndex() != 0) {
      Table[out] = Table[in];
      out++;
    } else {
      TotalEntries--;
    }
  }
}

#endif

IRESULT* IRSET::StealTable() {
  IRESULT* TempTablePtr = Table;
  Table = new IRESULT[2];
  TotalEntries = 0;
  MaxEntries = 2;
  
  return TempTablePtr;
}

void IRSET::ComputeScores(const INT TermWeight) {
  if (TotalEntries == 0) {
    return;
  }
  INT x;
  DOUBLE DocsInRs = TotalEntries;
  DOUBLE DocsInDb = Parent->GetMainMdt()->GetTotalEntries();
  DOUBLE InvDocFreq = DocsInDb / DocsInRs;
  DOUBLE SumSqScores = 0;
  DOUBLE SqrtSum;
  DOUBLE Score;
  for (x=0; x<TotalEntries; x++) {
    Score = Table[x].GetHitCount() * InvDocFreq;
    Table[x].SetScore(Score);
    SumSqScores += (Score * Score);
  }
  SqrtSum = sqrt(SumSqScores);
  if (SqrtSum == 0.0) {
    SqrtSum = 1.0;
  }
  for (x=0; x<TotalEntries; x++) {
    Table[x].SetScore(Table[x].GetScore() / SqrtSum * TermWeight);
  }
}

int IrsetScoreCompare(const void* x, const void* y) {
  DOUBLE Difference = ( (*((PIRESULT)y)).GetScore() - (*((PIRESULT)x)).GetScore() );
  if (Difference < 0) {
    return (-1);
  } else {
    if (Difference == 0) {
      return ( (*((PIRESULT)x)).GetMdtIndex() - (*((PIRESULT)y)).GetMdtIndex() );
    } else {
      return 1;
    }
  }
  /*
     return ( (INT)( ((*((PIRESULT)y)).GetScore())*1000 -
     ((*((PIRESULT)x)).GetScore())*1000 ) );
     */
}

void IRSET::SortByScore() {
  qsort(Table, TotalEntries, sizeof(IRESULT), IrsetScoreCompare);
}

void IRSET::SortByIndex() {
  qsort(Table, TotalEntries, sizeof(IRESULT), IrsetIndexCompare);
}

void IRSET::SetParent(PIDBOBJ const NewParent) {
  Parent = NewParent;
}

PIDBOBJ IRSET::GetParent() const {
  return Parent;
}

IRSET::~IRSET() {
  if (Table)
    delete [] Table;
}
