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
File:		index.hxx
Version:	1.00
Description:	Class INDEX
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef INDEX_HXX
#define INDEX_HXX

#include "defs.hxx"
#include "idbobj.hxx"
#include "string.hxx"
#include "mdt.hxx"
#include "squery.hxx"
#include "rset.hxx"
#include "irset.hxx"
#ifdef DICTIONARY
#include "dictionary.hxx"
#endif

class INDEX /*: public IDBOBJ*/ {
#ifdef DICTIONARY
friend class DICTIONARY;
#endif
public:
	INDEX(const PIDBOBJ DbParent, const STRING& NewFileName);
//	int MemIndexCompare(PGPTYPE x, PGPTYPE y);
//	void AddRecord(const RECORD& NewRecord);
	void WriteFieldData(const RECORD& Record, const GPTYPE GpOffset);
	void CreateDictionary(void);
	void CreateCentroid(void);
	void AddRecordList(PFILE RecordListFp);
	GDT_BOOLEAN ValidateInField(const GPTYPE HitGp, 
				    const STRING& FieldName, 
				    const INT PhraseLength = 0);
	PIRSET RsetOr(const OPOBJ& Set1, const OPOBJ& Set2) const;
	PRSET Search(const SQUERY& SearchQuery);
	PRSET AndSearch(const SQUERY& SearchQuery);
//	PRSET OrSearch(const SQUERY& SearchQuery);
	PIRSET SoundexSearch(const STRING& SearchTerm, 
			     const STRING& FieldName);
	PIRSET TermSearch(const STRING& SearchTerm, const STRING& FieldName);
	INT Match(const CHR *QueryTerm, const INT TermLength, 
		  const GPTYPE gp, const INT4 Offset=0);
	void DumpIndex(INT DebugSkip);
	INT IsStopWord(CHR* WordStart, INT WordMaximum) const;
//	void CleanupIndex();
	~INDEX();
private:
	GDT_BOOLEAN GetIndirectBuffer(const GPTYPE Gp, PCHR Buffer, 
				      const INT4 Offset=0);
	//@ManMemo: Calls IDB::ParseWords()
	GPTYPE BuildGpList(const STRING& Doctype, INT StartingPosition,
			   PCHR MemoryData, INT MemoryDataLength, 
			   PGPTYPE MemoryIndex, INT MemoryIndexLength);
	void MergeIndex(PCHR MemoryData, INT MemoryDataLength,
			PGPTYPE MemoryIndex, INT MemoryIndexLength,
			GPTYPE GlobalStart);
	PFILE GetFilePointer(const GPTYPE gp) const;
	STRING IndexFileName;
	PIDBOBJ Parent;
//	UINT4 DataMemorySize, IndexMemorySize;
//	PGPTYPE MemoryIndex;
//	PMDT MemoryMdt;
#ifdef DICTIONARY
	DICTIONARY *Dict;
#endif
};

typedef INDEX* PINDEX;

#endif
