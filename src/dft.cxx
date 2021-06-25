/*@@@
File:		dft.cxx
Version:	1.00
Description:	Class DFT - Data Field Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "dft.hxx"

DFT::DFT() {
	Init();
}

void DFT::Init() {
	Table = new DF[1];
	TotalEntries = 0;
	MaxEntries = 1;
}

DFT& DFT::operator=(const DFT& OtherDft) {
	if (Table) {
		delete [] Table;
	}
	Init();
	INT y = OtherDft.GetTotalEntries();
	INT x;
	DF df;
	for (x=1; x<=y; x++) {
		OtherDft.GetEntry(x, &df);
		AddEntry(df);
	}
	return *this;
}

void DFT::AddEntry(const DF& DfRecord) {
	if (TotalEntries == MaxEntries) {
		Expand();
	}
	Table[TotalEntries] = DfRecord;
	TotalEntries++;
}

void DFT::GetEntry(const INT Index, PDF DfRecord) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*DfRecord = Table[Index-1];
	}
}

void DFT::Expand() {
	Resize(TotalEntries+10);
}

void DFT::CleanUp() {
	Resize(TotalEntries);
}

void DFT::Resize(const INT Entries) {
	PDF Temp = new DF[Entries];
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

INT DFT::GetTotalEntries() const {
	return TotalEntries;
}

void DFT::Write(PFILE fp) const {
	fprintf(fp, "%d\n", TotalEntries);
	INT x;
	for (x=0; x<TotalEntries; x++) {
		Table[x].Write(fp);
	}
}

void DFT::Read(PFILE fp) {
	STRING s;
	INT n, x;
	DF Df;
	DFT Dft;
	s.FGet(fp, 16);
	n = s.GetInt();
	for (x=0; x<n; x++) {
		Df.Read(fp);
		Dft.AddEntry(Df);
	}
	*this = Dft;
}

DFT::~DFT() {
	if (Table)
		delete [] Table;
}
