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
File:		doctype.hxx
Version:	1.00
Description:	Class DOCTYPE - Document Type
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DOCTYPE_HXX
#define DOCTYPE_HXX

#include "defs.hxx"
#include "idbobj.hxx"
#include "result.hxx"
#include "squery.hxx"
#include "rset.hxx"

class DOCTYPE {
public:
	DOCTYPE(IDBOBJ* DbParent);
	virtual void BeforeIndexing();
	virtual void LoadFieldTable();
	virtual void AddFieldDefs();
	virtual void ParseRecords(const RECORD& FileRecord);
	//@ManMemo: This method called during indexing to build GP list for document.
	virtual GPTYPE ParseWords(CHR* DataBuffer, INT DataLength, INT DataOffset, GPTYPE* GpBuffer, INT GpLength);
//	virtual void SelectRegions(const RECORD& Document, FCT* FctPtr);
	virtual void ParseFields(RECORD* NewRecordPtr);
	virtual DOUBLE ParseDateSingle(const PCHR Buffer);
	virtual void ParseDateRange(const PCHR Buffer, DOUBLE* fStart, 
				    DOUBLE* fEnd);
	virtual DOUBLE ParseNumeric(const PCHR Buffer);
	virtual void ParseGPoly(const PCHR Buffer);
	virtual void AfterIndexing();
	virtual void BeforeSearching(SQUERY* SearchQueryPtr);
//	virtual void AfterSearching(RSET* ResultSetPtr);
	virtual PRSET AfterSearching(RSET* ResultSetPtr);
	virtual void BeforeRset(const STRING& RecordSyntax);
	virtual void AfterRset(const STRING& RecordSyntax);
	virtual void Present(const RESULT& ResultRecord, const STRING& ElementSet,
			STRING* StringBufferPtr);
	virtual void Present(const RESULT& ResultRecord, const STRING& ElementSet,
			const STRING& RecordSyntax, STRING* StringBufferPtr);
	virtual ~DOCTYPE();
protected:
	IDBOBJ* Db;
};

typedef DOCTYPE* PDOCTYPE;

#endif
