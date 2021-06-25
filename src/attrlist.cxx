/*@@@
File:		attrlist.cxx
Version:	1.00
Description:	Class ATTRLIST - Attribute List
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "attrlist.hxx"

ATTRLIST::ATTRLIST() {
	Init();
}

void ATTRLIST::Init() {
	Table = new ATTR[5];
	TotalEntries = 0;
	MaxEntries = 5;
}

ATTRLIST& ATTRLIST::operator=(const ATTRLIST& OtherAttrlist) {
	if (Table) {
		delete [] Table;
	}
	Init();
	INT y = OtherAttrlist.GetTotalEntries();
	INT x;
	ATTR attr;
	for (x=1; x<=y; x++) {
		OtherAttrlist.GetEntry(x, &attr);
		AddEntry(attr);
	}
	return *this;
}

void ATTRLIST::AddEntry(const ATTR& AttrRecord) {
	if (TotalEntries == MaxEntries)
		Expand();
	Table[TotalEntries] = AttrRecord;
	TotalEntries = TotalEntries + 1;
}

void ATTRLIST::GetEntry(const INT Index, PATTR AttrRecord) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*AttrRecord = Table[Index-1];
	}
}

void ATTRLIST::SetEntry(const INT Index, const ATTR& AttrRecord) {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		Table[Index-1] = AttrRecord;
	}
}

void ATTRLIST::DeleteEntry(const INT Index) {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		INT x;
		for (x=(Index-1); x<(TotalEntries-1); x++) {
			Table[x] = Table[x+1];
		}
		TotalEntries--;
	}
}

void ATTRLIST::Expand() {
	Resize(TotalEntries+10);
}

void ATTRLIST::CleanUp() {
	Resize(TotalEntries);
}

void ATTRLIST::Resize(const INT Entries) {
	PATTR Temp = new ATTR[Entries];
	INT RecsToCopy;
	INT x;
	if (Entries >= TotalEntries) {
		RecsToCopy = TotalEntries;
	} else {
		RecsToCopy = Entries;
		TotalEntries = Entries;
	}
	for (x=0; x<RecsToCopy; x++) {
		Temp[x] = Table[x];
	}
	if (Table)
		delete [] Table;
	Table = Temp;
	MaxEntries = Entries;
}

INT ATTRLIST::GetTotalEntries() const {
	return TotalEntries;
}

INT ATTRLIST::Lookup(const STRING& SetId, const INT AttrType) const {
	INT x;
	STRING S;
	for (x=0; x<TotalEntries; x++) {
		Table[x].GetSetId(&S);
		if ( (SetId == S) && (AttrType == Table[x].GetAttrType()) ) {
			return (x+1);
		}
	}
	return 0;
}

INT ATTRLIST::Lookup(const STRING& SetId, const INT AttrType, const INT AttrValue) const {
	INT x;
	STRING S;
	for (x=0; x<TotalEntries; x++) {
		Table[x].GetSetId(&S);
		if ( (SetId == S) && (AttrType == Table[x].GetAttrType()) &&
				(AttrValue == Table[x].GetAttrValue()) ) {
			return (x+1);
		}
	}
	return 0;
}

void ATTRLIST::SetValue(const STRING& SetId, const INT AttrType, const STRING& AttrValue) {
	ATTR Attr;
	INT y = Lookup(SetId, AttrType);
	if (y) {
		GetEntry(y, &Attr);
		Attr.SetAttrValue(AttrValue);
		SetEntry(y, Attr);
	} else {
		Attr.SetSetId(SetId);
		Attr.SetAttrType(AttrType);
		Attr.SetAttrValue(AttrValue);
		AddEntry(Attr);
	}
}

void ATTRLIST::SetValue(const STRING& SetId, const INT AttrType, const INT AttrValue) {
	STRING S;
	S = AttrValue;
	SetValue(SetId, AttrType, S);
}

void ATTRLIST::ClearAttr(const STRING& SetId, const INT AttrType, const INT AttrValue) {
	INT y = Lookup(SetId, AttrType, AttrValue);
	DeleteEntry(y);
}

GDT_BOOLEAN ATTRLIST::GetValue(const STRING& SetId, const INT AttrType, PSTRING StringBuffer) const {
	ATTR Attr;
	INT y = Lookup(SetId, AttrType);

	if (y) {

		GetEntry(y, &Attr);
		Attr.GetAttrValue(StringBuffer);
		
		return GDT_TRUE;
	} else {
		*StringBuffer = "";
		return GDT_FALSE;
	}
}


GDT_BOOLEAN ATTRLIST::GetValue(const STRING& SetId, const INT AttrType, PINT IntBuffer) const {
	GDT_BOOLEAN b;
	STRING S;
	b = GetValue(SetId, AttrType, &S);
	*IntBuffer = S.GetInt();
	return b;
}

void ATTRLIST::AttrSetFieldName(const STRING& FieldName) {
	STRING Field = FieldName;
	Field.UpperCase();
	SetValue(IsearchAttributeSet, IsearchFieldAttr, Field);
}

GDT_BOOLEAN ATTRLIST::AttrGetFieldName(PSTRING StringBuffer) const {
	STRING t;
	t=IsearchAttributeSet;
	return GetValue(t, IsearchFieldAttr, StringBuffer);
}

void ATTRLIST::AttrSetRightTruncation(const GDT_BOOLEAN RightTruncation) {
	if (RightTruncation) {
		SetValue(Bib1AttributeSet, 5, 1);
	} else {
		ClearAttr(Bib1AttributeSet, 5, 1);
	}
}

GDT_BOOLEAN ATTRLIST::AttrGetRightTruncation() const {
	INT x = Lookup(Bib1AttributeSet, 5, 1);
	if (x) {
		return GDT_TRUE;
	} else {
		return GDT_FALSE;
	}
}

void ATTRLIST::AttrSetTermWeight(const STRING& TermWeight) {
	SetValue(IsearchAttributeSet, IsearchWeightAttr, TermWeight);
}

GDT_BOOLEAN ATTRLIST::AttrGetTermWeight(PSTRING StringBuffer) const {
	return GetValue(IsearchAttributeSet, IsearchWeightAttr, StringBuffer);
}

ATTRLIST::~ATTRLIST() {
	if (Table)
		delete [] Table;
}
