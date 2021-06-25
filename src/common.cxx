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
File:		common.cxx
Version:	1.00
Description:	Common functions
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include <string.h>

#if defined(_MSDOS) || defined(_WIN32)
#include <direct.h>
#endif
#ifdef UNIX
#include <unistd.h>
#endif

#include "common.hxx"

void AddTrailingSlash(PSTRING PathName) {
	STRINGINDEX x;
	if ( ((x=PathName->GetLength()) > 1) &&
			(PathName->GetChr(x) != '/') ) {
		PathName->Cat('/');
	}
}

void RemovePath(PSTRING FileName) {
	STRINGINDEX x;
	if ((x=FileName->SearchReverse('/')) != 0) {
		FileName->EraseBefore(x+1);
	}
}

void RemoveFileName(PSTRING PathName) {
	STRINGINDEX x;
	x = PathName->SearchReverse('/');
	PathName->EraseAfter(x);
}

void RemoveFileExtension(PSTRING PathName) {
	STRINGINDEX x;
	x = PathName->SearchReverse('.');
	PathName->EraseAfter(x);
}

LONG GetFileSize(PFILE FilePointer) {
	LONG Position, Size;
	Position = ftell(FilePointer);
	fseek(FilePointer, 0, 2);
	Size = ftell(FilePointer);
	fseek(FilePointer, Position, 0);
	return Size;
}

void ExpandFileSpec(PSTRING FileSpec) {
	if (FileSpec->GetChr(1) == '/') {
		return;
	}
	STRING OldFileSpec;
	STRING NewFileSpec;
	STRINGINDEX p, p2;
	STRING s;
	INT Special;
	CHR Cwd[1024];
	getcwd(Cwd, 1022);
	NewFileSpec = Cwd;
	AddTrailingSlash(&NewFileSpec);
	OldFileSpec = *FileSpec;
	while ( (p=OldFileSpec.Search('/')) != 0) {
		Special = 0;
		s = OldFileSpec;
		s.EraseAfter(p);
		if ( (s.Equals("./")) || (s.Equals("/")) ) {
			Special = 1;
		}
		if (s.Equals("../")) {
			Special = 1;
			p2 = NewFileSpec.SearchReverse('/');
			if (p2 > 1) {
				NewFileSpec.EraseAfter(p2-1);
			}
			p2 = NewFileSpec.SearchReverse('/');
			NewFileSpec.EraseAfter(p2);
		}
		if (!Special) {
			NewFileSpec.Cat(s);
		}
		OldFileSpec.EraseBefore(p+1);
	}
	NewFileSpec.Cat(OldFileSpec);
	*FileSpec = NewFileSpec;
}

void GpSwab(PGPTYPE GpPtr) {
	GPTYPE Gp;
	swab((CHR*)GpPtr, (CHR*)&Gp, sizeof(GPTYPE));
	*((UINT2*)GpPtr) = (UINT2)((UINT2)*(((UINT2*)&Gp)+1));
	*(((UINT2*)GpPtr)+1) = (UINT2)*((UINT2*)&Gp);
}

GDT_BOOLEAN IsBigEndian() {
	UINT2 Test = 1;
	return (*((PUCHR)(&Test)) == 0) ? GDT_TRUE : GDT_FALSE;
}
