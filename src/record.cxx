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
File:		record.cxx
Version:	1.00
Description:	Class RECORD - Database Record
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "record.hxx"
#include "common.hxx"

RECORD::RECORD() {
	Key="";
	PathName="";
	FileName="";
	RecordStart=0;
	RecordEnd=0;
	DocumentType="";
}

RECORD::RECORD(STRING& NewPathName, STRING& NewFileName) {
	PathName = NewPathName;
	AddTrailingSlash(&PathName);
	ExpandFileSpec(&PathName);
	FileName = NewFileName;
	RemovePath(&FileName);
}

RECORD& RECORD::operator=(const RECORD& OtherRecord) {
	Key = OtherRecord.Key;
	PathName = OtherRecord.PathName;
	FileName = OtherRecord.FileName;
	RecordStart = OtherRecord.RecordStart;
	RecordEnd = OtherRecord.RecordEnd;
	Dft = OtherRecord.Dft;
	DocumentType = OtherRecord.DocumentType;
	DocumentType.UpperCase();
	return *this;
}

void RECORD::SetKey(const STRING& NewKey) {
	Key = NewKey;
}

void RECORD::GetKey(PSTRING StringBuffer) const {
	*StringBuffer = Key;
}

void RECORD::SetPathName(const STRING& NewPathName) {
	PathName = NewPathName;
	AddTrailingSlash(&PathName);
	ExpandFileSpec(&PathName);
}

void RECORD::GetPathName(PSTRING StringBuffer) const {
	*StringBuffer = PathName;
}

void RECORD::SetFileName(const STRING& NewFileName) {
	FileName = NewFileName;
	RemovePath(&FileName);
}

void RECORD::GetFileName(PSTRING StringBuffer) const {
	*StringBuffer = FileName;
}

void RECORD::GetFullFileName(PSTRING StringBuffer) const {
	*StringBuffer = PathName;
	StringBuffer->Cat(FileName);
}

void RECORD::SetRecordStart(const GPTYPE NewRecordStart) {
	RecordStart = NewRecordStart;
}

GPTYPE RECORD::GetRecordStart() const {
	return RecordStart;
}

void RECORD::SetRecordEnd(const GPTYPE NewRecordEnd) {
	RecordEnd = NewRecordEnd;
}

GPTYPE RECORD::GetRecordEnd() const {
	return RecordEnd;
}

void RECORD::SetDocumentType(const STRING& NewDocumentType) {
	DocumentType = NewDocumentType;
	DocumentType.UpperCase();
}

void RECORD::GetDocumentType(PSTRING StringBuffer) const {
	*StringBuffer = DocumentType;
}

void RECORD::SetDft(const DFT& NewDft) {
	Dft = NewDft;
}

void RECORD::GetDft(PDFT DftBuffer) const {
	*DftBuffer = Dft;
}

void RECORD::Write(PFILE fp) const {
	Key.Print(fp);
	fprintf(fp, "\n");
	PathName.Print(fp);
	fprintf(fp, "\n");
	FileName.Print(fp);
	fprintf(fp, "\n");
	fprintf(fp, "%d\n", RecordStart);
	fprintf(fp, "%d\n", RecordEnd);
	DocumentType.Print(fp);
	fprintf(fp, "\n");
	Dft.Write(fp);
}

void RECORD::Read(PFILE fp) {
	STRING s;
	Key.FGet(fp, DocumentKeySize);
	PathName.FGet(fp, DocPathNameSize);
	FileName.FGet(fp, DocFileNameSize);
	s.FGet(fp, 16);
	RecordStart = s.GetInt();
	s.FGet(fp, 16);
	RecordEnd = s.GetInt();
	DocumentType.FGet(fp, DocumentTypeSize);
	Dft.Read(fp);
}

RECORD::~RECORD() {
}
