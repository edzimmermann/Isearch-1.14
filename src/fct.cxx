/*@@@
File:		fct.cxx
Version:	1.00
Description:	Class FCT - Field Coordinate Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include "fct.hxx"
#include "string.hxx"

//#include <iostream.h>

FCT::FCT() : VLIST() {
}


FCT& FCT::operator=(const FCT& OtherFct) {
	Clear();
	SIZE_T x;
	SIZE_T y = OtherFct.GetTotalEntries();
	FCT* NodePtr;
	FCT* NewNodePtr;
	for (x=1; x<=y; x++) {
		NodePtr = (FCT*)(OtherFct.GetNodePtr(x));
		NewNodePtr = new FCT();
		NewNodePtr->Fc = NodePtr->Fc;
		VLIST::AddNode(NewNodePtr);
	}
	return *this;
}

void FCT::AddEntry(const FC& FcRecord) {
	FCT* NodePtr = new FCT();
	NodePtr->Fc = FcRecord;
	VLIST::AddNode(NodePtr);
}

void FCT::GetEntry(const INT Index, FC* FcRecord) const {
	FCT* NodePtr = (FCT*)(VLIST::GetNodePtr(Index));
	if (NodePtr) {
		*FcRecord = NodePtr->Fc;
	}
}

int FctFcCompare(const void* x, const void* y) {
	return ( ((FC*)x)->GetFieldStart() - ((FC*)y)->GetFieldStart() );
}

void FCT::SortByFc() {
	SIZE_T TotalEntries = GetTotalEntries();
	FC* TablePtr = new FC[TotalEntries];
	SIZE_T x = 0;
	FCT* p = (FCT*)(this->GetNextNodePtr());
	while (p != this) {
		TablePtr[x++] = p->Fc;
		p = (FCT*)(p->GetNextNodePtr());
	}
	qsort(TablePtr, TotalEntries, sizeof(FC), FctFcCompare);
	p = (FCT*)(p->GetNextNodePtr());
	x = 0;
	while (p != this) {
		p->Fc = TablePtr[x++];
		p = (FCT*)(p->GetNextNodePtr());
	}
	delete [] TablePtr;
}

void FCT::Write(PFILE fp) const {
	SIZE_T TotalEntries = GetTotalEntries();
	fprintf(fp, "%d\n", TotalEntries);
	SIZE_T x;
	for (x=1; x<=TotalEntries; x++) {
		((FCT*)(VLIST::GetNodePtr(x)))->Fc.Write(fp);
	}
}

void FCT::Read(PFILE fp) {
	Clear();
	STRING s;
	FC Fc;
	s.FGet(fp, 16);
	INT n, x;
	n = s.GetInt();
	for (x=0; x<n; x++) {
		Fc.Read(fp);
		AddEntry(Fc);
	}
}

void FCT::Print(ostream& Os) const {
	FCT* p = (FCT*)(this->GetNextNodePtr());
	while (p != this) {
		Os << p->Fc;
		p = (FCT*)(p->GetNextNodePtr());
	}
}

void FCT::SubtractOffset(const GPTYPE GpOffset) {
	FCT* p = (FCT*)(this->GetNextNodePtr());
	while (p != this) {
		p->Fc.SetFieldStart(p->Fc.GetFieldStart() - GpOffset);
		p->Fc.SetFieldEnd(p->Fc.GetFieldEnd() - GpOffset);
		p = (FCT*)(p->GetNextNodePtr());
	}
}

ostream& operator<<(ostream& Os, const FCT& Fct) {
	Fct.Print(Os);
	return Os;
}
