/*@@@
File:		fc.cxx
Version:	1.00
Description:	Class FC - Field Coordinates
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "fc.hxx"
#include "string.hxx"
#include "common.hxx"

FC::FC() {
}

void FC::SetFieldStart(const GPTYPE NewFieldStart) {
	FieldStart = NewFieldStart;
}

GPTYPE FC::GetFieldStart() {
	return FieldStart;
}

void FC::SetFieldEnd(const GPTYPE NewFieldEnd) {
	FieldEnd = NewFieldEnd;
}

GPTYPE FC::GetFieldEnd() {
	return FieldEnd;
}

void FC::Write(PFILE fp) const {
	fprintf(fp, "%d\n%d\n", FieldStart, FieldEnd);
}

void FC::Read(PFILE fp) {
	STRING s;
	s.FGet(fp, 16);
	FieldStart = s.GetInt();
	s.FGet(fp, 16);
	FieldEnd = s.GetInt();
}

void FC::FlipBytes() {
	GpSwab(&FieldStart);
	GpSwab(&FieldEnd);
}

ostream& operator<<(ostream& Os, const FC& Fc) {
	Os << Fc.FieldStart << ' ' << Fc.FieldEnd << endl;
	return Os;
}

FC::~FC() {
}
