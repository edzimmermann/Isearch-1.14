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
File:		merge.cxx
Version:	1.00
Description:	Class IDB
Author:		Jon Magid, jem@cnidr.org
@@@*/

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include "defs.hxx"

void printint(int *data, int nel) {
	for (int i = 0; i < nel; i++)
		printf("%d ", *(data + i));
	puts("");
}

void buildGpHeap(GPTYPE *data, size_t heapsize, 
	int (*compar)(const void *, const void *), int position, int reverse) {

	int childpos;
	GPTYPE value;

	value = data[position];

	while (position < heapsize) {
		childpos = position * 2 + 1;
		if (childpos < heapsize) {
			if ((childpos < heapsize) && 
				( ((*compar)( ((void *)(data + childpos + 1)), 
					((void *)(data + childpos)))) == 1 ))
						childpos++;
			if ( ((*compar)((void *)&value, (void *)(data + childpos))) > 0) {
				data[position] = value;
				return;
			}
			else {
				data[position] = data[childpos];
				position = childpos;
			}
		}
		else {
			data[position] = value;
			return;
		}
	}
}

void buildHeap(void *data, size_t heapsize, size_t width,  
	int (*compar) (const void *, const void *), int position, int reverse) {

	int childpos, cmpstatus; 
	void *value;

	value = (void *)malloc(width);
	memcpy(value, ((char *)data + (width * position) ), width);

	while (position < heapsize) {
		childpos = position * 2 + 1;

		//if there is a child...
		if (childpos < heapsize) {

			//if there is another child
			if (childpos + 1 < heapsize) {
				
				//make childpos equal to the greatest child
				//(unless we're reversed)
				cmpstatus =  (*compar)( ((char *)data + (width * (childpos + 1))), 
					((char *)data + (width * childpos)) );

				if  ( cmpstatus > 0 && !reverse)
					childpos++;
				else if (cmpstatus < 0 && reverse)
					childpos++;
			
			}
			
			//is unlocated value larger than the largest child?
			cmpstatus = ( (*compar)( value, ((char *)data + (width * childpos) )) );

			//unless we're reversed (smaller than the smallest child)
			if (reverse) 
				cmpstatus = 0 - cmpstatus;

			//value is bigger, we're done.
			if (cmpstatus > 0) {
					memcpy( ((char *)data + (position * width) ), value, width);
					free(value);
					return;
			} //child is bigger or same size, back through the loop 
			//again to find a place for value
			else {
					memcpy( ((char *)data + (position * width) ), 
						((char *)data + (childpos * width) ), width);
					position=childpos;
			}
		}

		//node has no child. loop is finished
		else {
			memcpy(((char *)data+ (position * width) ), value, width);
			free(value);
			return;
		}
	}
}

void GpHsort(GPTYPE *data, size_t nel, int (*compar) 
	(const void *, const void *)) {

	int i, tmp;
	for (i = nel/2; i > 0; i--)
		buildGpHeap(data, nel, compar, i, 0);
	
	for (i = nel - 1; i > 0; i--) {
		tmp = data[i];
		data[i] = data[0];
		data[0] = tmp;
		buildGpHeap(data, i, compar, 0, 0);
	}
}

void hsort(void *data, size_t nel, size_t width, 
	int (*compar) (const void *, const void *)) {

	int i;
	void *tmp;
	for (i = nel/2; i >=0; i--) 
		buildHeap(data, nel, width, compar, i, 0);

/*	FILE *fp = fopen("heap.dbg","wb");
	if (!fp) {
		perror("heap.dbg");
		exit(1);
	}
	fwrite(data, width, nel, fp);
	fclose(fp);
*/

	tmp = malloc(width);
	for (i = nel - 1; i > 0; i--) {
		memcpy(tmp, ((char *)data + (i * width)), width);
		memcpy(((char *)data + (i * width)), data, width);
		memcpy(data, tmp, width);
		buildHeap(data, i, width, compar, 0, 0);
	}
	free(tmp);
}

