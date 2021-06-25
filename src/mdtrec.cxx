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
File:		mdtrec.cxx
Version:	1.00
Description:	Class MDTREC - Multiple Document Table Record
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <string.h>
#include "mdtrec.hxx"
#include "common.hxx"

MDTREC::MDTREC() {
// Thanks to Edward C. Zimmermann for this: apparently there is
// a bug somewhere that is fixed (or at least minimized) by filling
// these character buffers with zeros.  I'll have to look more into this.....
	memset(Key, '\0', DocumentKeySize);
	memset(DocumentType, '\0', DocumentTypeSize);
	memset(PathName, '\0', DocPathNameSize);
	memset(FileName, '\0', DocFileNameSize);
/*
	*Key = '\0';
	*DocumentType = '\0';
	*PathName = '\0';
	*FileName = '\0';
*/
	GlobalFileStart = 0;
	GlobalFileEnd = 0;
	LocalRecordStart = 0;
	LocalRecordEnd = 0;
	Deleted = 0;
}

MDTREC& MDTREC::operator=(const MDTREC& OtherMdtRec) {
	strcpy(Key, OtherMdtRec.Key);
	strcpy(DocumentType, OtherMdtRec.DocumentType);
	strcpy(PathName, OtherMdtRec.PathName);
	strcpy(FileName, OtherMdtRec.FileName);
	GlobalFileStart = OtherMdtRec.GlobalFileStart;
	GlobalFileEnd = OtherMdtRec.GlobalFileEnd;
	LocalRecordStart = OtherMdtRec.LocalRecordStart;
	LocalRecordEnd = OtherMdtRec.LocalRecordEnd;
	Deleted = OtherMdtRec.Deleted;
	return *this;
}

void MDTREC::FlipBytes() {
	GpSwab(&GlobalFileStart);
	GpSwab(&GlobalFileEnd);
	GpSwab(&LocalRecordStart);
	GpSwab(&LocalRecordEnd);
}

void MDTREC::SetKey(const STRING& NewKey) {
	NewKey.GetCString(Key, DocumentKeySize);
}

void MDTREC::GetKey(STRING* StringBuffer) const {
	*StringBuffer = Key;
}

void MDTREC::SetDocumentType(const STRING& NewDocumentType) {
	NewDocumentType.GetCString(DocumentType, DocumentTypeSize);
}

void MDTREC::GetDocumentType(STRING* StringBuffer) const {
	*StringBuffer = DocumentType;
}

void MDTREC::SetPathName(const STRING& NewPathName) {
	NewPathName.GetCString(PathName, DocPathNameSize);
}

void MDTREC::GetPathName(STRING* StringBuffer) const {
	*StringBuffer = PathName;
}

void MDTREC::SetFileName(const STRING& NewFileName) {
	NewFileName.GetCString(FileName, DocFileNameSize);
}

void MDTREC::GetFileName(STRING* StringBuffer) const {
	*StringBuffer = FileName;
}

void MDTREC::GetFullFileName(STRING* StringBuffer) const {
	*StringBuffer = PathName;
	StringBuffer->Cat(FileName);
}

void MDTREC::SetGlobalFileStart(const GPTYPE NewGlobalFileStart) {
	GlobalFileStart = NewGlobalFileStart;
}

GPTYPE MDTREC::GetGlobalFileStart() const {
	return GlobalFileStart;
}

void MDTREC::SetGlobalFileEnd(const GPTYPE NewGlobalFileEnd) {
	GlobalFileEnd = NewGlobalFileEnd;
}

GPTYPE MDTREC::GetGlobalFileEnd() const {
	return GlobalFileEnd;
}

void MDTREC::SetLocalRecordStart(const GPTYPE NewLocalRecordStart) {
	LocalRecordStart = NewLocalRecordStart;
}

GPTYPE MDTREC::GetLocalRecordStart() const{
	return LocalRecordStart;
}

void MDTREC::SetLocalRecordEnd(const GPTYPE NewLocalRecordEnd) {
	LocalRecordEnd = NewLocalRecordEnd;
}

GPTYPE MDTREC::GetLocalRecordEnd() const {
	return LocalRecordEnd;
}

void MDTREC::SetDeleted(const GDT_BOOLEAN Flag) {
	if (Flag) {
		Deleted = 1;
	} else {
		Deleted = 0;
	}
}

GDT_BOOLEAN MDTREC::GetDeleted() const {
	return (Deleted == 1) ? GDT_TRUE : GDT_FALSE;
}

MDTREC::~MDTREC() {
}
