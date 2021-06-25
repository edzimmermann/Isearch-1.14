
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
#ifndef _memcntl_
#define _memcntl_

#ifdef __cplusplus
extern "C" {
#endif

/* Memory type bits */
#define GENERALMEM     0x00000000
#define SCREENINFOMEM  0x00000001
#define WININFOMEM     0x00000002
#define GADINFOMEM     0x00000004
#define MENUINFOMEM    0x00000008
#define ITEXTMEM       0x00000010
#define GADGETMEM      0x00000020
#define MENUMEM        0x00000040
#define MENUITEMMEM    0x00000080
#define STRINGINFOMEM  0x00000100
#define PROPINFOMEM    0x00000200
#define BORDERMEM      0x00000400
#define BORDERDATMEM   0x00000800
#define IMAGEMEM       0x00001000
#define IMAGEDATMEM    0x00002000
#define TEXTMEM        0x00004000
#define BITMAPMEM      0x00008000
#define NEWSCREENMEM   0x00010000
#define NEWWINDOWMEM   0x00020000
#define FREEOBJMEM    0x00040000

#define MEMF_PUBLIC (INT4)0
#define MEMF_CLEAR  (INT4)1

/* Memblock - My own version of the Intuition Remember structure for */
/* handling allocation and deallocation of all the structures used in */
/* the User Interface Tool.                                           */
struct MemBlock {
   struct MemBlock *nextmem;
   char *data;
   INT4 memsize;
   INT4 memtype;
   };

char *AllocSafe(struct MemBlock **base,INT4 size,INT4 flags,INT4 type);
int FreeSafe(struct MemBlock **base,char *mem,int flag);

#ifdef __cplusplus
}
#endif

#endif
