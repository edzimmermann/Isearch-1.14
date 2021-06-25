/*@@@
File:		fpt.cxx
Version:	1.00
Description:	Class FPT - File Pointer Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <limits.h>
#include "fpt.hxx"
#include "common.hxx"

FPT::FPT() {
#ifdef _POSIX_OPEN_MAX
	Init(_POSIX_OPEN_MAX - 5);
#else
	Init(_NFILE - 5);
#endif
//	Init(2);
}

FPT::FPT(const INT TableSize) {
	Init(TableSize);
}

void FPT::Init(const INT TableSize) {
	Table = new FPREC[TableSize];
	MaximumEntries = TableSize;
	TotalEntries = 0;
}

INT FPT::Lookup(const STRING& FileName) {
	// Expand FileName
	STRING Fn;
	Fn = FileName;
	ExpandFileSpec(&Fn);
	// Loop through table
	INT x;
	STRING Tfn;
	for (x=0; x<TotalEntries; x++) {
		Table[x].GetFileName(&Tfn);
		if (Tfn == Fn) {
			return (x + 1);
		}
	}
	return 0;
}

INT FPT::Lookup(const PFILE FilePointer) {
	// Loop through table
	INT x;
	for (x=0; x<TotalEntries; x++) {
		if (FilePointer == Table[x].GetFilePointer()) {
			return (x + 1);
		}
	}
	return 0;
}

void FPT::HighPriority(const INT Index) {
	INT x, y;
	INT IndexPriority;
	IndexPriority = Table[Index-1].GetPriority();
	for (x=0; x<TotalEntries; x++) {
		if ((y=Table[x].GetPriority()) <= IndexPriority) {
			Table[x].SetPriority(y + 1);
		}
	}
	Table[Index-1].SetPriority(0);
}

void FPT::LowPriority(const INT Index) {
	INT x, y;
	INT Highest, IndexPriority;
	IndexPriority = Table[Index-1].GetPriority();
	Highest = IndexPriority;
	for (x=0; x<TotalEntries; x++) {
		if ((y=Table[x].GetPriority()) > IndexPriority) {
			Table[x].SetPriority(y - 1);
			if (y > Highest) {
				Highest = y;
			}
		}
	}
	Table[Index-1].SetPriority(Highest);
}

PFILE FPT::ffopen(const STRING& FileName, const PCHR Type) {
	// Check if file is already open
	INT z;
	z = Lookup(FileName);
	if (z) {
		// If found, check OpenMode
		FPREC Fprec;
		STRING Fn, Om;
		PFILE Fp;
		GDT_BOOLEAN Closed;
		Fprec = Table[z-1];
		Fp = Fprec.GetFilePointer();
		Fprec.GetFileName(&Fn);
		Fprec.GetOpenMode(&Om);
		Closed = Fprec.GetClosed();
		if (Om == Type) {
			// If same OpenMode, use the cached information
			if (Om.SearchReverse("w")) {
				fclose(Fp);
				Table[z-1].SetClosed(GDT_FALSE);
				HighPriority(z);
				Table[z-1].SetFilePointer(Fp=fopen(FileName, Type));
				return Fp;
			}
			if (Om.SearchReverse("a")) {
				fclose(Fp);
				Table[z-1].SetClosed(GDT_FALSE);
				HighPriority(z);
				Table[z-1].SetFilePointer(Fp=fopen(FileName, Type));
				return Fp;
			}
			if (Om.SearchReverse("r")) {
				Table[z-1].SetClosed(GDT_FALSE);
				fseek(Fp, 0, 0);
				HighPriority(z);
				return Fp;
			}
		} else {
			// If different OpenMode, update table
			fclose(Fp);
			Table[z-1].SetClosed(GDT_FALSE);
			HighPriority(z);
			Table[z-1].SetOpenMode(Type);
			Table[z-1].SetFilePointer(Fp=fopen(FileName, Type));
			return Fp;
		}
	} else {
		// If not found, open and cache the FilePointer
		INT NewEntry;
		if (TotalEntries >= MaximumEntries) {
			// If table at maximum size, purge oldest element
			NewEntry = 0;
			INT x;
			for (x=0; x<TotalEntries; x++) {
				if (Table[x].GetPriority() > Table[NewEntry].GetPriority()) {
					NewEntry = x;
				}
			}
			if (Table[NewEntry].GetClosed()) {
				fclose(Table[NewEntry].GetFilePointer());
			}
		} else {
			// Otherwise, add an element
			TotalEntries++;
			NewEntry = TotalEntries - 1;
		}
		Table[NewEntry].SetFileName(FileName);
		Table[NewEntry].SetOpenMode(Type);
		Table[NewEntry].SetPriority(MaximumEntries+1);
		HighPriority(NewEntry+1);
		PFILE Fp;
		Table[NewEntry].SetClosed(GDT_FALSE);
		Table[NewEntry].SetFilePointer(Fp=fopen(FileName, Type));
		return Fp;
	}
	return 0;
}

INT FPT::ffclose(PFILE FilePointer) {
	INT z;
	z = Lookup(FilePointer);
	if (z) {
		Table[z-1].SetClosed(GDT_TRUE);
		LowPriority(z);
	} else {
		fclose(FilePointer);
	}
	return 0;
}

void FPT::CloseAll() {
	INT x;
	for (x=0; x<TotalEntries; x++) {
		if (Table[x].GetClosed() == GDT_TRUE) {
			fclose(Table[x].GetFilePointer());
		}
	}
	TotalEntries = 0;
}

FPT::~FPT() {
	CloseAll();
	delete [] Table;
}
