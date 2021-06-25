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
File:		index.cxx
Version:	1.00
Description:	Class INDEX
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "index.hxx"
#include "common.hxx"
#include "irset.hxx"
#include "sw.hxx"
#include "soundex.hxx"
#include "mdt.hxx"
#include "mdtrec.hxx"
#ifdef DICTIONARY
#include "dictionary.hxx"
#include "../doctype/doctype.hxx"
#endif

// const INT StopWordSize = 400;
const INT StopWordSize = (sizeof(stoplist)/sizeof(stoplist[0]));

static PCHR MemoryData;
static INT MemoryDataLength;

INDEX::INDEX(const PIDBOBJ DbParent, const STRING& NewFileName) {
  Parent = DbParent;
  IndexFileName = NewFileName;
#ifdef DICTIONARY
  Dict = new DICTIONARY(DbParent);
#endif
}

int MemIndexCompare(const void* x, const void* y) {
  return strncmp(MemoryData + (*((PGPTYPE)x)),
		     MemoryData + (*((PGPTYPE)y)), StringCompLength);
}
/*
int MemIndexCompare(const void* x, const void* y) {
  INT result;
  INT MaxCompare;
  INT t = MemoryDataLength - ((*((PGPTYPE)y)));
  MaxCompare = MemoryDataLength - ((*((PGPTYPE)x)));
  if (t < MaxCompare) {
    MaxCompare = t;
  }
  if (StringCompLength < MaxCompare) {
    MaxCompare = StringCompLength;
  }
  result = strncasecmp(MemoryData + (*((PGPTYPE)x)),
		       MemoryData + (*((PGPTYPE)y)), MaxCompare);
  return result;
}
*/

#ifdef DICTIONARY
void INDEX::CreateDictionary(void) {
	Dict->CreateNew();
}

void INDEX::CreateCentroid(void) {
	FILE *out;
	STRING CentroidName;
	Parent->ComposeDbFn(&CentroidName, DbExtCentroid);
	out = Parent->ffopen(CentroidName, "w");
	if (!out) {
		cout << "Can't open " << CentroidName << ':';
		cout << strerror(errno) << endl;
	}
	if (Dict->GetSearchable())
		Dict->Print(out);
	else {
		cout << "You must generate a dictionary with the -dict option,";
		cout << "before you can create a centroid." << endl;
	}
}
#endif

void INDEX::WriteFieldData(const RECORD& Record, const GPTYPE GpOffset) {
  DFT dft;
  Record.GetDft(&dft);
  INT total = dft.GetTotalEntries();
  SIZE_T ytotal;
  INT x, y;
  DF df;
  FCT fct;
  FC fc;
  PFILE fp;
  STRING FieldName, FileName;
  GPTYPE gp;
  for (x=1; x<=total; x++) {
    dft.GetEntry(x, &df);
    df.GetFieldName(&FieldName);
    Parent->DfdtGetFileName(FieldName, &FileName);
    fp = Parent->ffopen(FileName, "ab");
    if (!fp) {
      perror(FileName);
      exit(1);
    }
    df.GetFct(&fct);
    ytotal = fct.GetTotalEntries();
    for (y=1; y<=ytotal; y++) {
      fct.GetEntry(y, &fc);
      gp = fc.GetFieldStart() + GpOffset;
      Parent->GpFwrite(&gp, 1, sizeof(GPTYPE), fp);
      gp = fc.GetFieldEnd() + GpOffset;
      Parent->GpFwrite(&gp, 1, sizeof(GPTYPE), fp);
    }
    Parent->ffclose(fp);
  }
}

void INDEX::AddRecordList(PFILE RecordListFp) 
{
  UINT4 DataMemorySize = (UINT4)(Parent->GetIndexingMemory() );

//  UINT4 IndexMemorySize = (UINT4)((DataMemorySize / 3) * sizeof(GPTYPE));
//  PGPTYPE MemoryIndex = new GPTYPE[(IndexMemorySize / sizeof(GPTYPE)) + 1];
  UINT4 IndexMemorySize = (UINT4)(DataMemorySize / 3); // jw patch
  PGPTYPE MemoryIndex = new GPTYPE[IndexMemorySize+1]; // jw patch

  MemoryData = new CHR[DataMemorySize];
  INT FirstRecord = 1;
  INT CurrentRecord;
  INT MemoryIndexLength;
  PFILE DataFp = 0;
  RECORD record;
  STRING s, DataFileName, OldDataFileName;
  MDTREC mdtrec;
  UINT4 DataFileSize;
  INT GpListSize;
  INT Error;
  GPTYPE TrueGlobalStart = 0;
  GPTYPE TrueGlobalEnd = 0;
  GPTYPE OldGlobalStart;
  MDTREC lastmdtrec;
  INT j;
  PCHR p;
  CHR TempBuffer[80];
  STRING RecordFlag;
  STRING Doctype;
  GDT_BOOLEAN Break;
  Break = GDT_FALSE;
  do {
    CurrentRecord = FirstRecord;
    Error = 0;
    MemoryDataLength = 0;
    MemoryIndexLength = 0;
    OldGlobalStart = Parent->GetMainMdt()->GetNextGlobal();

    do {
      if (!Break) {
	RecordFlag.FGet(RecordListFp, 3);
      }
      if ( (RecordFlag == "#") || (Break) ) {
	
	if (!Break) {
	  record.Read(RecordListFp);
	}
	Break = GDT_FALSE;
	record.GetFullFileName(&DataFileName);
	if (!DataFileName.Equals(OldDataFileName)) {
	  if (DataFp) {
	    fclose(DataFp);
	  }
	  DataFp = fopen(DataFileName, "rb");
	}
	if (!DataFp) {
	  // ER
	  cout << "   Skipping file ";
	  DataFileName.Print();
	  cout << " ... (Error opening file)" << endl;
	  CurrentRecord++;
	} else {
	  if (record.GetRecordEnd() == 0) {
	    fseek(DataFp, 0, 2);
	    DataFileSize = ftell(DataFp);
	  } else {
	    DataFileSize = record.GetRecordEnd() -
	      record.GetRecordStart() + 1;
	  }
	  
	  if (!DataFileName.Equals(OldDataFileName)) {
            if (MemoryDataLength)                       /* <<<------------- */
              MemoryDataLength++;                       /* <<<------------- */
	    TrueGlobalStart = Parent->GetMainMdt()->GetNextGlobal();
	    fseek(DataFp, 0, 2);
	    TrueGlobalEnd = TrueGlobalStart + ftell(DataFp) - 1;

	  }
	
	  if ( DataFileSize >= DataMemorySize ) {
	    cout << "One of the document records you are indexing ";
	    cout << "is too large for the amount" << endl;
	    cout << "of memory allocated by Iindex.  Use the `-m' ";
	    cout << "option to set a value" << endl;
	    cout << "greater than the largest document record you ";
	    cout << "are indexing.  For example," << endl;
	    cout << "use `-m 2' if the largest document is 1.5 MB." << endl;
	    exit(1);
	  }

	  if ( (DataFileSize + MemoryDataLength) >= DataMemorySize ) {
	    Break = GDT_TRUE;
	    break;
	  }
	  fseek(DataFp, record.GetRecordStart(), 0);	// core dump
	  fread(MemoryData + MemoryDataLength, 1, DataFileSize,
		DataFp);

          for (p = MemoryData + MemoryDataLength;
            p < (MemoryData + MemoryDataLength + DataFileSize); p++) {
	    *p = tolower(*p);
            if (!isalnum(*p)) {
              *p = ' ';
            }
          }
          *p = '\0';              // Add a NULL to terminate the record
          record.GetDocumentType(&Doctype);
	  GpListSize = BuildGpList(Doctype, 
				   MemoryDataLength,
				   MemoryData,
				   MemoryDataLength + DataFileSize,
				   MemoryIndex + MemoryIndexLength,
				   IndexMemorySize - MemoryIndexLength);
	  if (GpListSize == -1) {
	    // ??
	    Break = GDT_TRUE;
	    break;
	  }

	  if (!DataFileName.Equals(OldDataFileName)) {
	    Parent->IndexingStatus(IndexingStatusParsingDocument,
				   &DataFileName, 0);
	  }
	  record.GetDocumentType(&s);
	  mdtrec.SetDocumentType(s);
	  record.GetPathName(&s);
	  mdtrec.SetPathName(s);
	  record.GetFileName(&s);
	  mdtrec.SetFileName(s);
	  mdtrec.SetLocalRecordStart(record.GetRecordStart());
	  if ( (record.GetRecordStart() == 0) &&
	      (record.GetRecordEnd() == 0) ) {
	    mdtrec.SetLocalRecordEnd(DataFileSize - 1);
	  } else {
	    mdtrec.SetLocalRecordEnd(record.GetRecordEnd());
	  }
	  
	  mdtrec.SetGlobalFileStart(TrueGlobalStart);
	  mdtrec.SetGlobalFileEnd(TrueGlobalEnd);
	  
	  record.GetKey(&s);
	  // Something that needs to be added somewhere:
	  // If record already contains a user-defined key,
	  // we need to make sure that it is unique!
	  if (s == "") {
	    sprintf(TempBuffer, "%d", 
		    mdtrec.GetGlobalFileStart() 
		    + mdtrec.GetLocalRecordStart());
	    s = TempBuffer;
	    Parent->GetMainMdt()->GetUniqueKey(&s);
	  }
	  mdtrec.SetKey(s);
	  
	  MemoryDataLength += DataFileSize;
//	  MemoryDataLength += DataFileSize+1;

	  MemoryIndexLength += GpListSize;
#ifdef DEBUG
	  STRING XXX;
	  mdtrec.GetKey(&XXX);
	  cout << "MDTrec key=" << XXX;
	  cout << endl;
	  cout << "MDTrec LocalRecordStart=" << mdtrec.GetLocalRecordStart();
	  cout << endl;
	  cout << "MDTrec LocalRecordEnd=" << mdtrec.GetLocalRecordEnd() ;
	  cout << endl;
	  cout << "MDTrec GlobalFileStart=" << mdtrec.GetGlobalFileStart();
	  cout << endl;
	  cout << "MDTrec GlobalFileEnd=" << mdtrec.GetGlobalFileEnd() ;
	  cout << endl;
#endif /* DEBUG */
	  Parent->GetMainMdt()->AddEntry(mdtrec);
	  OldDataFileName = DataFileName;
	  CurrentRecord++;

#ifdef VERBOSE
	  cout << "   ...Parsing fields" << endl;
#endif

	Parent->ParseFields(&record);

#ifdef VERBOSE
	  cout << "   ...Writing field data" << endl;
#endif

	  WriteFieldData(record, mdtrec.GetGlobalFileStart() +
			 mdtrec.GetLocalRecordStart());
	}
	if (Break) {
	  break;
	}
      }
    } while (RecordFlag == "#");
    if (Error == 0) {
      Parent->IndexingStatus(IndexingStatusIndexing, 0,
			     MemoryIndexLength);
      qsort(MemoryIndex, MemoryIndexLength, sizeof(GPTYPE),
	    MemIndexCompare);
      Parent->IndexingStatus(IndexingStatusMerging, 0, 0);
      MergeIndex(MemoryData, MemoryDataLength, MemoryIndex,
		 MemoryIndexLength, OldGlobalStart);
    }
    FirstRecord = CurrentRecord;
  } while (RecordFlag == "#");
  fclose(DataFp);

//  DumpIndex(0);

  delete [] MemoryData;
  delete [] MemoryIndex;
}

void INDEX::MergeIndex(PCHR MemoryData, INT MemoryDataLength,
		PGPTYPE NewMemoryIndex, INT MemoryIndexLength,
		GPTYPE GlobalStart) {
	// Open index file
  PFILE fp = Parent->ffopen(IndexFileName, "rb");
  if (fp) {
    // Merge index
    STRING NewIndexFN;
    Parent->ComposeDbFn(&NewIndexFN, DbExtTemp);
/*
printf("[");
NewIndexFN.Print();
printf("]\n");
fflush(stdout);
*/
    PFILE fpNew = Parent->ffopen(NewIndexFN, "wb");
    INT j;
    INT x;
    INT w;
    GPTYPE DiskGP;
    GPTYPE y;
    PFILE FileFP;
    CHR DiskData[StringCompLength + 1];

    x = 0;

    while (x < MemoryIndexLength &&
	Parent->GpFread(&DiskGP, 1, sizeof(GPTYPE), fp) ) {

	FileFP = GetFilePointer(DiskGP);
	w = fread(DiskData, 1, StringCompLength, FileFP);
	Parent->ffclose(FileFP);

	for (j = 0; j < w; j++) {
	  if ( !isalnum(DiskData[j]) ) {
	    DiskData[j] = ' ';
	  }
	}
	DiskData[j] = '\0';

	while (x < MemoryIndexLength &&
	  strncasecmp(MemoryData + NewMemoryIndex[x],
	    DiskData, StringCompLength) < 0) {
	  y = GlobalStart + NewMemoryIndex[x];
	  Parent->GpFwrite(&y, 1, sizeof(GPTYPE), fpNew);
	  x++;
	}

	Parent->GpFwrite(&DiskGP, 1, sizeof(GPTYPE), fpNew);
    }
    
    while (x < MemoryIndexLength) {
      y = GlobalStart + NewMemoryIndex[x];
      Parent->GpFwrite(&y, 1, sizeof(GPTYPE), fpNew);
      x++;
    }

    while ( Parent->GpFread(&DiskGP, 1, sizeof(GPTYPE), fp) ) {
      Parent->GpFwrite(&DiskGP, 1, sizeof(GPTYPE), fpNew);
    }

    Parent->ffclose(fpNew);
    Parent->ffclose(fp);
    PCHR TempFN, Temp2FN;
    TempFN = IndexFileName.NewCString();
    Temp2FN = NewIndexFN.NewCString();
 
#if defined(_MSDOS) || defined(_WIN32)
 
    /*
     * MSDOS / WIN32 rename doesn't remove an existing file so
     * we have to do it ourselves.
     */

    remove(TempFN);
#endif

    rename(Temp2FN, TempFN);
    delete [] TempFN;
    delete [] Temp2FN;
  } else {
    fp = Parent->ffopen(IndexFileName, "wb");
    if (!fp) {
      perror(IndexFileName);
      exit(1);
    }
    // Dump out index
    Parent->GpFwrite(NewMemoryIndex, 1, MemoryIndexLength*sizeof(GPTYPE), fp);
    Parent->ffclose(fp);
  }
}

PFILE INDEX::GetFilePointer(const GPTYPE gp) const {
  INT x = Parent->GetMainMdt()->LookupByGp(gp);
  if (x) {
    STRING FileName;
    PFILE fp;
    MDTREC Mdtrec;
    Parent->GetMainMdt()->GetEntry(x, &Mdtrec);
    Mdtrec.GetFullFileName(&FileName);
    fp = Parent->ffopen(FileName, "rb");
    if (!fp) {
      perror(FileName);
      exit(1);
    }
    fseek(fp, gp - Mdtrec.GetGlobalFileStart(), 0);
    return fp;
  } else {
    return 0;
  }
}

INT INDEX::IsStopWord(PCHR WordStart, INT WordMaximum) const {
  INT x = 0;
  INT WordLength = 0;
  while ( (WordLength < WordMaximum) &&
	 (isalnum(WordStart[WordLength])) ) {
    WordLength++;
  }
  CHR SaveCh = WordStart[WordLength];
  WordStart[WordLength] = '\0';
  INT High = StopWordSize;
  INT Low = 0;
  INT Middle = 0;
  INT Old;
  do {
    Old = Middle;
    Middle = (Low + High) / 2;
    x = strcasecmp(WordStart, stoplist[Middle]);
    if (x == 0) {
      //not good to leave nuls embedded!
      WordStart[WordLength] = SaveCh; 
      return 1;
    }
    if (x < 0) {
      High = Middle;
    }
    if (x > 0) {
      Low = Middle;
    }
  } while (Middle != Old);
  WordStart[WordLength] = SaveCh;
  return 0;
}

GPTYPE INDEX::BuildGpList(
               //@ManMemo: The associated doctype (for calling ParseWords())
               const STRING& Doctype,
               //@ManMemo: Index offset into the text buffer where the document starts.
               INT StartingPosition,
               //@ManMemo: Pointer to beginning of big text buffer.
               PCHR MemoryData,
               //@ManMemo: Length of big text buffer.
               INT MemoryDataLength,
               //@ManMemo Pointer to beginning of remaining GP index list buffer.
               PGPTYPE MemoryIndex,
               //@ManMemo: Length of GP index list buffer remaining.
               INT MemoryIndexLength
               ) 
{
  return ( Parent->ParseWords(Doctype, MemoryData + StartingPosition,
//			      MemoryDataLength,// - StartingPosition,
			      MemoryDataLength - StartingPosition,
			      StartingPosition, MemoryIndex,
			      MemoryIndexLength) );   // Convert parameters to what ParseWords() wants
}

GDT_BOOLEAN INDEX::ValidateInField(const GPTYPE HitGp, const STRING& FieldName,
			   const int PhraseLength) {
  STRING Fn;
  Parent->DfdtGetFileName(FieldName, &Fn);
  PFILE Fp = Parent->ffopen(Fn, "rb");
  if (!Fp) {
    cerr << "Field " << FieldName << " not present in this index." << endl;
    exit(1);
  }
  else {
    fseek(Fp, 0, 2);
    INT Total = ftell(Fp) / ( sizeof(GPTYPE) * 2 );
    INT Low = 0;
    INT High = Total - 1;
    INT X = High / 2;
    INT OX;
    GPTYPE GpS, GpE;
    do {
      OX = X;
      fseek(Fp, X * sizeof(GPTYPE) * 2, 0);
      Parent->GpFread(&GpS, 1, sizeof(GPTYPE), Fp);
      Parent->GpFread(&GpE, 1, sizeof(GPTYPE), Fp);

      if ( (HitGp >= GpS) && (HitGp <= GpE) ) {
	Parent->ffclose(Fp);
	//make sure the entire phrase is within the field
	if (PhraseLength) {
	  INT PhraseEndGp = HitGp + PhraseLength - 1;
//	  cout << PhraseEndGp << "--" << GpS << "," << GpE << endl;
	  if ( (PhraseEndGp >= GpS) && (PhraseEndGp <= GpE) ) {
	    return GDT_TRUE;
	  } else {
	    //since we know fields don't overlap, there is
	    //no match.
	    return GDT_FALSE;
	  }
	}
	else {
	  return GDT_TRUE;
	}
      }
      if (HitGp < GpS) {
	High = X;
      } else {
	Low = X + 1;
      }
      X = (Low + High) / 2;
      if (X < 0) {
	X = 0;
      } else {
	if (X >= Total) {
	  X = Total - 1;
	}
      }
    } while (X != OX);
  }
  Parent->ffclose(Fp);
  return GDT_FALSE;
}

/*
INT INDEX::ValidateInField(const GPTYPE HitGp, const STRING& FieldName) {
  STRING Fn;
  Parent->DfdtGetFileName(FieldName, &Fn);
  GPTYPE GpS, GpE;
  PFILE Fp = fopen(Fn, "rb");
  if (!Fp) {
    perror(Fn);
    exit(1);
  }
  else {
    INT R;
    while ( (R=fread(&GpS, 1, sizeof(GPTYPE), Fp) > 0) ) {
      fread(&GpE, 1, sizeof(GPTYPE), Fp);
      if ( (HitGp >= GpS) && (HitGp <= GpE) ) {
	fclose(Fp);
	return 1;
      }
      if (GpS > HitGp) {
	fclose(Fp);
	return 0;
      }
    }
  }
  fclose(Fp);
  return 0;
}
*/

PIRSET INDEX::RsetOr(const OPOBJ& Set1, const OPOBJ& Set2) const {
/*
	INT x;
	PIRSET NewIrset = new IRSET(Parent);
	*NewIrset = Set1;
	IRESULT OtherIresult;
	for (x=1; x<=Set2.GetTotalEntries(); x++) {
		Set2.GetEntry(x, &OtherIresult);
		NewIrset->AddEntry(OtherIresult, 0);
	}
*/
	return 0;
}

PRSET INDEX::Search(const SQUERY& SearchQuery) {
  // Flip OPSTACK upside-down to convert so we can
  // pop from it in RPN order.
  OPSTACK Stack;
  SearchQuery.GetOpstack(&Stack);
  Stack.Reverse();
  // Pop OPOBJ's, converting OPERAND's to result sets, and
  // executing OPERATOR's
  OPSTACK TempStack;
  POPOBJ OpPtr;
  PIRSET NewIrset;
  ATTRLIST Attrlist;
  STRING Term, FieldName, S;
  INT TermWeight;
  POPOBJ Op1, Op2;
  while (Stack >> OpPtr) {
    if (OpPtr->GetOpType() == TypeOperator) {
      TempStack >> Op1;
      TempStack >> Op2;
      if (OpPtr->GetOperatorType() == OperatorOr) {
	Op1->Or(*Op2);
	Stack << *Op1;
      }
      if (OpPtr->GetOperatorType() == OperatorAnd) {
	Op1->And(*Op2);
	Stack << *Op1;
      }
      if (OpPtr->GetOperatorType() == OperatorAndNot) {
	Op1->AndNot(*Op2);
	Stack << *Op1;
      }
      if (OpPtr->GetOperatorType() == OperatorNear) {
	Op1->Near(*Op2);
	Stack << *Op1;
      }
      delete Op1;
      delete Op2;
    }
    if (OpPtr->GetOpType() == TypeOperand) {
      if (OpPtr->GetOperandType() == TypeRset) {
	TempStack << *OpPtr;
      }
      if (OpPtr->GetOperandType() == TypeTerm) {
	OpPtr->GetTerm(&Term);
	OpPtr->GetAttributes(&Attrlist);
	if (Attrlist.AttrGetRightTruncation()) {
	  Term += "*";
	}
	if (Attrlist.AttrGetFieldName(&S)) {
	  FieldName = S;
	} else {
	  FieldName = "";
	}
	NewIrset = TermSearch(Term, FieldName);
	if (Attrlist.AttrGetTermWeight(&S)) {
	  TermWeight = S.GetInt();
	} else {
	  TermWeight = 1;
	}
	NewIrset->ComputeScores(TermWeight);
	TempStack << *NewIrset;
	delete NewIrset;
      }
    }
  }
  TempStack >> NewIrset;
  NewIrset->SortByScore();
  PRSET Prset = NewIrset->GetRset();
  delete NewIrset; // core dump under 2.7.0 on single term
  return Prset;
}

PRSET INDEX::AndSearch(const SQUERY& SearchQuery) {
  // Convert all operators to AND's
  OPSTACK Stack, TempStack, NewStack;
  POPOBJ OpPtr;
  SQUERY NewQuery;
  SearchQuery.GetOpstack(&Stack);
  while (Stack >> OpPtr) {
    TempStack << *OpPtr;
    delete OpPtr;
  }
  while (TempStack >> OpPtr) {
    if (OpPtr->GetOpType() == TypeOperator) {
      OpPtr->SetOperatorType(OperatorAnd);
    }
    NewStack << *OpPtr;
    delete OpPtr;
  }
  NewQuery.SetOpstack(NewStack);
  return Search(NewQuery);
/*
	INT x, y, round;
	INT TermWeight;
	STRING QueryTerm, WeightString, FieldName;
	STRING SingleWord;
	PIRSET MainIrset;
	PIRSET NewIrset;
	CHR s[StringCompLength];
	SearchQuery.GetTerm(&QueryTerm);
	round = 0;
	while ( !QueryTerm.Equals("") ) {
		FieldName = "";
		x = QueryTerm.SearchReverse(' ');
		SingleWord = QueryTerm;
		SingleWord.EraseBefore(x+1);
		if (x == 0) {
			QueryTerm = "";
		} else {
			QueryTerm.EraseAfter(x-1);
		}
		if ( (y=SingleWord.SearchReverse(':')) > 0) {
			WeightString = SingleWord;
			SingleWord.EraseAfter(y-1);
			WeightString.EraseBefore(y+1);
			CHR Buffer[5];
			WeightString.GetCString(Buffer, 5);
			TermWeight = atoi(Buffer);
		} else {
			TermWeight = 1;
		}
		if ( (y=SingleWord.Search('/')) > 0) {
			FieldName = SingleWord;
			SingleWord.EraseBefore(y+1);
			FieldName.EraseAfter(y-1);
		}
		SingleWord.GetCString(s, StringCompLength);
		if (!IsStopWord(s, strlen(s))) {
			round++;
			if (SingleWord.GetChr(y=SingleWord.GetLength()) == '~') {
				SingleWord.EraseAfter(y-1);
				NewIrset = SoundexSearch(SingleWord, FieldName);
			} else {
				NewIrset = TermSearch(SingleWord, FieldName);
			}
			NewIrset->ComputeScores(TermWeight);
			if (round == 1) {
				MainIrset = NewIrset;
			} else {
				MainIrset->And(*NewIrset);
				delete NewIrset;
			}
		}
	}
	MainIrset->SortByScore();
	PRSET prset = MainIrset->GetRset();
	delete MainIrset;
	return prset;
*/
}

/*
PRSET INDEX::OrSearch(const SQUERY& SearchQuery) {
	INT x, y;
	INT TermWeight;
	STRING QueryTerm, WeightString, FieldName;
	STRING SingleWord;
	PIRSET MainIrset = new IRSET(Parent);
	PIRSET NewIrset;
	CHR s[StringCompLength];
	SearchQuery.GetTerm(&QueryTerm);
	while ( !QueryTerm.Equals("") ) {
		FieldName = "";
		x = QueryTerm.SearchReverse(' ');
		SingleWord = QueryTerm;
		SingleWord.EraseBefore(x+1);
		if (x == 0) {
			QueryTerm = "";
		} else {
			QueryTerm.EraseAfter(x-1);
		}
		if ( (y=SingleWord.SearchReverse(':')) > 0) {
			WeightString = SingleWord;
			SingleWord.EraseAfter(y-1);
			WeightString.EraseBefore(y+1);
			CHR Buffer[5];
			WeightString.GetCString(Buffer, 5);
			TermWeight = atoi(Buffer);
		} else {
			TermWeight = 1;
		}
		if ( (y=SingleWord.Search('/')) > 0) {
			FieldName = SingleWord;
			SingleWord.EraseBefore(y+1);
			FieldName.EraseAfter(y-1);
		}
		SingleWord.GetCString(s, StringCompLength);
		if (!IsStopWord(s, strlen(s))) {
			if (SingleWord.GetChr(y=SingleWord.GetLength()) == '~') {
				SingleWord.EraseAfter(y-1);
				NewIrset = SoundexSearch(SingleWord, FieldName);
			} else {
				NewIrset = TermSearch(SingleWord, FieldName);
			}
			NewIrset->ComputeScores(TermWeight);
			MainIrset->Or(*NewIrset);
			delete NewIrset;
		}
	}
	MainIrset->SortByScore();
	PRSET prset = MainIrset->GetRset();
	delete MainIrset;
	return prset;
}
*/

//private
GDT_BOOLEAN INDEX::GetIndirectBuffer(const GPTYPE Gp, PCHR Buffer, const INT4 Offset) {
  MDTREC Mdtrec;
  STRING FileName;
  PFILE Fp;
  INT x;
  Parent->GetMainMdt()->GetMdtRecord(Gp, &Mdtrec);
  if (Offset != 0) {
    GPTYPE FileStart = Mdtrec.GetGlobalFileStart();
    GPTYPE LocalGp = Gp - FileStart;
    LocalGp += Offset;
    GPTYPE LocalStart = Mdtrec.GetLocalRecordStart();
    GPTYPE LocalEnd = Mdtrec.GetLocalRecordEnd();
    if (LocalGp <= LocalStart || LocalGp >= LocalEnd)
      return GDT_FALSE;
  }
  Mdtrec.GetFullFileName(&FileName);
  Fp = Parent->ffopen(FileName, "rb");
  if (!Fp) {
    perror(FileName);
    exit(1);
  }
  fseek(Fp, Gp - Mdtrec.GetGlobalFileStart() + Offset, 0);
  x = fread(Buffer, 1, StringCompLength, Fp);
  Parent->ffclose(Fp);
  Buffer[x] = '\0';
  return GDT_TRUE;
}

PIRSET INDEX::SoundexSearch(const STRING& QueryTerm, const STRING& FieldName) { 
  // to do this efficiently, we need a soundex index
  // binary search
  PFILE fpi = fopen(IndexFileName, "rb");
  if (!fpi) {
    perror(IndexFileName);
    exit(1);
  }
  GPTYPE gp;
  INT ip, oip, maxip, low, high;
  INT x, z;
  CHR Buffer[StringCompLength+1];
  CHR Term[StringCompLength+1];
  INT done = 0;
  fseek(fpi, 0, 2);
  maxip = (ftell(fpi) / sizeof(GPTYPE)) - 1;
  high = maxip;
  ip = high / 2;
  low = 0;
  INT hit;
  z = 0;
  STRING s1, s2, sx1, sx2;
  Term[0] = toupper(QueryTerm.GetChr(1));
  Term[1] = '\0';
  do {
    hit = 0;
    oip = ip;
    fseek(fpi, ip * sizeof(GPTYPE), 0);
    x = fread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) {
      GetIndirectBuffer(gp, Buffer, 0);
      
      z = strncasecmp(Term, Buffer, 1);
      /*
	 if (z == 0) {
	 if (isalnum(Buffer[strlen(Term)])) {
	 z = -1;
	 }
	 }
	 */
      
      if (z == 0) {
	done = 1;
	hit = 1;
      }
      if (z < 0) {
	high = ip;
      }
      if (z > 0) {
	low = ip + 1;
      }
      ip = (low + high) / 2;
      if (ip < 0) {
	ip = 0;
      }
      if (ip > maxip) {
	ip = maxip;
      }
    } else {
      ip = 0;
      done = 1;
    }
  } while ( (!done) && (ip != oip) );
  
  // find beginning
  INT first = ip;
  INT match = 1;
  while ( (first > 0) && (match) ) {
    first--;
    fseek(fpi, first * sizeof(GPTYPE), 0);
    x = fread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) {
      GetIndirectBuffer(gp, Buffer, 0);
      if (toupper(Buffer[0]) != Term[0]) {
	match = 0;
      }
    } else {
      match = 0;
    }
  }
  
  IRESULT iresult;
  PIRSET pirset = new IRSET(Parent);
  INT w, OK;
  
  do {
    x = fread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) {
      GetIndirectBuffer(gp, Buffer, 0);
      OK = 0;
      if (FieldName.Equals("")) {
	OK = 1;
      } else {
	if (ValidateInField(gp, FieldName)) {
	  OK = 1;
	}
      }
      if (OK) {
	s1 = Buffer;
	s2 = QueryTerm;
	SoundexEncode(s1, &sx1);
	SoundexEncode(s2, &sx2);
	if (sx1.Equals(sx2)) {
	  // match!
	  w = Parent->GetMainMdt()->LookupByGp(gp);
	  iresult.SetMdtIndex(w);
	  iresult.SetHitCount(1);
	  iresult.SetScore(0);
	  pirset->AddEntry(iresult, 1);
	}
      }
    }
  } while (toupper(Buffer[0]) == Term[0]);
  fclose(fpi);
  return pirset;
}

INT INDEX::Match(const CHR *QueryTerm, const INT TermLength, const GPTYPE gp, const INT4 Offset) {
  CHR Buffer[StringCompLength+1];
  INT z;
       
  if (!GetIndirectBuffer(gp, Buffer, Offset))
    return -1;

  for (z = 0; Buffer[z]; z++)
    if (!isalnum(Buffer[z]))
      Buffer[z] = ' ';
  
  if ( QueryTerm[TermLength - 1] == '*' ) 
    z = strncasecmp(QueryTerm, Buffer, TermLength - 1);
  else {
    z = strncasecmp(QueryTerm, Buffer, TermLength);
    if ( z == 0 && isalnum(Buffer[TermLength]) )
      z = -1;
  }
#ifdef DEBUG
  cout << "Compared Term=" << QueryTerm << " with Buffer=" << Buffer << endl;
#endif
  
  return z;
}

PIRSET INDEX::TermSearch(const STRING& QueryTerm, const STRING& FieldName) {
  // binary search
  PFILE fpi = Parent->ffopen(IndexFileName, "rb");
  if (!fpi) {
    perror(IndexFileName);
    exit(1);
  }
  GPTYPE gp;
  INT ip, oip, maxip, low, high;
  INT x, z, TermLength, OrigTermLength;
  CHR OrigTerm[StringCompLength+1], *Term;
//  INT x, z;
//  CHR Buffer[StringCompLength+1];
//  CHR Term[StringCompLength+1];
  INT done = 0;
  fseek(fpi, 0, 2);
  maxip = (ftell(fpi) / sizeof(GPTYPE)) - 1;
  high = maxip;
  ip = high / 2;
  low = 0;
  INT hit;
  z = 0;

  QueryTerm.GetCString(OrigTerm, sizeof(OrigTerm));
  OrigTermLength = QueryTerm.GetLength();

  //because of sorting unpleasantness, 
  //we must convert non alnums in phrases to spaces
  //for phrase searches we need to look past 
  //all stop words, and start with the first
  //indexed word. later we'll check backwords in the data.
  INT PhraseEnd = OrigTermLength;
  INT n, PhraseBeg = 0, FoundBeg=0;
  if (OrigTerm[OrigTermLength - 1] == '*')
    PhraseEnd--;
  for (n=0; n < PhraseEnd; n++) {
    if (!isalnum(OrigTerm[n])) {
      OrigTerm[n] = ' ';
      if (!FoundBeg && IsStopWord(OrigTerm+PhraseBeg, n - PhraseBeg)) 
	PhraseBeg = n + 1;
      else
	FoundBeg = 1;
    }
  }
  
  if (PhraseBeg >= OrigTermLength) {
    //it's all stop words. return an empty IRSET.
    PIRSET pirset = new IRSET(Parent);
    return pirset;
  }
  Term = OrigTerm + PhraseBeg; 
  TermLength = OrigTermLength - PhraseBeg;

  do {
    hit = 0;
    oip = ip;
    fseek(fpi, ip * sizeof(GPTYPE), 0);
    x = Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) {
      z = Match(Term, TermLength, gp);
      if (z == 0) {
	done = 1;
	hit = 1;
      }
      else if (z < 0) {
//	high = ip;
	high = ip-1;
      }
      else if (z > 0) {
	low = ip + 1;
      }
      ip = (low + high) / 2;
      if (ip < 0) {
	ip = 0;
      }
      if (ip > maxip) {
	ip = maxip;
      }
    } else {
      ip = 0;
      done = 1;
    }
//  } while ( (!done) && (ip != oip) );
  } while ( (!done) && (high >= low) );
  
  
  if (!hit) {
    PIRSET pirset = new IRSET(Parent);
    return pirset;
  }
  
  // bracket hits
  INT first, last;
  INT match, nomatch;
  
  // find first
  low = 0;
  high = ip;
  first = high / 2;
  match = ip;
  nomatch = 0;
  do {
    fseek(fpi, first * sizeof(GPTYPE), 0);
    x = Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) 
      z = Match(Term, TermLength, gp);
    if (z == 0) {
      match = first;
      high = first;
    } else {
      nomatch = first;
      low = first + 1;
    }
    first = (low + high) / 2;
    if (first < 0) {
      first = 0;
    } else {
      if (first > ip) {
	first = ip;
      }
    }
  } while ( (match - nomatch) > 5 );
  first = match;
  do {
    if (first > 0) {
      first--;
    }
    fseek(fpi, first * sizeof(GPTYPE), 0);
    x = Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) 
      z = Match(Term, TermLength, gp);
  } while ( (z == 0) && (first > 0) );
  if ( (z != 0) || (first > 0) ) {
    first++;
  }
  
  
  // find last
  low = ip;
  high = maxip;
  last = (high + low) / 2;
  match = ip;
  nomatch = maxip;
  do {
    fseek(fpi, last * sizeof(GPTYPE), 0);
    x = Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) 
      z = Match(Term, TermLength, gp);
    if (z == 0) {
      match = last;
      low = last + 1;
    } else {
      nomatch = last;
      high = last;
    }
    last = (low + high) / 2;
    if (last < ip) {
      last = ip;
    } else {
      if (last > maxip) {
	last = maxip;
      }
    }
  } while ( (nomatch - match) > 5 );
  last = match;
  do {
    if (last < maxip) {
      last++;
    }
    fseek(fpi, last * sizeof(GPTYPE), 0);
    x = Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi);
    if (x) 
      z = Match(Term, TermLength, gp);
  } while ( (z == 0) && (last < maxip) );
  if ( (z != 0) || (last < maxip) ) {
    last--;
  }
  
//	first++;
//	last--;

  // Build result set
  IRESULT iresult;
  MDTREC mdtrec;
  GPTYPE GlobalRecEnd;
  PIRSET pirset = new IRSET(Parent);
  PGPTYPE gplist = new GPTYPE[last-first+1];
  INT w;
  INT OK;
  PFCT Pfct;
  FC Fc;
/*
  INT QueryLength;
  if (QueryTerm.GetChr((x=QueryTerm.GetLength())) == '*') {
    QueryLength = x - 1;	// ignore "*" at end
  } else {
    QueryLength = x;
  }
*/
  fseek(fpi, first * sizeof(GPTYPE), 0);
  x = Parent->GpFread(gplist, 1, (last-first+1) * sizeof(GPTYPE), fpi) / sizeof(GPTYPE);
  fclose(fpi);
  pirset->Resize(pirset->GetTotalEntries() + x);	// resize to total size ahead of time

  INT Offset = TermLength - OrigTermLength;

  INT TermLenNoStar;

  if (QueryTerm.GetChr(OrigTermLength) == '*') {
    TermLenNoStar = OrigTermLength - 1;     // ignore "*" at end
  } else {
    TermLenNoStar = OrigTermLength;
  }

  Pfct = new FCT();
  for (ip=0; ip<x; ip++) {
    OK = 0;
    if (TermLength != OrigTermLength) {
      if ( (Match(OrigTerm, OrigTermLength, gplist[ip], Offset)) == 0)  
	gplist[ip] += Offset;
      else
	continue;
    }
                               
    if (FieldName.Equals("")) {
      OK = 1;
    } else {
      if (ValidateInField(gplist[ip], FieldName, TermLenNoStar)) {
	OK = 1;
      }
    }
    if (OK) {
      //make sure that phrases don't go past 
      //the end of the local record.
      w = Parent->GetMainMdt()->GetMdtRecord(gplist[ip], &mdtrec);
      GlobalRecEnd = mdtrec.GetGlobalFileStart() + 
	mdtrec.GetLocalRecordEnd();
      if  ( !((GlobalRecEnd - gplist[ip]) >= (TermLenNoStar - 1)) )
	continue;
      iresult.SetMdtIndex(w);
      iresult.SetHitCount(1);
      iresult.SetScore(0);
      Fc.SetFieldStart(gplist[ip]);
      Fc.SetFieldEnd(gplist[ip] + TermLength - 1);
      Pfct->Clear();
      Pfct->AddEntry(Fc);
      iresult.SetHitTable(*Pfct);
      pirset->AddEntry(iresult, 1);
    }
  }
  delete Pfct;
  delete [] gplist;
  return pirset;
  
}

void INDEX::DumpIndex(INT DebugSkip) {
  PFILE fpi = fopen(IndexFileName, "rb");
  if (!fpi) {
    perror(IndexFileName);
    cout << "(no index)" << endl;
    return;
  }
  PFILE fpd;
  GPTYPE gp;
  MDTREC mdtrec;
  INT x, y, j;
  CHR Buffer[StringCompLength+1], Term[StringCompLength+1];
  Term[0] = '\0';
  STRING FileName;
  
  if (DebugSkip > 0) {
    fseek(fpi, sizeof(GPTYPE)*DebugSkip, SEEK_SET);
    cout << "Skipping " << DebugSkip << "SIStrings." << endl;
  }
  
  y = 0;
  while (Parent->GpFread(&gp, 1, sizeof(GPTYPE), fpi) > 0) {
    Parent->GetMainMdt()->GetMdtRecord(gp, &mdtrec);
    mdtrec.GetFullFileName(&FileName);
    fpd = fopen(FileName, "rb");
    if (!fpd) {
      perror(FileName);
      exit(1);
    }
    y++;
//    cout << "gp=" << gp << '\t';
    cout << "SIString#" << DebugSkip+y << '\t';
    fseek(fpd, gp - mdtrec.GetGlobalFileStart(), 0);
    x = fread(Buffer, 1, StringCompLength, fpd);
    fclose(fpd);
    
    for (j=x; j<StringCompLength; j++) {
      Buffer[j] = ' ';
    }
    
    // convert all non-alphas to spaces.  If we add phrase
    // searching, this should be eliminated.
    for (j=0; j<StringCompLength; j++) {
      if (!isalnum(Buffer[j])) {
	Buffer[j] = ' ';
      }
    }
    
    //star if the current entry is out of order.
    if( (strncasecmp(Term, Buffer, StringCompLength)) > 0)
      cout << "(*)";
    
    // store current term for comparison next time
    memcpy(Term, Buffer, StringCompLength);
    
//    cout << FileName << ":" << DebugSkip+y << ":";
    cout << FileName << ":" << gp << ":";
    cout << gp - mdtrec.GetGlobalFileStart() << endl;

    Buffer[x] = '\0';
    cout << "-->" << Buffer << "<--" << endl;
//    FileName.Print();
    cout << endl;
    
    
  }
  fclose(fpi);
}

INDEX::~INDEX() {
#ifdef DICTIONARY
	delete Dict;
#endif
}
