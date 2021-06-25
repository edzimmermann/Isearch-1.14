/*@@@
File:		attr.cxx
Version:	1.01
Description:	Class ATTR - Attribute
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "string.hxx"
#include "attr.hxx"

ATTR::ATTR() {
  SetId = "";
  AttrType = 0;
  AttrValue = "";
}

ATTR& ATTR::operator=(const ATTR& OtherAttr) {
	SetId = OtherAttr.SetId;
	AttrType = OtherAttr.AttrType;
	AttrValue = OtherAttr.AttrValue;
	return *this;
}


void ATTR::SetSetId(const STRING& NewSetId) {
	SetId = NewSetId;
}

void ATTR::GetSetId(PSTRING StringBuffer) const {
	*StringBuffer = SetId;
}

void ATTR::SetAttrType(const INT NewAttrType) {
	AttrType = NewAttrType;
}

INT ATTR::GetAttrType() const {
	return AttrType;
}

void ATTR::SetAttrValue(const STRING& NewAttrValue) {
	AttrValue = NewAttrValue;
}

void ATTR::GetAttrValue(PSTRING StringBuffer) const {
	*StringBuffer = AttrValue;
}

void ATTR::SetAttrValue(const INT NewAttrValue) {
	AttrValue = NewAttrValue;
}

INT ATTR::GetAttrValue() const {
	return AttrValue.GetInt();
}

ATTR::~ATTR() {
}
