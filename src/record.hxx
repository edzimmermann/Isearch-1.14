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
File:		record.hxx
Version:	1.00
Description:	Class RECORD - Database Record
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef RECORD_HXX
#define RECORD_HXX

#include "defs.hxx"
#include "string.hxx"
#include "dft.hxx"

class RECORD {
public:
	RECORD();
	RECORD(STRING& NewPathName, STRING& NewFileName);
	RECORD& operator=(const RECORD& OtherRecord);
	void SetKey(const STRING& NewKey);
	void GetKey(PSTRING StringBuffer) const;
	void SetPathName(const STRING& NewPathName);
	void GetPathName(PSTRING StringBuffer) const;
	void SetFileName(const STRING& NewFileName);
	void GetFileName(PSTRING StringBuffer) const;
	void GetFullFileName(PSTRING StringBuffer) const;
	void SetRecordStart(const GPTYPE NewRecordStart);
	GPTYPE GetRecordStart() const;
	void SetRecordEnd(const GPTYPE NewRecordEnd);
	GPTYPE GetRecordEnd() const;
	void SetDocumentType(const STRING& NewDocumentType);
	void GetDocumentType(PSTRING StringBuffer) const;
	void SetDft(const DFT& NewDft);
	void GetDft(PDFT DftBuffer) const;
	void Write(PFILE fp) const;
	void Read(PFILE fp);
	~RECORD();
private:
	STRING Key;
	STRING PathName, FileName;
	GPTYPE RecordStart;
	GPTYPE RecordEnd;
	STRING DocumentType;
	DFT Dft;
};

typedef RECORD* PRECORD;

#endif
