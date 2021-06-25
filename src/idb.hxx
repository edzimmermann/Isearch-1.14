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
File:		idb.hxx
Version:	1.00
Description:	Class IDB
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef IDB_HXX
#define IDB_HXX

#include "defs.hxx"
#include "idbobj.hxx"
#include "string.hxx"
#include "record.hxx"
#include "index.hxx"
#include "mdt.hxx"
#include "squery.hxx"
#include "dfdt.hxx"
#include "dtreg.hxx"
#include "fpt.hxx"
#include "registry.hxx"
#ifdef DICTIONARY
#include "dictionary.hxx"
#endif

class IDB : public IDBOBJ {
friend class INDEX;
friend class IRSET;
friend class DOCTYPE;
#ifdef DICTIONARY
friend class DICTIONARY;
#endif
public:
	IDB(const STRING& NewPathName, const STRING& NewFileName);
	IDB(const STRING& NewPathName, const STRING& NewFileName, const STRLIST& NewDocTypeOptions);
	void Initialize(const STRING& NewPathName, const STRING& NewFileName,
		const STRLIST& NewDocTypeOptions);
	GDT_BOOLEAN IsDbCompatible() const;
	void GetAllDocTypes(PSTRLIST StringListBuffer) const;
	void GetDocTypeOptions(PSTRLIST StringListBuffer) const;
	void KeyLookup(const STRING& Key, PRESULT ResultBuffer) const;
	void GetDfdt(PDFDT DfdtBuffer) const;
	void GetRecordDfdt(const STRING& Key, PDFDT DfdtBuffer) const;
//	void SetTitle(const STRING& NewTitle);
//	void GetTitle(PSTRING StringBuffer);
//	void SetComments(const STRING& NewComments);
//	void GetComments(PSTRING StringBuffer);
	void DfdtAddEntry(const DFD& NewDfd);
	void DfdtGetEntry(const INT Index, PDFD DfdRecord) const;
#ifdef DICTIONARY
	void CreateDictionary(void);
	void CreateCentroid(void);
#endif
	INT DfdtGetTotalEntries() const;
	PDOCTYPE GetDocTypePtr(const STRING& DocType) const;
	GDT_BOOLEAN ValidateDocType(const STRING& DocType) const;
//	void AddRecordList(const RECLIST& NewRecordList);
	INT GetTotalRecords() const;
	void SetIndexingMemory(const UINT4 MemorySize);
	UINT4 GetIndexingMemory() const;
	PRSET AndSearch(const SQUERY& SearchQuery); // temporary!
	PRSET Search(const SQUERY& SearchQuery);
	void DfdtGetFileName(const STRING& FieldName, 
			     PSTRING StringBuffer) const;
//	void GetRecordData(const RESULT& ResultRecord, PSTRING StringBuffer) const;
	GDT_BOOLEAN GetFieldData(const RESULT& ResultRecord, 
				 const STRING& FieldName,
				 STRING* StringBuffer) const;
	GDT_BOOLEAN GetFieldData(const RESULT& ResultRecord, 
				 const STRING& FieldName,
				 STRLIST* StrlistBuffer) const;
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		     const STRING& RecordSyntax, 
		     PSTRING StringBuffer) const;
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
			PSTRING StringBuffer) const;
//	void GenerateKeys();
	void DebugModeOn();
	void DebugModeOff();
	void SetDebugSkip(const INT Skip);
	INT IsSystemFile(const STRING& FileName);
	void KillAll();
	void ComposeDbFn(PSTRING StringBuffer, const PCHR Suffix) const;
	void GetDbFileStem(PSTRING StringBuffer) const;
	void GetDbVersionNumber(PSTRING StringBuffer) const;
	void GetIsearchVersionNumber(PSTRING StringBuffer) const;
	void AddRecord(const RECORD& NewRecord);
	void DocTypeAddRecord(const RECORD& NewRecord);
	void Index();
	void ParseFields(PRECORD Record);
	//@ManMemo: This method called during indexing to build GP list for document.
	GPTYPE ParseWords(const STRING& Doctype, CHR* DataBuffer, INT DataLength, INT DataOffset,
			  GPTYPE* GpBuffer, INT GpLength);
	INT IsStopWord(CHR* WordStart, INT WordMaximum) const;
//	void SelectRegions(const RECORD& Record, FCT* RegionsPtr) const;
	PFILE ffopen(const STRING& FileName, const PCHR Type);
	INT ffclose(PFILE FilePointer);
	GDT_BOOLEAN IsWrongEndian() const;
	void SetWrongEndian();
	void SetDocumentInfo(const INT Index, const RECORD& Record);
	void GetDocumentInfo(const INT Index, PRECORD RecordBuffer) const;
	SIZE_T GpFwrite(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, FILE* Stream) const;
	SIZE_T GpFread(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, FILE* Stream) const;
	GDT_BOOLEAN GetDocumentDeleted(const INT Index) const;
	INT DeleteByKey(const STRING& Key);
	INT UndeleteByKey(const STRING& Key);
	SIZE_T CleanupDb();
	void SetGlobalDocType(const STRING& NewGlobalDocType);
	void GetGlobalDocType(PSTRING StringBuffer) const;
	void BeginRsetPresent(const STRING& RecordSyntax);
	void EndRsetPresent(const STRING& RecordSyntax);
	~IDB();
protected:
	void IndexingStatus(const INT StatusMessage, const PSTRING FileName,
			const INT Count) const;
private:
	PMDT GetMainMdt();
	PDFDT GetMainDfdt();
	STRING DbPathName, DbFileName;
	STRING Title, Comments;
	PINDEX MainIndex;
	PMDT MainMdt;
	PDFDT MainDfdt;
	UINT4 IndexingMemory;
	INT DebugMode, DebugSkip;
	PDTREG DocTypeReg;
	FPT MainFpt;
	STRLIST DocTypeOptions;
	INT TotalRecordsQueued;
	PREGISTRY MainRegistry;
	GDT_BOOLEAN DbInfoChanged;
	GDT_BOOLEAN WrongEndian;
};

typedef IDB* PIDB;

#endif
