/*@@@
File:		df.cxx
Version:	1.00
Description:	Class DF - Data Field
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "df.hxx"

DF::DF() {
}

DF& DF::operator=(const DF& OtherDf) {
	FieldName = OtherDf.FieldName;
	Fct = OtherDf.Fct;
	return *this;
}

void DF::SetFieldName(const STRING& NewFieldName) {
	FieldName = NewFieldName;
	FieldName.UpperCase();
}

void DF::GetFieldName(PSTRING StringBuffer) const {
	*StringBuffer = FieldName;
}

void DF::SetFct(const FCT& NewFct) {
	Fct = NewFct;
}

void DF::GetFct(PFCT FctBuffer) const {
	*FctBuffer = Fct;
}

void DF::Write(PFILE fp) const {
	FieldName.Print(fp);
	fprintf(fp, "\n");
	Fct.Write(fp);
}

void DF::Read(PFILE fp) {
	FieldName.FGet(fp, 256);
	Fct.Read(fp);
}

DF::~DF() {
}
