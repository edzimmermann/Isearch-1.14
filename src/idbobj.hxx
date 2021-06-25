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
File:		idbobj.hxx
Version:	1.00
Description:	Class IDBOBJ: Database object virtual class
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef IDBOBJ_HXX
#define IDBOBJ_HXX

#include "defs.hxx"
#include "string.hxx"
#include "mdt.hxx"
#include "dfdt.hxx"
#include "dfd.hxx"
#include "result.hxx"
#include "strlist.hxx"
#include "record.hxx"

class IDBOBJ {
friend class INDEX;
friend class IRSET;
public:
	IDBOBJ() { };
	virtual ~IDBOBJ() { };
	virtual UINT4 GetIndexingMemory() const { return 0; };
	virtual void DfdtAddEntry(const DFD& NewDfd) = 0;
	virtual void DfdtGetEntry(const INT Index, PDFD DfdRecord) const { };
	virtual INT DfdtGetTotalEntries() const { return 0; };
	virtual void DfdtGetFileName(const STRING& FieldName, 
				     PSTRING StringBuffer) const { };
	virtual GDT_BOOLEAN GetFieldData(const RESULT& ResultRecord, 
				  const STRING& FieldName,
				  PSTRING StringBuffer) 
	  const { return GDT_FALSE; };
	virtual GDT_BOOLEAN GetFieldData(const RESULT& ResultRecord, 
				  const STRING& FieldName,
				  STRLIST* StrlistBuffer) 
	  const { return GDT_FALSE; };
	virtual void GetDocTypeOptions(PSTRLIST StringListBuffer) const { };
//	virtual void GetRecordData(const RESULT& ResultRecord, 
//	                           PSTRING StringBuffer) const { };
	virtual void ComposeDbFn(PSTRING StringBuffer, const PCHR Suffix) 
	  const { };
	virtual void DocTypeAddRecord(const RECORD& NewRecord) { };
	virtual PFILE ffopen(const STRING& FileName, const PCHR Type) 
	  { return 0; };
	virtual INT ffclose(PFILE FilePointer) { return 0; };
	virtual INT GpFwrite(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, 
			     FILE* Stream) const {
		cout << "Bad call to IDBOBJ::GpFwrite()" << endl;
		return 0;
	};
	virtual INT GpFread(GPTYPE* Ptr, SIZE_T Size, SIZE_T NumElements, 
			    FILE* Stream) const {
		cout << "Bad call to IDBOBJ::GpFread()" << endl;
		return 0;
	};
	void GetDbFileStem(PSTRING StringBuffer) const { };
	virtual INT IsStopWord(CHR* WordStart, INT WordMaximum) const = 0;
	STRLIST FieldTypes;

protected:
	virtual void IndexingStatus(const INT StatusMessage,
				    const PSTRING FileName, 
				    const INT WordCount) const { };
private:
	virtual PMDT GetMainMdt() { return 0; };
	virtual PDFDT GetMainDfdt() { return 0; };
	virtual void ParseFields(PRECORD Record) = 0;
	//@ManMemo: This method called during indexing to build GP list for document
	virtual GPTYPE ParseWords(const STRING& Doctype, CHR* DataBuffer, 
				  INT DataLength, INT DataOffset, 
				  GPTYPE* GpBuffer, INT GpLength) = 0;
//	virtual void SelectRegions(const RECORD& Record, FCT* RegionsPtr) 
//	                           const { };
};

typedef IDBOBJ* PIDBOBJ;

#endif

