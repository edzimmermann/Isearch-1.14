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
File:		mdt.cxx
Version:	1.00
Description:	Class MDT - Multiple Document Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include <string.h>

#if defined(_MSDOS) || defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "mdt.hxx"
#include "common.hxx"

MDT::MDT(const STRING& DbFileStem, const GDT_BOOLEAN WrongEndian) {
	MdtWrongEndian = WrongEndian;
	FileStem = DbFileStem;
	STRING Fn;
	FILE* Fp;
	Fn = FileStem;
	Fn += DbExtMdtGpIndex;
	// Load Gp Index
	Fp = fopen(Fn, "rb");
	if (Fp) {
		TotalEntries = GetFileSize(Fp) / sizeof(GPREC);
		GpIndex = new GPREC[TotalEntries];
		fread(GpIndex, 1, TotalEntries * sizeof(GPREC), Fp);
		fclose(Fp);
		// Load Key Index
		Fn = FileStem;
		Fn += DbExtMdtKeyIndex;
		Fp = fopen(Fn, "rb");
		if (Fp) {
			KeyIndex = new KEYREC[TotalEntries];
			fread(KeyIndex, 1, TotalEntries * sizeof(KEYREC), Fp);
			fclose(Fp);
		} else {
			KeyIndex = 0;
		}
	} else {
		TotalEntries = 0;
		GpIndex = 0;
		KeyIndex = 0;
	}
	// Open on-disk MDT
	ReadOnly = GDT_FALSE;
	Fn = FileStem;
	Fn += DbExtMdt;
	MdtFp = fopen(Fn, "r+b");
	if (MdtFp == 0) {
		MdtFp = fopen(Fn, "w+b");
		if (MdtFp) {
			fclose(MdtFp);
			MdtFp = fopen(Fn, "r+b");
		} else {
			MdtFp = fopen(Fn, "rb");
			if (!MdtFp) {
				perror(Fn);
				exit(1);
			}
			ReadOnly = GDT_TRUE;
		}
	}
	Changed = GDT_FALSE;
	KeyIndexSorted = GDT_TRUE;
	GpIndexSorted = GDT_TRUE;
	MaxEntries = TotalEntries;
	if (MdtWrongEndian) {
		FlipIndexBytes();
	}
}

void MDT::FlipIndexBytes() {
	SIZE_T x;
	for (x=0; x<TotalEntries; x++) {
		GpSwab(&(GpIndex[x].GpStart));
		GpSwab(&(GpIndex[x].GpEnd));
		GpSwab(&(GpIndex[x].Index));
		GpSwab(&(KeyIndex[x].Index));
	}
}

void MDT::AddEntry(const MDTREC& MdtRecord) {
	if (ReadOnly == GDT_TRUE) {
		return;
	}
	STRING TempKey;
	if (TotalEntries == MaxEntries) {
		Resize(TotalEntries + 1);
	}
	// Add to Key Index
	MdtRecord.GetKey(&TempKey);
	TempKey.GetCString(KeyIndex[TotalEntries].Key, DocumentKeySize);
	KeyIndex[TotalEntries].Index = TotalEntries + 1;
	// Add to Gp Index
	GpIndex[TotalEntries].GpStart = MdtRecord.GetGlobalFileStart() + MdtRecord.GetLocalRecordStart();
	GpIndex[TotalEntries].GpEnd = MdtRecord.GetGlobalFileStart() + MdtRecord.GetLocalRecordEnd();
	GpIndex[TotalEntries].Index = TotalEntries + 1;
	// Add to on-disk MDT
	fseek(MdtFp, TotalEntries * sizeof(MDTREC), SEEK_SET);
	if (MdtWrongEndian) {
		MDTREC TempMdtrec;
		TempMdtrec = MdtRecord;
		TempMdtrec.FlipBytes();
		fwrite(&TempMdtrec, 1, sizeof(MDTREC), MdtFp);
	} else {
		fwrite(&MdtRecord, 1, sizeof(MDTREC), MdtFp);
	}
	TotalEntries++;
	Changed = GDT_TRUE;
	KeyIndexSorted = GDT_FALSE;
	GpIndexSorted = GDT_FALSE;
}

static int MdtCompareKeysByIndex(const void* KeyRecPtr1, const void* KeyRecPtr2) {
	return ( (((KEYREC*)KeyRecPtr1)->Index) -
			(((KEYREC*)KeyRecPtr2)->Index) );
}

static int MdtCompareGpByIndex(const void* GpRecPtr1, const void* GpRecPtr2) {
	return ( (((GPREC*)GpRecPtr1)->Index) -
			(((GPREC*)GpRecPtr2)->Index) );
}

void MDT::IndexSortByIndex() {
	qsort(KeyIndex, TotalEntries, sizeof(KEYREC), MdtCompareKeysByIndex);
	KeyIndexSorted = GDT_FALSE;
	qsort(GpIndex, TotalEntries, sizeof(GPREC), MdtCompareGpByIndex);
	GpIndexSorted = GDT_FALSE;
	Changed = GDT_TRUE;
}

SIZE_T MDT::RemoveDeleted() {
	if (ReadOnly == GDT_TRUE) {
		return 0;
	}
	IndexSortByIndex();
	SIZE_T x;
	SIZE_T n = 1;
	MDTREC Mdtrec;
	for (x=1; x<=TotalEntries; x++) {
		GetEntry(x, &Mdtrec);
		if (Mdtrec.GetDeleted() == GDT_FALSE) {
			if (x != n) {
				if (MdtWrongEndian) {
					Mdtrec.FlipBytes();
				}
				fseek(MdtFp, (n - 1) * sizeof(MDTREC), SEEK_SET);
				fwrite(&Mdtrec, 1, sizeof(MDTREC), MdtFp);
				KeyIndex[n-1] = KeyIndex[x-1];
				KeyIndex[n-1].Index = n;
				GpIndex[n-1] = GpIndex[x-1];
				GpIndex[n-1].Index = n;
			}
			n++;
		}
	}
	SIZE_T Count = TotalEntries - n + 1;
	TotalEntries = n - 1;
	INT FileDesc = fileno(MdtFp);
#if defined(_MSDOS) || defined(_WIN32)
	chsize(FileDesc, TotalEntries * sizeof(MDTREC));
#else
	ftruncate(FileDesc, TotalEntries * sizeof(MDTREC));
#endif
	return Count;
}

void MDT::GetEntry(const SIZE_T Index, MDTREC* MdtrecPtr) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		fseek(MdtFp, (Index - 1) * sizeof(MDTREC), SEEK_SET);
		fread(MdtrecPtr, 1, sizeof(MDTREC), MdtFp);
		if (MdtWrongEndian) {
			MdtrecPtr->FlipBytes();
		}
	}
}

void MDT::SetEntry(const SIZE_T Index, const MDTREC& MdtRecord) {
	if (ReadOnly == GDT_TRUE) {
		return;
	}
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		// Save on-disk record
		fseek(MdtFp, (Index - 1) * sizeof(MDTREC), SEEK_SET);
		if (MdtWrongEndian) {
			MDTREC TempMdtrec;
			TempMdtrec = MdtRecord;
			TempMdtrec.FlipBytes();
			fwrite(&TempMdtrec, 1, sizeof(MDTREC), MdtFp);
		} else {
			fwrite(&MdtRecord, 1, sizeof(MDTREC), MdtFp);
		}
		SIZE_T x;
		STRING Key;
		// Update Key Index
		for (x=0; x<TotalEntries; x++) {
			if (KeyIndex[x].Index == Index) {
				MdtRecord.GetKey(&Key);
				Key.GetCString(KeyIndex[x].Key, DocumentKeySize);
				break;
			}
		}
		// Update Gp Index
		for (x=0; x<TotalEntries; x++) {
			if (GpIndex[x].Index == Index) {
				GpIndex[x].GpStart = MdtRecord.GetGlobalFileStart() +
						MdtRecord.GetLocalRecordStart();
				GpIndex[x].GpEnd = MdtRecord.GetGlobalFileStart() +
						MdtRecord.GetLocalRecordEnd();
				break;
			}
		}
		KeyIndexSorted = GDT_FALSE;
		GpIndexSorted = GDT_FALSE;
		Changed = GDT_TRUE;
	}
}

static int MdtCompareKeys(const void* KeyRecPtr1, const void* KeyRecPtr2) {
	return strcmp((CHR*)(((KEYREC*)KeyRecPtr1)->Key),
			(CHR*)(((KEYREC*)KeyRecPtr2)->Key));
}

void MDT::SortKeyIndex() {
	qsort(KeyIndex, TotalEntries, sizeof(KEYREC), MdtCompareKeys);
	KeyIndexSorted = GDT_TRUE;
	Changed = GDT_TRUE;
}

void MDT::Resize(const SIZE_T Entries) {
	if (Entries < TotalEntries) {
		return;
	}
	// Resize Key Index
	KEYREC* TempKeyIndex = new KEYREC[Entries];
	if (KeyIndex) {
		memcpy(TempKeyIndex, KeyIndex, TotalEntries * sizeof(KEYREC));
		delete [] KeyIndex;
	}
	KeyIndex = TempKeyIndex;
	// Add to Gp Index
	GPREC* TempGpIndex = new GPREC[Entries];
	if (GpIndex) {
		memcpy(TempGpIndex, GpIndex, TotalEntries * sizeof(GPREC));
		delete [] GpIndex;
	}
	GpIndex = TempGpIndex;
	MaxEntries = Entries;
}

SIZE_T MDT::LookupByKey(const STRING& Key) {
	if (TotalEntries == 0) {
		return 0;
	}
	if (KeyIndexSorted == GDT_FALSE) {
		SortKeyIndex();
	}
	KEYREC KeyRec;
	Key.GetCString(KeyRec.Key, DocumentKeySize);
	KEYREC* KeyRecPtr;
	KeyRecPtr = (KEYREC*)bsearch(&KeyRec, KeyIndex, TotalEntries, sizeof(KEYREC), MdtCompareKeys);
	if (KeyRecPtr) {
		return KeyRecPtr->Index;
	} else {
		return 0;
	}
}

SIZE_T MDT::GetMdtRecord(const STRING& Key, MDTREC* MdtrecPtr) {
	INT x = LookupByKey(Key);
	if (x) {
		GetEntry(x, MdtrecPtr);
	} else {
		MDTREC EmptyMdtrec;
		*MdtrecPtr = EmptyMdtrec;
	}
	return x;
}

static int MdtCompareGpStarts(const void* GpRecPtr1, const void* GpRecPtr2) {
	return ((((GPREC*)GpRecPtr1)->GpStart) -
			(((GPREC*)GpRecPtr2)->GpStart));
}

static int MdtCompareGps(const void* GpPtr, const void* GpRecPtr) {
	if ( ( *((GPTYPE*)GpPtr) >= ((GPREC*)GpRecPtr)->GpStart ) &&
			( *((GPTYPE*)GpPtr) <= ((GPREC*)GpRecPtr)->GpEnd ) ) {
		return 0;
	} else {
		if ( *((GPTYPE*)GpPtr) < ((GPREC*)GpRecPtr)->GpStart ) {
			return -1;
		} else {
			return 1;
		}
	}
}

void MDT::SortGpIndex() {
	qsort(GpIndex, TotalEntries, sizeof(GPREC), MdtCompareGpStarts);
	GpIndexSorted = GDT_TRUE;
	Changed = GDT_TRUE;
}

SIZE_T MDT::LookupByGp(const GPTYPE Gp) {
	if (TotalEntries == 0) {
		return 0;
	}
	if (GpIndexSorted == GDT_FALSE) {
		SortGpIndex();
	}
	GPREC* GpRecPtr;
	GpRecPtr = (GPREC*)bsearch(&Gp, GpIndex, TotalEntries, sizeof(GPREC), MdtCompareGps);
	if (GpRecPtr) {
		return GpRecPtr->Index;
	} else {
		return 0;
	}
}

SIZE_T MDT::GetMdtRecord(const GPTYPE gp, MDTREC* MdtrecPtr) {
	INT x = LookupByGp(gp);
	if (x) {
		GetEntry(x, MdtrecPtr);
	} else {
		MDTREC EmptyMdtrec;
		*MdtrecPtr = EmptyMdtrec;
	}
	return x;
}

GPTYPE MDT::GetNextGlobal() const {
	if (TotalEntries == 0) {
		return 0;
	} else {
		MDTREC Mdtrec;
		GetEntry(TotalEntries, &Mdtrec);
		return (Mdtrec.GetGlobalFileEnd() + 2);
//		return (Mdtrec.GetGlobalFileEnd() + 1);
	}
}

SIZE_T MDT::GetTotalEntries() const {
	return TotalEntries;
}

void MDT::GetUniqueKey(STRING* StringPtr) {
	INT x = 1;
	INT y = 1;
	CHR s[30];
	STRING S;
	if (*StringPtr != "") {
		x = StringPtr->GetInt();
	}
	do {
		sprintf(s, "%d%d", y, x);
		S = s;
		if (LookupByKey(S) == 0) {
			*StringPtr = S;
			return;
		}
		x++;
		if (x > 30000) {
			x = 1;
			y++;
		}
	} while (1);
}

void MDT::Dump() const {
	INT x;
	STRING s;
	MDTREC Mdtrec;
	for (x=1; x<=TotalEntries; x++) {
		GetEntry(x, &Mdtrec);
		Mdtrec.GetKey(&s);
		s.Print();
		cout << '\t';
		Mdtrec.GetFileName(&s);
		s.Print();
		cout << "\tgs:" << Mdtrec.GetGlobalFileStart() <<
				"\tge:" << Mdtrec.GetGlobalFileEnd() <<
				"\tls:" << Mdtrec.GetLocalRecordStart() <<
				"\tle:" << Mdtrec.GetLocalRecordEnd() << endl;
	}
}

INT MDT::GetChanged() const {
	return Changed;
}

MDT::~MDT() {
	if ( (Changed == GDT_TRUE) && (ReadOnly == GDT_FALSE) ) {
		// Sort indices
		if (GpIndexSorted == GDT_FALSE) {
			SortGpIndex();
		}
		if (KeyIndexSorted == GDT_FALSE) {
			SortKeyIndex();
		}
		// Use same byte ordering as original files
		if (MdtWrongEndian) {
			FlipIndexBytes();
		}
		STRING Fn;
		FILE* Fp;
		// Save Gp Index
		Fn = FileStem;
		Fn += DbExtMdtGpIndex;
		Fp = fopen(Fn, "wb");
		if (!Fp) {
			perror(Fn);
			exit(1);
		}
		else {
			fwrite(GpIndex, 1, TotalEntries * sizeof(GPREC), Fp);
			fclose(Fp);
		}
		if (GpIndex) {
			delete [] GpIndex;
		}
		// Save Key Index
		Fn = FileStem;
		Fn += DbExtMdtKeyIndex;
		Fp = fopen(Fn, "wb");
		if (!Fp) {
			perror(Fn);
			exit(1);
		}
		else {
			fwrite(KeyIndex, 1, TotalEntries * sizeof(KEYREC), Fp);
			fclose(Fp);
		}
		if (KeyIndex) {
			delete [] KeyIndex;
		}
	}
	fclose(MdtFp);
}
