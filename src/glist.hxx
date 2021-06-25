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

#ifndef _GLIST_HXX_
#define _GLIST_HXX_

#include "gdt.h"

/* 
   I want to be able to deal with different data types on the same
   list so I need some typing information.
   Each cell has an element called Type that relates to one of the
   types below.
*/

#define LIST_BASE 1000
#define LIST_UNSIGNEDCHAR LIST_BASE+0 
#define LIST_CHAR LIST_BASE+1
#define LIST_ENUM LIST_BASE+2
#define LIST_UNSIGNEDINT LIST_BASE+3
#define LIST_SHORTINT LIST_BASE+4
#define LIST_INT LIST_BASE+5
#define LIST_UNSIGNEDLONG LIST_BASE+6
#define LIST_LONG LIST_BASE+7
#define LIST_FLOAT LIST_BASE+8
#define LIST_DOUBLE LIST_BASE+9
#define LIST_LONGDOUBLE LIST_BASE+10
#define LIST_NEAR LIST_BASE+11
#define LIST_FAR LIST_BASE+12
#define LIST_PTR LIST_BASE+13

typedef struct _cell POSITION;	// For readability
typedef void ATOM;	// For readability

struct _cell {
  ATOM *Atom;		// Any data type you want
  int Type;		// Optional type information
  POSITION *Next;	// Next cell in list
  POSITION *Prev;	// Prev cell in list
};

typedef POSITION *PPOSITION;

class GLIST {
public:
  GLIST();
  GDT_BOOLEAN IsEmpty();
  POSITION* First();
  POSITION* Last();
  POSITION* Next(POSITION *c);
  POSITION* Prev(POSITION *c);
  GDT_BOOLEAN InsertBefore(POSITION *c, ATOM *a, int Type);
  GDT_BOOLEAN InsertAfter(POSITION *c, ATOM *a, int Type);
  GDT_BOOLEAN InsertBefore(POSITION *c, ATOM *a);
  GDT_BOOLEAN InsertAfter(POSITION *c, ATOM *a);
  ATOM* Retrieve(POSITION *c);
  GDT_BOOLEAN Update(POSITION *c, ATOM *a);
  INT GetLength();
  INT DataType(POSITION *c);
  void Delete(POSITION *c);

private:
  INT Length;		/* Number of cells in list */
  POSITION *Head, *Tail;	/* Pointers to head and tail of list */
};
 
typedef GLIST *PGLIST; 

#endif /* _GLIST_HXX_ */

