/*@@@
File:		dfd.cxx
Version:	1.00
Description:	Class DFD - Data Field Definition
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "dfd.hxx"

DFD::DFD() {
	FileNumber = 0;
}

DFD& DFD::operator=(const DFD& OtherDfd) {
//	FieldName = OtherDfd.FieldName;
	FileNumber = OtherDfd.FileNumber;
	Attributes = OtherDfd.Attributes;
	return *this;
}

void DFD::SetFieldName(const STRING& NewFieldName) {
//	FieldName = NewFieldName;
//	FieldName.UpperCase();
	Attributes.AttrSetFieldName(NewFieldName);
}

void DFD::GetFieldName(PSTRING StringBuffer) const {
//	*StringBuffer = FieldName;
	Attributes.AttrGetFieldName(StringBuffer);
}

void DFD::SetFileNumber(const INT NewFileNumber) {
	FileNumber = NewFileNumber;
}

INT DFD::GetFileNumber() const {
	return FileNumber;
}

void DFD::SetAttributes(const ATTRLIST& NewAttributes) {
	Attributes = NewAttributes;
}

void DFD::GetAttributes(PATTRLIST AttributesBuffer) const {
	*AttributesBuffer = Attributes;
}

DFD::~DFD() {
}
