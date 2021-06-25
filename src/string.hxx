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
File:		string.hxx
Version:	1.01
Description:	Class STRING
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef STRING_HXX
#define STRING_HXX

#include <iostream.h>
#include "gdt.h"

#ifdef METRICS
#include <sys/types.h>
#include <time.h>
#endif


typedef INT4 STRINGINDEX;
typedef STRINGINDEX* PSTRINGINDEX;

class STRING {
public:
	STRING();
	STRING(const STRING& OtherString);
	STRING(const CHR* CString);
	STRING(const UCHR* CString);
	STRING(const UCHR* NewBuffer, const STRINGINDEX BufferLength);
	STRING& operator=(const CHR* CString);
	STRING& operator=(const GDT_BOOLEAN BoolValue);
	STRING& operator=(const INT IntValue);
	STRING& operator=(const DOUBLE DoubleValue);
	STRING& operator=(const STRING& OtherString);
	void STRING::Set(const UCHR* NewBuffer, const STRINGINDEX BufferLength);
 	STRING& operator+=(const UCHR Character);
 	STRING& operator+=(const CHR* CString);
 	STRING& operator+=(const STRING& OtherString);
	INT operator==(const STRING& OtherString) const;
	INT operator==(const CHR* CString) const;
	INT operator!=(const STRING& OtherString) const;
	INT operator!=(const CHR* CString) const;
	INT operator^=(const STRING& OtherString) const;
	INT operator^=(const CHR* CString) const;
	INT Equals(const STRING& OtherString) const;
	INT Equals(const CHR* CString) const;
	INT CaseEquals(const STRING& OtherString) const;
	INT CaseEquals(const CHR* CString) const;
	void Print() const;
	void Print(PFILE FilePointer) const;
	friend ostream& operator<<(ostream& os, const STRING& str);
	INT GetInt() const;
	DOUBLE GetFloat() const;
	GDT_BOOLEAN FGet(PFILE FilePointer, const STRINGINDEX MaxCharacters);
	STRINGINDEX GetLength() const;
	UCHR GetChr(STRINGINDEX Index) const;
	void SetChr(const STRINGINDEX Index, const UCHR NewChr);
	void Cat(const UCHR Character);
	void Cat(const CHR* CString, STRINGINDEX CLength=-1);
	void Cat(const STRING& OtherString);
	void Insert(const INT InsertionPoint, const STRING& OtherString);
	STRINGINDEX Search(const CHR* CString) const;
	STRINGINDEX Search(const UCHR Character) const;
	STRINGINDEX SearchReverse(const CHR* CString) const;
	STRINGINDEX SearchReverse(const UCHR Character) const;
	INT Replace(const CHR* CStringSearch, const CHR* CStringReplace);
	INT Replace(const CHR* CStringSearch, const STRING& CStringReplace);
	void EraseBefore(const STRINGINDEX Index);
	void EraseAfter(const STRINGINDEX Index);
	void UpperCase();
	void GetCString(CHR* CStringBuffer, const INT BufferSize) const;
	CHR* NewCString() const;	// Remember to delete [] !!
	UCHR* NewUCString() const;	// Remember to delete [] !!
	void WriteFile(const STRING& FileName) const;
	void ReadFile(const STRING& FileName);
	GDT_BOOLEAN IsNumber();
	GDT_BOOLEAN IsPrint();
	void MakePrintable();
#ifdef METRICS
	INT GetNumTimesConstructed(void) ;
	INT GetNumTimesCopied(void) ;
	INT GetNumTimesExpanded(void) ;
	INT GetNumTimesDeleted(void) ;
	INT GetTotalNumStrings(void) ;
	INT4 GetTotalStringLength(void) ;
	INT GetNumNullStrings(void) ;
	INT GetTotalStringExpns(void);
	DOUBLE GetAvgCopiedStrLen(void) ;
	INT GetTotalTimesShrunk(void) ;
	DOUBLE GetAvgTotalStrLen(void) ;
	DOUBLE STRING::GetAvgTotalStringExpns(void);
	time_t GetHowLong(void) ;
	void WriteMetrics(FILE *fp) ;
	void PrintMetrics(void) ;
#endif
	void SetMinInitBufLen(INT InitBufLen);
	void SetBufLenIncr(INT BufLenIncr);
	void SetDoDoubleBufLen(GDT_BOOLEAN DoDoubling);
	void StrBuffAlloc(INT BufferSizeRequest);
	~STRING();
private:
	void STRING::Copy(const UCHR *CString, STRINGINDEX CLength);
	STRINGINDEX Length;
	UCHR* Buffer;
	static INT4 InitialBufferLength;
	static INT4 BufferLengthIncr;
	static GDT_BOOLEAN DoubleBufferOnCopy;
	INT4 BufferSize;
#ifdef METRICS
	static INT NumTimesConstructed;
	static INT NumTimesCopied;
	static INT NumTimesExpanded;
	static INT NumNullStrings;
	static INT NumTimesDeleted;
	static INT4 TotalStringLength;
	static INT TotalNumStrings;
	static INT TotalStringExpns;
	static time_t HowLong;
#endif
//	static EREG ereg;
};

typedef STRING* PSTRING;

PFILE fopen(const STRING& FileName, const CHR* Type);
INT StrUnlink(const STRING& FileName);
INT StrCaseCmp(const CHR* s1, const CHR* s2);
INT StrCaseCmp(const UCHR* s1, const UCHR* s2);
INT StrNCaseCmp(const CHR* s1, const CHR* s2, const INT4 n);
INT StrNCaseCmp(const UCHR* s1, const UCHR* s2, const INT4 n);
INT StrNCmp(const CHR* s1, const CHR* s2, const INT4 n);
INT StrNCmp(const UCHR* s1, const UCHR* s2, const INT4 n);
void perror(const STRING &s);
INT rename(const STRING From, const STRING To);

#endif /* STRING_HXX */
