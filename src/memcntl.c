
/*
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************************************************************
* MemCNTL.c - This module handles all allocation and de-allocation of 
* memory
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "gdt.h"
#include "memcntl.h"  
/* includes the MemBlock structure declaration */

/**************************************************************************/
/* AllocSafe - Allocate memory placing all allocated blocks into a linked */
/* list of MemBlocks                                                      */
/**************************************************************************/
char *AllocSafe(struct MemBlock **base,INT4 size,INT4 flags,INT4 type)
{
  /*char *malloc();*/
  struct MemBlock *block;
  int i;
  char *mem;

  if ((block = (struct MemBlock *)malloc(sizeof(struct MemBlock))))
    { /* store the block in a pushdown stack */
      if (*base == NULL) 
         {*base = block;
          block->nextmem = NULL;
         }
      else { block->nextmem = *base;
             *base = block;
           }
      block->memtype = type;
      block->memsize = size;
      if ((mem = block->data = (char *)malloc((int)size)))
         {if (flags & MEMF_CLEAR)
             for(i=0;i<size;i++) *mem++ = '\0'; /* zero out the memory */
          return(block->data);
         }
      else { fprintf(stderr,"memcntl: Not enough memory for new data");
         return(NULL);
       }
     }
  else { fprintf(stderr,"memcntl: Not enough memory for new control structure");
         return(NULL);
       }
}


/**************************************************************************/
/* FreeSafe - free the data and memblock associated with the supplied     */
/* pointer, and patch up the memblock list                                */
/**************************************************************************/
int FreeSafe(struct MemBlock **base,char *mem,int flag)
{
  struct MemBlock *prev, *curr, *next;

  /* if nothing is allocated , just return */
  if (*base == NULL) return (0);
  if (flag == 0 && mem == NULL) return (0);

  prev = *base;
  curr = *base;

  do { next = curr->nextmem;

       if (flag) /* free all memory */
         {
           free(curr->data);
           free(curr);
           curr = next;
         }
       else
         {
           if (curr->data == mem)
             { if (curr == *base) *base = next;
               else prev->nextmem = next;
               free(curr->data);
               free(curr);
               return (0);
             }
           else
             { prev = curr;
               curr = next;
             }
         }
      } while (curr);
return(0);
}

