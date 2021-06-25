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
File:		idb.cxx
Version:	1.00
Description:	Class IDB
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "idb.hxx"
#include "common.hxx"

IDB::IDB(const STRING& NewPathName, const STRING& NewFileName) {
	STRLIST EmptyList;
	Initialize(NewPathName, NewFileName, EmptyList);
}

IDB::IDB(const STRING& NewPathName, const STRING& NewFileName, const STRLIST& NewDocTypeOptions) {
	Initialize(NewPathName, NewFileName, NewDocTypeOptions);
}

void IDB::Initialize(const STRING& NewPathName, const STRING& NewFileName,
		const STRLIST& NewDocTypeOptions) {
	DebugMode = 0;
	DebugSkip = 0;
	TotalRecordsQueued = 0;
	DbPathName = NewPathName;
	AddTrailingSlash(&DbPathName);
	ExpandFileSpec(&DbPathName);
	DbFileName = NewFileName;
	RemovePath(&DbFileName);
	// Load DbInfo file
	STRING DbInfoFn;
	ComposeDbFn(&DbInfoFn, DbExtDbInfo);
	MainRegistry = new REGISTRY("Isearch");
	DbInfoChanged = GDT_FALSE;
	STRLIST Position;
	MainRegistry->LoadFromFile(DbInfoFn, Position);
	SetWrongEndian();
	// Create INDEX
	STRING IndexFN;
	ComposeDbFn(&IndexFN, DbExtIndex);
	MainIndex = new INDEX(this, IndexFN);
	// Create and load MDT
	STRING MDTFN;
	ComposeDbFn(&MDTFN, DbExtMdt);
	STRING FileStem;
	GetDbFileStem(&FileStem);
	MainMdt = new MDT(FileStem, IsWrongEndian());
//	MainMdt->LoadTable(MDTFN);
//	if (IsWrongEndian()) {
//		MainMdt->FlipBytes();
//	}
	STRING DFDTFN;
	ComposeDbFn(&DFDTFN, DbExtDfd);
	MainDfdt = new DFDT();
	MainDfdt->LoadTable(DFDTFN);
#if defined(_MSDOS) && !defined(_WIN32)
	UINT4 DefaultMemSize = 16;
#else
	UINT4 DefaultMemSize = 1024;
#endif
	DefaultMemSize *= 1024;
	IndexingMemory = DefaultMemSize;
	DocTypeOptions = NewDocTypeOptions;
	DocTypeReg = new DTREG(this);
}

GDT_BOOLEAN IDB::IsWrongEndian() const {
	return WrongEndian;
}

void IDB::SetWrongEndian() {
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("BigEndian");
	MainRegistry->GetData(Position, &Value);
	STRING S;
	Value.GetEntry(1, &S);
	WrongEndian = (S.GetInt() != IsBigEndian()) ? GDT_TRUE : GDT_FALSE;
}

#ifdef DICTIONARY
void IDB::CreateDictionary(void) {
	MainIndex->CreateDictionary();
}

void IDB::CreateCentroid(void) {
	MainIndex->CreateCentroid();
}
#endif

SIZE_T IDB::GpFwrite(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, FILE* Stream) const {
	if (IsWrongEndian()) {
		SIZE_T y;
		for (y=0; y<((Size*NumElements)/sizeof(GPTYPE)); y++) {
			GpSwab(Ptr + y);
		}
	}
	INT val;
	val = fwrite(Ptr, Size, NumElements, Stream);
	if (val < NumElements) {
	  cout << "ERROR: Can't Complete Write!" << 
	    strerror(errno) << endl;
	}
	return val;
//	return fwrite(Ptr, Size, NumElements, Stream);
}

SIZE_T IDB::GpFread(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, FILE* Stream) const {
	SIZE_T x = fread(Ptr, Size, NumElements, Stream);
	if ( (x) && (IsWrongEndian()) ) {
		SIZE_T y;
		for (y=0; y<((Size *x)/sizeof(GPTYPE)); y++) {
			GpSwab(Ptr + y);
		}
	}
	return x;
}

GDT_BOOLEAN IDB::IsDbCompatible() const {
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("MagicNumber");
	MainRegistry->GetData(Position, &Value);
	STRING S;
	Value.GetEntry(1, &S);
	return (S.GetInt() == IsearchMagicNumber) ? GDT_TRUE : GDT_FALSE;
}

void IDB::GetAllDocTypes(PSTRLIST StringListBuffer) const {
	DocTypeReg->GetDocTypeList(StringListBuffer);
}

void IDB::GetDocTypeOptions(PSTRLIST StringListBuffer) const {
	*StringListBuffer = DocTypeOptions;
}

void IDB::KeyLookup(const STRING& Key, PRESULT ResultBuffer) const {
	MDTREC Mdtrec;
	MainMdt->GetMdtRecord(Key, &Mdtrec);
	STRING S;
	Mdtrec.GetKey(&S);
	ResultBuffer->SetKey(S);
	Mdtrec.GetDocumentType(&S);
	ResultBuffer->SetDocumentType(S);
	Mdtrec.GetPathName(&S);
	ResultBuffer->SetPathName(S);
	Mdtrec.GetFileName(&S);
	ResultBuffer->SetFileName(S);
	ResultBuffer->SetRecordStart(Mdtrec.GetLocalRecordStart());
	ResultBuffer->SetRecordEnd(Mdtrec.GetLocalRecordEnd());
}

void IDB::GetDfdt(PDFDT DfdtBuffer) const {
	*DfdtBuffer = *MainDfdt;
}

void IDB::GetRecordDfdt(const STRING& Key, PDFDT DfdtBuffer) const {
	DFDT EmptyDfdt;
	*DfdtBuffer = EmptyDfdt;
	MDTREC Mdtrec;
	MainMdt->GetMdtRecord(Key, &Mdtrec);
	GPTYPE MdtS = Mdtrec.GetGlobalFileStart() + Mdtrec.GetLocalRecordStart();
	GPTYPE MdtE = Mdtrec.GetGlobalFileStart() + Mdtrec.GetLocalRecordEnd();
	INT c = MainDfdt->GetTotalEntries();
	INT x;
	DFD dfd;
	STRING FieldName, Fn;
	PFILE Fp;
	INT Done;
	for (x=1; x<=c; x++) {
		MainDfdt->GetEntry(x, &dfd);
		dfd.GetFieldName(&FieldName);
		DfdtGetFileName(FieldName, &Fn);
		Fp = fopen(Fn, "rb");
		if (!Fp) {
			perror(Fn);
			exit(1);
		}
		else {
			Done = 0;
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
				GpFread(&GpS, 1, sizeof(GPTYPE), Fp);
				GpFread(&GpE, 1, sizeof(GPTYPE), Fp);
				if ( (MdtS <= GpS) && (MdtE >= GpE) ) {
					fclose(Fp);
					Done = 1;
					DfdtBuffer->AddEntry(dfd);
				}
				if (MdtE < GpS) {
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
			} while ( (X != OX) && (!Done) );
			fclose(Fp);
		}
	}
}

void IDB::ComposeDbFn(PSTRING StringBuffer, const PCHR Suffix) const {
	GetDbFileStem(StringBuffer);
	StringBuffer->Cat(Suffix);
}

void IDB::GetDbFileStem(PSTRING StringBuffer) const {
	*StringBuffer = DbPathName;
	StringBuffer->Cat(DbFileName);
}

/*

void IDB::SetTitle(const STRING& NewTitle) {
	Title = NewTitle;
}

void IDB::GetTitle(PSTRING StringBuffer) {
	*StringBuffer = Title;
}

void IDB::SetComments(const STRING& NewComments) {
	Comments = NewComments;
}

void IDB::GetComments(PSTRING StringBuffer) {
	*StringBuffer = Comments;
}

*/

void IDB::DfdtAddEntry(const DFD& NewDfd) {
	MainDfdt->AddEntry(NewDfd);
}

void IDB::DfdtGetEntry(const INT Index, PDFD DfdRecord) const {
	MainDfdt->GetEntry(Index, DfdRecord);
}

INT IDB::DfdtGetTotalEntries() const {
	return MainDfdt->GetTotalEntries();
}

PDOCTYPE IDB::GetDocTypePtr(const STRING& DocType) const {
	PDOCTYPE DoctypePtr = DocTypeReg->GetDocTypePtr(DocType);
	if (DoctypePtr) {
		return DoctypePtr;
	} else {
		return DocTypeReg->GetDocTypePtr("");
	}
}

GDT_BOOLEAN IDB::ValidateDocType(const STRING& DocType) const {
	return (DocTypeReg->GetDocTypePtr(DocType) != NULL)? GDT_TRUE : GDT_FALSE;
}

void IDB::IndexingStatus(const INT StatusMessage, const PSTRING FileName,
		const INT WordCount) const {
}

INT IDB::GetTotalRecords() const {
	return MainMdt->GetTotalEntries();
}

void IDB::SetIndexingMemory(const UINT4 MemorySize) {
#if defined(_MSDOS) && !defined(_WIN32)
  UINT4 DefaultMemSize = 16;
#else
	UINT4 DefaultMemSize = 1024;
#endif
	DefaultMemSize *= 1024;
	if (MemorySize < DefaultMemSize) {
		IndexingMemory = DefaultMemSize;
		// ER
		return;
	}
	IndexingMemory = MemorySize;
}

UINT4 IDB::GetIndexingMemory() const {
	return IndexingMemory;
}

/*
void IDB::GenerateKeys() {
	MainMdt->GenerateKeys();
}
*/

void IDB::DebugModeOn() {
	DebugMode = 1;
}

void IDB::DebugModeOff() {
	DebugMode = 0;
}

void IDB::SetDebugSkip(const INT Skip) {
	DebugSkip = Skip;
}

PRSET IDB::AndSearch(const SQUERY& SearchQuery) {
	return MainIndex->AndSearch(SearchQuery);
}

PRSET IDB::Search(const SQUERY& SearchQuery) {
	if (!IsDbCompatible()) {
		return (new RSET);
	}
	STRING GlobalDoctype;
	GetGlobalDocType(&GlobalDoctype);
	PDOCTYPE DoctypePtr;
	DoctypePtr = DocTypeReg->GetDocTypePtr(GlobalDoctype);
	SQUERY Query;
	Query = SearchQuery;
	DoctypePtr->BeforeSearching(&Query);
	PRSET RsetPtr;
	RsetPtr = MainIndex->Search(Query);
	RsetPtr = DoctypePtr->AfterSearching(RsetPtr);
	return RsetPtr;
}

void IDB::BeginRsetPresent(const STRING& RecordSyntax) {
	STRING GlobalDoctype;
	GetGlobalDocType(&GlobalDoctype);
	PDOCTYPE DoctypePtr;
	DoctypePtr = DocTypeReg->GetDocTypePtr(GlobalDoctype);
	DoctypePtr->BeforeRset(RecordSyntax);
}

void IDB::EndRsetPresent(const STRING& RecordSyntax) {
	STRING GlobalDoctype;
	GetGlobalDocType(&GlobalDoctype);
	PDOCTYPE DoctypePtr;
	DoctypePtr = DocTypeReg->GetDocTypePtr(GlobalDoctype);
	DoctypePtr->AfterRset(RecordSyntax);
}

void IDB::DfdtGetFileName(const STRING& FieldName, PSTRING StringBuffer) const {
	DFD Dfd;
	MainDfdt->GetDfdRecord(FieldName, &Dfd);
	INT FileNumber = Dfd.GetFileNumber();
	CHR s[10];
	INT x, y;
	ComposeDbFn(StringBuffer, ".");
	if (FileNumber > 999) {
		FileNumber = 0;
	}
	sprintf(s, "%d", FileNumber);
	y = 3 - strlen(s);
	for (x=1; x<=y; x++) {
		StringBuffer->Cat("0");
	}
	StringBuffer->Cat(s);
}

/*
void IDB::GetRecordData(const RESULT& ResultRecord, PSTRING StringBuffer) const {
	*StringBuffer = "";
	STRING fn;
	ResultRecord.GetFullFileName(&fn);
	PFILE fp = fopen(fn, "rb");
	if (!fp) {
		perror(fn);
		exit(1);
	}
	else {
		INT rs = ResultRecord.GetRecordStart();
		INT re = ResultRecord.GetRecordEnd();
		INT size = re - rs + 1;
		fseek(fp, rs, 0);
		PCHR p = new CHR[size+1];
		p[fread(p, 1, size, fp)] = '\0';
		fclose(fp);
		*StringBuffer = p;
		delete [] p;
	}
}
*/

static FILE* GlobalFcFp;
static GDT_BOOLEAN GlobalWrongEndian;

static int IdbCompareFcsOnDisk(const void* FcPtr1, const void* FcPtr2) {
	fseek(GlobalFcFp, (((LONG)FcPtr2) - 1) * sizeof(FC), SEEK_SET);
	static FC Fc;
	fread(&Fc, 1, sizeof(Fc), GlobalFcFp);
	if (GlobalWrongEndian) {
		Fc.FlipBytes();
	}
	if ( ( ((FC*)FcPtr1)->GetFieldStart() <= Fc.GetFieldStart() ) &&
			( ((FC*)FcPtr1)->GetFieldEnd() >= Fc.GetFieldEnd() ) ) {
		return 0;
	} else {
		if ( ((FC*)FcPtr1)->GetFieldStart() < Fc.GetFieldStart() ) {
			return -1;
		} else {
			return 1;
		}
	}
}

GDT_BOOLEAN IDB::GetFieldData(const RESULT& ResultRecord, 
			      const STRING& FieldName,
			      STRING* StringBuffer) const 
{
  STRLIST Strlist;
  GDT_BOOLEAN Status;
  Status = GetFieldData(ResultRecord, FieldName, &Strlist);
  if (Status)
    Strlist.Join(",", StringBuffer);
  return(Status);
}

GDT_BOOLEAN IDB::GetFieldData(const RESULT& ResultRecord, 
			      const STRING& FieldName,
			      STRLIST* StrlistBuffer) const 
{
  StrlistBuffer->Clear();
  STRING DfFileName;
  DfdtGetFileName(FieldName, &DfFileName);
  PFILE fp = fopen(DfFileName, "rb");
  if (!fp) {
    perror(DfFileName);
    return(GDT_FALSE);
  }
  else {
    STRING ResultKey;
    MDTREC MdtRecord;
    ResultRecord.GetKey(&ResultKey);
    MainMdt->GetMdtRecord(ResultKey, &MdtRecord);
    INT GpStart = MdtRecord.GetGlobalFileStart() + MdtRecord.GetLocalRecordStart();
    INT GpEnd = MdtRecord.GetGlobalFileStart() + MdtRecord.GetLocalRecordEnd();
    PFILE fpd;
    PCHR p;
    INT x, y;
    STRING Fn;
    // binary search to find matching FC pair
    fseek(fp, 0, 2);
    LONG Size = ftell(fp);
    FC Fc;
    Fc.SetFieldStart(GpStart);
    Fc.SetFieldEnd(GpEnd);
    FC* FcPtr;
    GlobalFcFp = fp;
    GlobalWrongEndian = IsWrongEndian();
    FcPtr = (FC*)bsearch(&Fc, (void*)1, Size / sizeof(FC), 1, IdbCompareFcsOnDisk);
    if (FcPtr) {
      LONG Pos, InitPos;
      GPTYPE OldStart, OldEnd;
      Pos = (((LONG)FcPtr) - 1);
      fseek(fp, Pos * sizeof(FC), SEEK_SET);
      GPTYPE GpPair[2];
      GpFread(GpPair, 1, sizeof(FC), fp);
      InitPos = Pos;
      // work backwards to find first pair
      while ( (Pos >= 0) && (GpPair[0] >= Fc.GetFieldStart()) && (GpPair[1] <= Fc.GetFieldEnd()) ) {
	
	OldStart = GpPair[0];
	OldEnd = GpPair[1];
	
	// extract field from document
	MdtRecord.GetFullFileName(&Fn);
	fpd = fopen(Fn, "rb");
	if (!fpd) {
	  perror(Fn);
	  return(GDT_FALSE);
	}
	else {
	  x = GpPair[1] - GpPair[0] + 1;
	  p = new CHR[x+1];
	  fseek(fpd, (long)(GpPair[0] - MdtRecord.GetGlobalFileStart()), 0);
	  y = fread(p, 1, x, fpd);
	  p[y] = '\0';
	  StrlistBuffer->AddEntry(p);
	  delete [] p;
	  fclose(fpd);
	}

	Pos--;
	fseek(fp, Pos * sizeof(FC), SEEK_SET);
	GpFread(GpPair, 1, sizeof(FC), fp);
      }
      StrlistBuffer->Reverse();
      
      GpPair[0] = OldStart;
      GpPair[1] = OldEnd;
      // work forwards to find last pair
      Pos = InitPos + 1;
      SIZE_T BytesRead = sizeof(FC);
      while ( (BytesRead == sizeof(FC)) && (GpPair[0] >= Fc.GetFieldStart()) &&
	     (GpPair[1] <= Fc.GetFieldEnd()) ) {
	fseek(fp, Pos * sizeof(FC), SEEK_SET);
	BytesRead = GpFread(GpPair, 1, sizeof(FC), fp);
	if ( (BytesRead == sizeof(FC)) && (GpPair[0] >= Fc.GetFieldStart()) &&
	    (GpPair[1] <= Fc.GetFieldEnd()) ) {	// I know this is ugly
	  // extract field from document
	  MdtRecord.GetFullFileName(&Fn);
	  fpd = fopen(Fn, "rb");
	  if (!fpd) {
	    perror(Fn);
	    return(GDT_FALSE);
	  }
	  else {
	    x = GpPair[1] - GpPair[0] + 1;
	    p = new CHR[x+1];
	    fseek(fpd, (long)(GpPair[0] - MdtRecord.GetGlobalFileStart()), 0);
	    y = fread(p, 1, x, fpd);
	    p[y] = '\0';
	    StrlistBuffer->AddEntry(p);
	    delete [] p;
	    fclose(fpd);
	  }
	}
	Pos++;
      }

/*
   // extract field from document
   MdtRecord.GetFullFileName(&Fn);
   fpd = fopen(Fn, "rb");
   if (!fpd) {
   perror(Fn);
   exit(1);
   }
   else {
   x = OldEnd - OldStart + 1;
   p = new CHR[x+1];
   fseek(fpd, (long)(OldStart - MdtRecord.GetGlobalFileStart()), 0);
   y = fread(p, 1, x, fpd);
   p[y] = '\0';
   StringBuffer->Cat(p);
   delete [] p;
   fclose(fpd);
   }
   */
    }
    fclose(fp);
    return(GDT_TRUE);
  }
}

void IDB::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		const STRING& RecordSyntax, PSTRING StringBuffer) const {
	STRING ESet;
	ESet = ElementSet;
	ESet.UpperCase();
	STRING DocType;
	ResultRecord.GetDocumentType(&DocType);
	PDOCTYPE DocTypePtr = DocTypeReg->GetDocTypePtr(DocType);
	DocTypePtr->Present(ResultRecord, ESet, RecordSyntax, StringBuffer);
}

void IDB::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer) const {
	STRING RecordSyntax;
	Present(ResultRecord, ElementSet, RecordSyntax, StringBuffer);
}

PMDT IDB::GetMainMdt() {
	return MainMdt;
}

PDFDT IDB::GetMainDfdt() {
	return MainDfdt;
}

void IDB::GetDbVersionNumber(PSTRING StringBuffer) const {
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("VersionNumber");
	MainRegistry->GetData(Position, &Value);
	Value.GetEntry(1, StringBuffer);
}

void IDB::GetIsearchVersionNumber(PSTRING StringBuffer) const {
	*StringBuffer = IsearchVersion;
}

void IDB::AddRecord(const RECORD& NewRecord) {
	STRING IndexingQueueFn;
	ComposeDbFn(&IndexingQueueFn, DbExtIndexQueue1);
	PFILE fp;
	fp = IDB::ffopen(IndexingQueueFn, "a");
	if (!fp) {
		perror(IndexingQueueFn);
		exit(1);
	}
	fprintf(fp, "#\n");
	NewRecord.Write(fp);
	IDB::ffclose(fp);
}

void IDB::DocTypeAddRecord(const RECORD& NewRecord) {
	STRING IndexingQueueFn;
	ComposeDbFn(&IndexingQueueFn, DbExtIndexQueue2);
	PFILE fp;
	fp = IDB::ffopen(IndexingQueueFn, "a");
	if (!fp) {
		perror(IndexingQueueFn);
		exit(1);
	}
	fprintf(fp, "#\n");
	NewRecord.Write(fp);
	IDB::ffclose(fp);
	TotalRecordsQueued++;
}

void IDB::Index() {
	if (!IsDbCompatible()) {
		return;
	}
	STRING GlobalDoctype;
	GetGlobalDocType(&GlobalDoctype);
	PDOCTYPE DoctypePtr;
	DoctypePtr = DocTypeReg->GetDocTypePtr(GlobalDoctype);
	DoctypePtr->BeforeIndexing();
	IndexingStatus(IndexingStatusParsingFiles, 0, 0);
	STRING IqFn;
	ComposeDbFn(&IqFn, DbExtIndexQueue1);
	PFILE fp;
	fp = IDB::ffopen(IqFn, "r");
	if (!fp) {
		perror(IqFn);
		exit(1);
	}
	STRING s;
	RECORD Record;
	STRING DocType;
	PDOCTYPE DocTypePtr;
	// Check whether we need to set the Global DocType
	STRING GDocType;
	GDT_BOOLEAN SetGDocType;
	GetGlobalDocType(&GDocType);
	if (GDocType == "") {
		SetGDocType = GDT_TRUE;
	} else {
		SetGDocType = GDT_FALSE;
	}
	do {
		s.FGet(fp, 3);
		if (s == "#") {
			Record.Read(fp);	// Read a record from file queue
			Record.GetDocumentType(&DocType);
			if (SetGDocType == GDT_TRUE) {	// Set Global DocType
				SetGlobalDocType(DocType);
				SetGDocType = GDT_FALSE;
			}
			DocTypePtr = DocTypeReg->GetDocTypePtr(DocType);
			DocTypePtr->AddFieldDefs();
			if (Record.GetRecordEnd() == 0) {
				DocTypePtr->ParseRecords(Record);
			} else {
				DocTypeAddRecord(Record);
			}
		}
	} while (s == "#");
	IDB::ffclose(fp);
	StrUnlink(IqFn);
	MainMdt->Resize(MainMdt->GetTotalEntries() + TotalRecordsQueued);
	ComposeDbFn(&IqFn, DbExtIndexQueue2);
	fp = IDB::ffopen(IqFn, "r");
	if (!fp) {
		cout << "No valid files found for indexing..." << endl;
		exit(1);
	}
	MainIndex->AddRecordList(fp);
	IDB::ffclose(fp);
	StrUnlink(IqFn);
	TotalRecordsQueued = 0;
	MainFpt.CloseAll();
	DoctypePtr->AfterIndexing();
}

void IDB::ParseFields(PRECORD Record) {
	PDOCTYPE DocTypePtr;
	STRING DocType;
	Record->GetDocumentType(&DocType);
	DocTypePtr = GetDocTypePtr(DocType);
	DocTypePtr->ParseFields(Record);
}

INT IDB::IsStopWord(CHR* WordStart, INT WordMaximum) const {
  return ( MainIndex->IsStopWord(WordStart, WordMaximum) );
}
  
GPTYPE IDB::ParseWords(const STRING& Doctype, CHR* DataBuffer, INT DataLength,
		       INT DataOffset, GPTYPE* GpBuffer, INT GpLength) {
  // Redirect the call to this method to the appropriate doctype.
  PDOCTYPE DocTypePtr;
  STRING DocType;
  DocTypePtr = GetDocTypePtr(DocType);
  return ( DocTypePtr->ParseWords(DataBuffer, DataLength, DataOffset, GpBuffer, GpLength) );
}

/*
void IDB::SelectRegions(const RECORD& Record, FCT* RegionsPtr) const {
	PDOCTYPE DocTypePtr;
	STRING DocType;
	Record.GetDocumentType(&DocType);
	DocTypePtr = GetDocTypePtr(DocType);
	RegionsPtr->Clear();
	DocTypePtr->SelectRegions(Record, RegionsPtr);
}
*/

PFILE IDB::ffopen(const STRING& FileName, const PCHR Type) {
//	return MainFpt.ffopen(FileName, Type);
	return fopen(FileName, Type);
}

INT IDB::ffclose(PFILE FilePointer) {
//	return MainFpt.ffclose(FilePointer);
	return fclose(FilePointer);
}



INT IDB::IsSystemFile(const STRING& FileName) {
	STRING s;
	ComposeDbFn(&s, DbExtIndex);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtMdt);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtMdtKeyIndex);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtMdtGpIndex);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtDfd);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtTemp);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtIndexQueue1);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtIndexQueue2);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, DbExtDbInfo);
	if (s.Equals(FileName)) {
		return 1;
	}
	ComposeDbFn(&s, ".");
	STRING t;
	CHR b[10];
	INT x = 1;
	INT y, z;
	do {
		t = s;
		sprintf(b, "%d", x);
		y = 3 - strlen(b);
		for (z=1; z<=y; z++) {
			t.Cat("0");
		}
		t.Cat(b);
		if (t.Equals(FileName)) {
			return 1;
		}
		x++;
	} while (x < 1000);
	return 0;
}

void IDB::KillAll() {
	// Delete files
	STRING s;
	ComposeDbFn(&s, DbExtIndex);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtMdt);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtMdtKeyIndex);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtMdtGpIndex);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtDfd);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtTemp);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtIndexQueue1);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtIndexQueue2);
	StrUnlink(s);
	ComposeDbFn(&s, DbExtDbInfo);
	StrUnlink(s);
	ComposeDbFn(&s, ".mno");	// temporary
	StrUnlink(s);
	INT t = MainDfdt->GetTotalEntries();
	INT x;
	DFD Dfd;
	STRING f;
	for (x=1; x<=t; x++) {
		MainDfdt->GetEntry(x, &Dfd);
		Dfd.GetFieldName(&f);
		DfdtGetFileName(f, &s);
		StrUnlink(s);
	}
	// Delete objects
	if (MainIndex) {
		delete MainIndex;
	}
	if (MainMdt) {
		delete MainMdt;
	}
	if (MainDfdt) {
		delete MainDfdt;
	}
	delete DocTypeReg;
	// Re-init objects
	STRING IndexFN;
	ComposeDbFn(&IndexFN, DbExtIndex);
	MainIndex = new INDEX(this, IndexFN);
	STRING MDTFN;
	ComposeDbFn(&MDTFN, DbExtMdt);
	STRING FileStem;
	GetDbFileStem(&FileStem);
	MainMdt = new MDT(FileStem, GDT_FALSE);
	STRING DFDTFN;
	ComposeDbFn(&DFDTFN, DbExtDfd);
	MainDfdt = new DFDT();
	DocTypeReg = new DTREG(this);
	// Recycle Main Registry
	delete MainRegistry;
	MainRegistry = new REGISTRY("Isearch");
	DbInfoChanged = GDT_FALSE;
	// Register Isearch Version Number
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("VersionNumber");
	STRING S;
	S = IsearchVersion;
	Value.AddEntry(S);
	MainRegistry->SetData(Position, Value);
	// Register Magic Number
	Position.SetEntry(2, "MagicNumber");
	S = IsearchMagicNumber;
	Value.SetEntry(1, S);
	MainRegistry->SetData(Position, Value);
	// Register Doctype
	Position.SetEntry(2, "DocType");
	Value.SetEntry(1, "");
	MainRegistry->SetData(Position, Value);
	// Register Endianness
	Position.SetEntry(2, "BigEndian");
	S = IsBigEndian();
	Value.SetEntry(1, S);
	MainRegistry->SetData(Position, Value);
	SetWrongEndian();
}

void IDB::SetDocumentInfo(const INT Index, const RECORD& Record) {
	MDTREC Mdtrec;
	MainMdt->GetEntry(Index, &Mdtrec);
	STRING S;
	Record.GetKey(&S);
	Mdtrec.SetKey(S);
	Record.GetFileName(&S);
	Mdtrec.SetFileName(S);
	Record.GetPathName(&S);
	Mdtrec.SetPathName(S);
	Mdtrec.SetLocalRecordStart(Record.GetRecordStart());
	Mdtrec.SetLocalRecordEnd(Record.GetRecordEnd());
	Record.GetDocumentType(&S);
	Mdtrec.SetDocumentType(S);
	// Do we just ignore the DFT???
	MainMdt->SetEntry(Index, Mdtrec);
}

void IDB::GetDocumentInfo(const INT Index, PRECORD RecordBuffer) const {
	MDTREC Mdtrec;
	MainMdt->GetEntry(Index, &Mdtrec);
	RECORD Record;
	STRING S;
	Mdtrec.GetKey(&S);
	Record.SetKey(S);
	Mdtrec.GetFileName(&S);
	Record.SetFileName(S);
	Mdtrec.GetPathName(&S);
	Record.SetPathName(S);
	Record.SetRecordStart(Mdtrec.GetLocalRecordStart());
	Record.SetRecordEnd(Mdtrec.GetLocalRecordEnd());
	Mdtrec.GetDocumentType(&S);
	Record.SetDocumentType(S);
	// Here needs to go a call to a function that builds the DFT.
	*RecordBuffer = Record;
}

GDT_BOOLEAN IDB::GetDocumentDeleted(const INT Index) const {
	MDTREC Mdtrec;
	MainMdt->GetEntry(Index, &Mdtrec);
	return Mdtrec.GetDeleted();
}

INT IDB::DeleteByKey(const STRING& Key) {
	INT x = MainMdt->LookupByKey(Key);
	if (x) {
		MDTREC Mdtrec;
		MainMdt->GetEntry(x, &Mdtrec);
		Mdtrec.SetDeleted(GDT_TRUE);
		MainMdt->SetEntry(x, Mdtrec);
		return 1;
	} else {
		return 0;
	}
}

INT IDB::UndeleteByKey(const STRING& Key) {
	INT x = MainMdt->LookupByKey(Key);
	if (x) {
		MDTREC Mdtrec;
		MainMdt->GetEntry(x, &Mdtrec);
		Mdtrec.SetDeleted(GDT_FALSE);
		MainMdt->SetEntry(x, Mdtrec);
		return 1;
	} else {
		return 0;
	}
}

SIZE_T IDB::CleanupDb() {
	// Compute offset GP changes for each MDTREC
	INT MdtTotalEntries = MainMdt->GetTotalEntries();
	PGPTYPE GpList;
	GpList = new GPTYPE[MdtTotalEntries];
	INT Offset = 0;
	INT x;
	MDTREC Mdtrec;
	for (x=1; x<=MdtTotalEntries; x++) {
		MainMdt->GetEntry(x, &Mdtrec);
		if (Mdtrec.GetDeleted() == GDT_TRUE) {
			Offset += Mdtrec.GetLocalRecordEnd() - Mdtrec.GetLocalRecordStart() + 1;
		} else {
			GpList[x-1] = Offset;
		}
	}
	// Remove deleted GP's from index and field files, also collapsing GP space
	INT FileNum;
	INT DfdtTotalEntries = MainDfdt->GetTotalEntries();
	DFD Dfd;
	STRING S, Fn, TempFn;
	ComposeDbFn(&TempFn, DbExtTemp);
	PFILE Fpo, Fpn;
	for (FileNum=0; FileNum<=DfdtTotalEntries; FileNum++) {
		if (FileNum == 0) {
			ComposeDbFn(&Fn, DbExtIndex);
		} else {
			MainDfdt->GetEntry(FileNum, &Dfd);
			Dfd.GetFieldName(&S);
			DfdtGetFileName(S, &Fn);
		}
		if ( (Fpo = fopen(Fn, "rb")) == NULL) {
			perror(Fn);
			exit(1);
		}
		if ( (Fpn = fopen(TempFn, "wb")) == NULL) {
			perror(TempFn);
			exit(1);
		}
		//should I bail if this doesn't work? -jem.
		GPTYPE Gp;
		while (GpFread(&Gp, 1, sizeof(GPTYPE), Fpo)) {
			x = MainMdt->LookupByGp(Gp);
			MainMdt->GetEntry(x, &Mdtrec);
			if (Mdtrec.GetDeleted() == GDT_FALSE) {
				Gp -= GpList[x-1];
				GpFwrite(&Gp, 1, sizeof(GPTYPE), Fpn);
			}
		}
		fclose(Fpn);
		fclose(Fpo);
		PCHR Temp1, Temp2;
		Temp1 = Fn.NewCString();
		Temp2 = TempFn.NewCString();

#if defined(_MSDOS) || defined(_WIN32)

		/*
		 * MSDOS / WIN32 rename doesn't remove an existing file so
		 * we have to do it ourselves.
		 */

		remove(Temp1);
#endif

		rename(Temp2, Temp1);
		delete [] Temp1;
		delete [] Temp2;

	}
	// Update GP's in MDT
	for (x=1; x<=MdtTotalEntries; x++) {
		MainMdt->GetEntry(x, &Mdtrec);
		if (Mdtrec.GetDeleted() == GDT_FALSE) {
			Mdtrec.SetGlobalFileStart(Mdtrec.GetGlobalFileStart() - GpList[x-1]);
			Mdtrec.SetGlobalFileEnd(Mdtrec.GetGlobalFileEnd() - GpList[x-1]);
			MainMdt->SetEntry(x, Mdtrec);
		}
	}
/*
	// Remove MDTREC's marked as deleted from MainMdt
	INT n = 1;
	for (x=1; x<=MdtTotalEntries; x++) {
		MainMdt->GetEntry(x, &Mdtrec);
		if (Mdtrec.GetDeleted() == GDT_FALSE) {
			if (x != n) {
				Mdtrec.SetGlobalFileStart(Mdtrec.GetGlobalFileStart() - GpList[x-1]);
				Mdtrec.SetGlobalFileEnd(Mdtrec.GetGlobalFileEnd() - GpList[x-1]);
				MainMdt->SetEntry(n, Mdtrec);
			}
			n++;
		}
	}
	INT Count = MdtTotalEntries - n + 1;
	MainMdt->SetTotalEntries(n - 1);
*/
	delete [] GpList;
	return (MainMdt->RemoveDeleted());
}

void IDB::SetGlobalDocType(const STRING& NewGlobalDocType) {
	STRING S;
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("DocType");
	S = NewGlobalDocType;
	S.UpperCase();
	Value.AddEntry(S);
	MainRegistry->SetData(Position, Value);
	DbInfoChanged = GDT_TRUE;
}

void IDB::GetGlobalDocType(PSTRING StringBuffer) const {
	STRLIST Position, Value;
	Position.AddEntry("DbInfo");
	Position.AddEntry("DocType");
	MainRegistry->GetData(Position, &Value);
	Value.GetEntry(1, StringBuffer);
}

IDB::~IDB() {
	if (DebugMode) {
		MainMdt->Dump();
		MainIndex->DumpIndex(DebugSkip);
	}
	if (IsDbCompatible()) {
		STRLIST Position;
		Position.AddEntry("DbInfo");
		if ( (MainMdt->GetChanged()) || (MainDfdt->GetChanged()) ||
				(DbInfoChanged) ) {
			// Register Isearch Version Number
			STRLIST Value;
			Position.AddEntry("VersionNumber");
			STRING S;
			S = IsearchVersion;
			Value.AddEntry(S);
			MainRegistry->SetData(Position, Value);
			// Save DbInfo registry
			Position.Clear();
			Position.AddEntry("DbInfo");
			STRING DbInfoFn;
			ComposeDbFn(&DbInfoFn, DbExtDbInfo);
			MainRegistry->SaveToFile(DbInfoFn, Position);
		}
	}
	if (MainIndex) {
		delete MainIndex;
	}
//	if (IsDbCompatible()) {
//		STRING MDTFN;
//		ComposeDbFn(&MDTFN, DbExtMdt);
//		if (MainMdt->GetChanged()) {
//			if (IsWrongEndian()) {
//				MainMdt->FlipBytes();
//			}
//			MainMdt->SaveTable(MDTFN);
//		}
//	}
	if (MainMdt) {
		delete MainMdt;
	}
	if (IsDbCompatible()) {
		STRING DFDTFN;
		ComposeDbFn(&DFDTFN, DbExtDfd);
		if (MainDfdt->GetChanged()) {
			MainDfdt->SaveTable(DFDTFN);
		}
	}
	if (MainDfdt) {
		delete MainDfdt;
	}
	delete DocTypeReg;
	delete MainRegistry;
}
