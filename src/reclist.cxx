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
File:		reclist.cxx
Version:	1.00
Description:	Class RECLIST - Database Record List
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdio.h>
#include "reclist.hxx"
#include "common.hxx"

RECLIST::RECLIST() {
	Table = new RECORD[1000];
	TotalEntries = 0;
	MaxEntries = 1000;
}

void RECLIST::AddEntry(const RECORD& RecordEntry) {
	if (TotalEntries == MaxEntries)
		Expand();
	Table[TotalEntries] = RecordEntry;
	TotalEntries = TotalEntries + 1;
}

void RECLIST::GetEntry(const INT Index, PRECORD RecordEntry) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*RecordEntry = Table[Index-1];
	}
}

void RECLIST::Expand() {
	Resize(TotalEntries+1000);
}

void RECLIST::CleanUp() {
	Resize(TotalEntries);
}

void RECLIST::Resize(const INT Entries) {
	PRECORD Temp = new RECORD[Entries];
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

INT RECLIST::GetTotalEntries() const {
	return TotalEntries;
}

RECLIST::~RECLIST() {
	if (Table)
		delete [] Table;
}
