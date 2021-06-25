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

#include "glist.hxx"
#include "gdt.h"

/*
   PRE:	 None.
   POST: Pointer to allocated GLIST
*/
GLIST::GLIST()
{
  Length=0;
  Head=NULL;
  Tail=NULL;
}

/*
   PRE:	 l has been created.
   POST: Nonzero if empty
	 Zero if not empty
*/
GDT_BOOLEAN GLIST::IsEmpty(void)
{
  if(Length==0)
    return GDT_TRUE;
  return GDT_FALSE;
}

/*
   PRE:	 l has been created.
	 l contains at least one cell.
   POST: Pointer to first cell in list on success.
	 NULL pointer on failure.
*/
POSITION* GLIST::First()
{
  return Head;
}

/*
   PRE:	 l has been created.
	 l contains at least one cell.
   POST: Pointer to last cell in list on success.
	 NULL pointer on failure.
*/
POSITION* GLIST::Last()
{
  if(Tail == NULL)
    // Attempt to access first position of empty list
    return NULL;

  return Tail;
}

/*
   PRE:	 l has been created.
	 c is a pointer to a cell in l.
   POST: Pointer to next cell in list on success.
	 NULL pointer if no more cells in list.
*/
POSITION* GLIST::Next(POSITION *c)
{
  if(Tail == c)
    // Attempt to access one position past end of list
    return NULL;

  return c->Next;
}

/*
   PRE:	 l has been created.
	 c is a pointer to a cell in l.
   POST: Pointer to previous cell in list on success.
	 NULL pointer if no more cells in list before c.
*/
POSITION* GLIST::Prev(POSITION *c)
{
  if(Head == c)
    // Attempt to access one position before start of list
    return NULL;

  return c->Prev;
}

GDT_BOOLEAN GLIST::InsertBefore(POSITION *c, ATOM *a, int Type)
{
  ATOM *ta;

  if(a==NULL)
    return GDT_FALSE;

  if(IsEmpty())
    InsertAfter(c,a,Type);
  else {
    InsertAfter(c,a,Type);
    ta = Retrieve(c);
    Update(c,a);
    Update(Next(c), ta);
  }	
  return GDT_TRUE;

}

GDT_BOOLEAN GLIST::InsertBefore(POSITION *c, ATOM *a)
{
  return(InsertBefore(c,a,LIST_PTR));
}

GDT_BOOLEAN GLIST::Update(POSITION *c, ATOM *a)
{
  if(a==NULL)
    return GDT_FALSE;

  c->Atom = a;

  return GDT_TRUE;
}

/*
   PRE:	 l has been created.
	 c points to a cell in l.
	 a points to an allocated structure.
   POST: 0 if out of memory condition.
	 1 on success.  a is inserted in list after c.
*/
GDT_BOOLEAN GLIST::InsertAfter(POSITION *c, ATOM *a, int Type)
{
  POSITION *nc, *tc;

  if(Head == NULL) {
    // Inserting into empty list
    if((Head = new POSITION()) == NULL)
      return GDT_FALSE;
    Head->Atom = a;
    Head->Type = Type;
    Head->Next = NULL;
    Head->Prev = NULL;
    Tail = Head;
  } else {
    // Inserting into non-empty list
    if((nc =  new POSITION()) == NULL)
      return GDT_FALSE;
    nc->Atom = a;
    nc->Type = Type;
    tc = c->Next;
    c->Next = nc;
    nc->Prev = c;
    nc->Next = tc;
    if(c == Tail)
      // Inserted after tail, move the pointer
      Tail = nc;
  }
  Length=Length+1;

  return GDT_TRUE;
}

GDT_BOOLEAN GLIST::InsertAfter(POSITION *c, ATOM *a)
{
  return(InsertAfter(c,a,LIST_PTR));
}

/*
   PRE:	 l has been created.
	 c points to a cell in l.
   POST: Pointer to atom in c.
*/
ATOM* GLIST::Retrieve(POSITION *c)
{
  return c->Atom;
}

INT GLIST::GetLength()
{
  return Length;
}

/*
   PRE:	 l has been created.
	 c points to a cell in l.
	 Caller has freed memory allocated within the atom in c.
   POST: c is removed from l.
*/
void GLIST::Delete(POSITION *c)
{
  if(IsEmpty())
    return;

  if(Head == c) {
    if(Tail == c) {
      // singleton list
      Head = NULL;
      Tail = NULL;
    } else {
      // deleting first cell from non-singleton
      Head = c->Next;
      c->Next->Prev = NULL;
    }
  } else {
    if(Tail == c) {
      // deleting last cell from non-singleton
      Tail = c->Prev;
      c->Prev->Next = NULL;
    } else {
      // deleting cell from interior
      c->Prev->Next = c->Next;
      c->Next = c->Prev;
    }
  }
  delete c;
  Length=Length - 1;

  return;
}

/*
   PRE:	 c is a valid cell.
   POST: Data type integer as defined in list.h
*/
INT GLIST::DataType(POSITION *c)
{
  return(c->Type);
}
