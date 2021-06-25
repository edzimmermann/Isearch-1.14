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
File:		vlist.cxx
Version:	1.00
Description:	Class VLIST - Doubly Linked Circular List Base Class
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "vlist.hxx"

VLIST::VLIST() {
	Next = this;	// a circle of one
	Prev = this;
}

SIZE_T VLIST::GetTotalEntries() const {
	SIZE_T x = 0;
	VLIST* p = Next;
	while (p != this) {
		x++;
		p = p->Next;
	}
	return x;
}

void VLIST::AddNode(VLIST* NewEntryPtr) {
	Prev->Next = NewEntryPtr;	// set last node to point to new node
	NewEntryPtr->Prev = Prev;	// set new node to point to last node
	Prev = NewEntryPtr;		// set top node to point to new node
	NewEntryPtr->Next = this;	// set new node to point to top node
}

VLIST* VLIST::GetNodePtr(const SIZE_T Index) const {
	SIZE_T x = 1;
	VLIST* p = Next;	// start at first real node
	while (p != this) {	// exit when we reach full circle
		if (x == Index) {
			return p;	// found it
		}
		p = p->Next;
		x++;
	}
	return 0;	// Index was never matched
}

VLIST* VLIST::GetNextNodePtr() const {
	return Next;
}

void VLIST::Clear() {
	if (this->Next != this) {
		Prev->Next = 0;	// disattach circle to isolate nodes
		Prev = 0;
		delete this->Next;	// delete all subsequent nodes
		Next = this;	// reattach circle of one
		Prev = this;
	}
}

void VLIST::EraseAfter(const SIZE_T Index) {
	VLIST* p = VLIST::GetNodePtr(Index);
	if (p) {
		if (p->Next != this) {
			Prev->Next = 0;	// disattach circle to isolate nodes
			Prev = 0;
			delete p->Next;	// delete all subsequent nodes
			p->Next = this;	// reattach circle
			this->Prev = p;
		}
	}
}

void VLIST::Reverse() {
	VLIST* p = this;
	VLIST* nextp;
	VLIST* temp;
	do {
		nextp = p->Next;
		temp = p->Prev;	// reverse pointers
		p->Prev = p->Next;
		p->Next = temp;
		p = nextp;
	} while (p != this);
}

VLIST::~VLIST() {
  // Disattach from previous node
//  if (GetTotalEntries() > 1)       // Insure++ says this might be needed
  Prev->Next = 0;

  // Delete next node
  if (Next) {
    delete Next;
  }
}
