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
File:		string.cxx
Version:	1.01
Description:	Class STRING
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#ifdef UNIX
#include <unistd.h>
#endif
#include <ctype.h>
#include "string.hxx"

#ifdef METRICS

#include <stdio.h>
#include <sys/types.h>
#include <time.h>

INT STRING::NumTimesConstructed=0;
INT STRING::NumTimesCopied=0;
INT STRING::NumTimesExpanded=0;
INT STRING::NumTimesDeleted=0;
INT STRING::TotalStringLength=0;
INT STRING::TotalNumStrings=0;
INT STRING::NumNullStrings=0;
INT STRING::TotalStringExpns=0;

INT STRING::GetNumTimesConstructed(void)  {
  return NumTimesConstructed;
}

INT STRING::GetNumTimesCopied(void)  {
  return NumTimesCopied;
}

INT STRING::GetNumTimesExpanded(void)  {
  return NumTimesExpanded;
}

INT STRING::GetNumTimesDeleted(void)  {
  return NumTimesDeleted;
}

INT STRING::GetTotalNumStrings(void)  {
  return TotalNumStrings;
}

INT4 STRING::GetTotalStringLength(void)  {
  return TotalStringLength;
}

INT STRING::GetNumNullStrings(void)  {
  return NumNullStrings;
}

INT STRING::GetTotalStringExpns(void) {
  return TotalStringExpns;
}

time_t STRING::GetHowLong(void)  {
	//not implemented yet.
  return -1;
}

DOUBLE STRING::GetAvgCopiedStrLen(void)  {
  return (DOUBLE) TotalStringLength / NumTimesCopied;
}

DOUBLE STRING::GetAvgTotalStrLen(void)  {
  return (DOUBLE) TotalStringLength / TotalNumStrings;
}

DOUBLE STRING::GetAvgTotalStringExpns(void) {
  return (DOUBLE) TotalStringExpns / NumTimesExpanded;
}

INT STRING::GetTotalTimesShrunk(void)  {
  return NumTimesCopied - NumTimesExpanded;
}

void STRING::WriteMetrics(FILE *fp)  {
  fprintf(fp, "STRING's were used %d times\n", TotalNumStrings);
  fprintf(fp, "The STRING constructors have been called %d times.\n",
	  NumTimesConstructed);
  fprintf(fp, "%d NULL STRING's have been created.\n", NumNullStrings);
  fprintf(fp, "%d STRING's have been copied.\n", NumTimesCopied);
  fprintf(fp, "%d STRING buffers have been deleted\n", NumTimesDeleted);
  fprintf(fp, "STRING's have been expanded %d times\n", NumTimesExpanded);
  fprintf(fp, "The average expansion is %4f characters\n", GetAvgTotalStringExpns());
  fprintf(fp, "STRING's have been shrunk %d times\n", GetTotalTimesShrunk());
  fprintf(fp, "Average Copied String Length %4f\n", GetAvgCopiedStrLen());
  fprintf(fp, "Average Total String Length %4f\n", GetAvgTotalStrLen());
}

void STRING::PrintMetrics(void)  {
  WriteMetrics(stdout);
}


#endif


/*
//magic values, based on experimentation (http://ficus.cnidr.org:8080/metrics.html)
INT STRING::InitialBufferLength = 20;
INT STRING::BufferLengthIncr = 12;
GDT_BOOLEAN STRING::DoubleBufferOnCopy = GDT_FALSE;
*/

//eventually we'll be smarter. for now, we're conservative.
INT STRING::InitialBufferLength = 0;
INT STRING::BufferLengthIncr = 0;
GDT_BOOLEAN STRING::DoubleBufferOnCopy = GDT_FALSE;

void STRING::SetMinInitBufLen(INT InitBufLen) {
  InitialBufferLength = InitBufLen;
}
	
void STRING::SetBufLenIncr(INT BufLenIncr) {
  BufferLengthIncr = BufLenIncr;
}

void STRING::SetDoDoubleBufLen(GDT_BOOLEAN DoDoubling) {
  DoubleBufferOnCopy = DoDoubling;
}

//this is where allocation and copying policy lives
void STRING::StrBuffAlloc(INT BufferSizeRequest) {
#ifdef METRICS
  if (BufferSizeRequest > Length) {
    NumTimesExpanded++;
    TotalStringExpns += (BufferSizeRequest - Length);
  }
#endif
  if (BufferSizeRequest > BufferSize) {
    if (BufferSize != 0) {
#ifdef METRICS
      NumTimesDeleted++;
#endif
      delete [] Buffer;
      if (DoubleBufferOnCopy) 
	BufferSize =  BufferSizeRequest * 2;
      else 
	BufferSize = (BufferSizeRequest + BufferLengthIncr);
    }
    else
      BufferSize = BufferSizeRequest > InitialBufferLength ?
	BufferSizeRequest : InitialBufferLength;
    
    Buffer = new UCHR[BufferSize];
  }
}

void STRING::Copy(const UCHR *CString, STRINGINDEX CLength) {
	if (CLength > 0) {
	  StrBuffAlloc(CLength);
	  memcpy(Buffer, CString, CLength);
#ifdef METRICS
	  NumTimesCopied++;
#endif
	}
#ifdef METRICS
	else {
	  NumNullStrings++;
	}
#endif
	Length = CLength;
#ifdef METRICS
	TotalNumStrings++;
	TotalStringLength += Length;
#endif
}


STRING::STRING() {
  Buffer = NULL;
  Length = BufferSize = 0;
#ifdef METRICS
  NumTimesConstructed++;
  TotalNumStrings++;
#endif
}

STRING::STRING(const STRING& OtherString) {
  Buffer = NULL;
  Length = BufferSize = 0; 
  if (OtherString.Length) {
    Copy(OtherString.Buffer, OtherString.Length);		
    Length = OtherString.Length;
  } else {
#ifdef METRICS
    NumNullStrings++;
#endif
  }
#ifdef METRICS
  NumTimesConstructed++;
#endif
}

STRING::STRING(const CHR* CString) {
  Buffer = NULL;
  Length = BufferSize = 0; 
  if (*CString != '\0') {
    STRINGINDEX StrLength = strlen(CString);
    Copy((UCHR *)CString, StrLength);
    Length = StrLength;
  } else {
#ifdef METRICS
    NumNullStrings++;
#endif
  }
#ifdef METRICS
  NumTimesConstructed++;
#endif
}

STRING::STRING(const UCHR* CString) {
  Buffer = NULL;
  Length = BufferSize = 0; 
  if (*CString != '\0') {
    STRINGINDEX StrLength = strlen((CHR *)CString);
    Copy((UCHR *)CString, StrLength);
    Length = StrLength;
  } else {
#ifdef METRICS
    NumNullStrings++;
#endif
  }
#ifdef METRICS
  NumTimesConstructed++;
#endif
}

STRING::STRING(const UCHR* NewBuffer, const STRINGINDEX BufferLength) {
        Buffer = NULL;
	Length = BufferSize = 0; 
	if ( (NewBuffer) && (BufferLength != 0) ) {
		Copy(NewBuffer, BufferLength);
		Length = BufferLength;
	} else {
#ifdef METRICS
		NumNullStrings++;
#endif
	}
#ifdef METRICS
	NumTimesConstructed++;
#endif
}

STRING& STRING::operator=(const CHR* CString) {
	STRINGINDEX StrLength = strlen(CString);
	Copy((UCHR *)CString, StrLength);
	Length = StrLength;
	return *this;
}

STRING& STRING::operator=(const GDT_BOOLEAN BoolValue) {
  CHR s[256];
  sprintf(s, "%i", (INT)BoolValue);
  *this = s;
  return *this;
}


STRING& STRING::operator=(const INT IntValue) {
	CHR s[256];
	sprintf(s, "%i", IntValue);
	*this = s;
	return *this;
}

STRING& STRING::operator=(const DOUBLE DoubleValue) {
  CHR s[256];
  sprintf(s, "%f", DoubleValue);
  *this = s;
  return *this;
}
 
STRING& STRING::operator=(const STRING& OtherString) {
	//if he is me, and me is he, oh gee!
	if (&OtherString == this) 
		return *this;
	if (OtherString.Length) {
		Copy(OtherString.Buffer, OtherString.Length);
		Length = OtherString.Length;
	} else {
		Length = 0;
#ifdef METRICS
		NumNullStrings++;
#endif
	}
#ifdef METRICS
	TotalNumStrings++;
#endif
	return *this;
}


void STRING::Set(const UCHR* NewBuffer, const STRINGINDEX BufferLength) {
	if ( (NewBuffer) && (BufferLength != 0) ) {
		Copy(NewBuffer, BufferLength);
		Length = BufferLength;
	} else {
#ifdef METRICS
		NumNullStrings++;
#endif
		Length = 0;
	}
}

STRING& STRING::operator+=(const UCHR Character) {
	Cat(Character);
	return *this;
}

STRING& STRING::operator+=(const CHR* CString) {
	Cat(CString);
	return *this;
}

STRING& STRING::operator+=(const STRING& OtherString) {
	Cat(OtherString);
	return *this;
}

INT STRING::operator==(const STRING& OtherString) const {
	return Equals(OtherString);
}

INT STRING::operator==(const CHR* CString) const {
	return Equals(CString);
}

INT STRING::operator!=(const STRING& OtherString) const {
	return !(Equals(OtherString));
}

INT STRING::operator!=(const CHR* CString) const {
	return !(Equals(CString));
}

INT STRING::operator^=(const STRING& OtherString) const {
	return CaseEquals(OtherString);
}

INT STRING::operator^=(const CHR* CString) const {
	return CaseEquals(CString);
}

INT STRING::Equals(const STRING& OtherString) const {
	if (Length != OtherString.Length) {
		return 0;
	}
	return ( memcmp(Buffer, OtherString.Buffer, Length) == 0 );
}

INT STRING::Equals(const CHR* CString) const {
  //this is bogus. 
  if (Length == strlen(CString))
    return ( memcmp(Buffer, CString, Length) == 0 );
  else
    return 0;
}

/*
INT STRING::GreaterThan(const STRING& OtherString) const {
  STRINGINDEX SmallerLength;
  if (Length > OtherString.Length) {
    SmallerLength = OtherString.Length;
  } else {
    SmallerLength = Length;
  }
  INT4 x;
  for (x=0; x<SmallerLength; x++) {
    if (Buffer[x])
    }
}
*/

INT STRING::CaseEquals(const STRING& OtherString) const {
  if (Length != OtherString.Length) {
    return 0;
  }
  INT4 x;
  for (x=0; x<Length; x++) {
    if (toupper(Buffer[x]) != toupper(OtherString.Buffer[x])) {
      return 0;
    }
  }
  return 1;
}

INT STRING::CaseEquals(const CHR* CString) const {
  const CHR *p1 = CString;
  const UCHR *p2 = Buffer;
  INT Match = 1;
  INT4 x;
  for (x = 0; ( (x < Length) && *p1 ); x++) {
    if ( (toupper(*p1) - toupper(*p2)) != 0) {
      Match = 0;
      break;
    }
    else {
      p1++; p2++;
    }
  }
  if (Match) {
    if ( (x == Length) && !*p1)
      return 1;
  }
  
  return 0;
}

void STRING::Print() const {
  cout.write(Buffer, Length);
}

void STRING::Print(PFILE FilePointer) const {
  STRINGINDEX x;
  for (x=0; x<Length; x++)
    fprintf(FilePointer, "%c", Buffer[x]);
}

// can this be const STRING& ?
ostream& operator<<(ostream& os, const STRING& str) {
	os.write(str.Buffer, str.Length);
	return os;
}

#define MaxIntLen 256
INT STRING::GetInt() const {
  INT x;
  CHR s[MaxIntLen];

  if (Length >= MaxIntLen) {
    CHR* ps = NewCString();
    x = atoi(ps);
    delete [] ps;
  } else {
    CHR * pS;
    pS = s;
    strncpy(pS,(char const *)Buffer,(size_t)Length);
    pS[Length] = '\0';
    x=atoi(pS);
  }

  return x;
}

/*
INT STRING::GetInt() const {
  CHR s[MaxIntLen];
  GetCString(s, MaxIntLen);
  INT x = atoi(s);
  return x;
}
*/

DOUBLE STRING::GetFloat() const {
  DOUBLE x;
  CHR s[MaxIntLen];
  
  if (Length >= MaxIntLen) {
    CHR* ps = NewCString();
    x = atof(ps);
    delete [] ps;
  } else {
    CHR * pS;
    pS = s;
    strncpy(pS,(char const *)Buffer,(size_t)Length);
    pS[Length] = '\0';
    x=atof(pS);
  }
  
  return x;
}

GDT_BOOLEAN STRING::FGet(PFILE FilePointer, const STRINGINDEX MaxCharacters) {
  CHR* pc = new CHR[MaxCharacters+2];
  CHR* p;
  GDT_BOOLEAN Ok;
  if (fgets(pc, MaxCharacters+1, FilePointer)) {
    p = pc + strlen(pc) - 1;
    while ( (p >= pc) && ( (*p == '\n') || (*p == '\r') ) ) {
      *(p--) = '\0';
    }
    *this = pc;
    Ok = GDT_TRUE;
  } else {
    *this = "";
    Ok = GDT_FALSE;
  }
  delete [] pc;
  return Ok;
}

STRINGINDEX STRING::GetLength() const {
	return Length;
}

UCHR STRING::GetChr(STRINGINDEX Index) const {
	if ( (Index > 0) && (Index <= Length) )
		return Buffer[Index-1];
	else
		return 0;	// generate ER
}

void STRING::SetChr(const STRINGINDEX Index, const UCHR NewChr) {
	if (Index > 0) {
		if (Index > Length) {
			INT4 x;
			for (x=1; x<(Index-Length); x++) {
				Cat(' ');
			}
			Cat(NewChr);
		} else {
			Buffer[Index-1] = NewChr;
		}
	}
}

void STRING::Cat(const UCHR Character) {
  if (BufferSize > Length) {
    Buffer[Length] = Character;
#ifdef METRICS
    NumTimesExpanded++; //this is usually done in StrBuffAlloc
    TotalStringExpns++; //ditto
#endif
  }
  else {
    UCHR *Temp = new UCHR[Length];
    memcpy(Temp, Buffer, Length);
    StrBuffAlloc(Length + 1);
    memcpy(Buffer, Temp, Length);
    Buffer[Length] = Character;
    delete [] Temp;
  }
  Length++;
#ifdef METRICS
  TotalStringLength++;
  TotalNumStrings++;
#endif
}

void STRING::Cat(const CHR* CString, INT CLength) {
  STRINGINDEX StringLength;
  if (CLength < 0)
    StringLength = strlen(CString);
  else
    StringLength = CLength; 

  if (StringLength == 0) {
    return;
  }
  if (BufferSize >= (StringLength + Length)) {
    memcpy(Buffer + Length, CString, StringLength);
#ifdef METRICS
    NumTimesExpanded++; //this is usually done in StrBuffAlloc
    TotalStringExpns += StringLength; //ditto
#endif
  }
  else {
    UCHR* Temp;
    if(Length>0){
      Temp = new UCHR[Length];
      memcpy(Temp, Buffer, Length);
    }
    StrBuffAlloc(Length + StringLength);
    if(Length>0)
      memcpy(Buffer, Temp, Length);
    memcpy(Buffer + Length, CString, StringLength);
    if(Length>0)
      delete [] Temp;
  }
  Length += StringLength;
#ifdef METRICS
  TotalStringLength += Length;
  TotalNumStrings++;
#endif
}

void STRING::Cat(const STRING& OtherString) {
  if (OtherString.Length == 0)
    return;
  else {
    Cat((CHR *)OtherString.Buffer, OtherString.Length);
/*
	  PCHR Temp;
	  Temp = OtherString.NewCString();
	  Cat(Temp);
	  delete Temp;
	  //This is buggy - the NULL is past the end of the string
//		OtherString.Buffer[OtherString.Length] = '\0';
//		Cat((CHR *)OtherString.Buffer);
*/
  }
}

void STRING::Insert(const INT InsertionPoint, const STRING& OtherString) {
	STRINGINDEX StringLength = OtherString.Length;
	if (StringLength == 0) {
		return;
	}
	if (Length == 0)  {
		StrBuffAlloc(StringLength);
		memcpy(Buffer, OtherString.Buffer, StringLength);
	}
	else if (BufferSize >= (StringLength + Length)) {
		INT RemnantSize = Length - InsertionPoint + 1;
		UCHR *EndFirstBit = Buffer + InsertionPoint - 1;
		UCHR* Remnant = new UCHR[RemnantSize];
		memcpy(Remnant, EndFirstBit, RemnantSize);
		memcpy(EndFirstBit, OtherString.Buffer, StringLength);
		memcpy(EndFirstBit + StringLength, Remnant, RemnantSize);
		delete [] Remnant;
#ifdef METRICS
		NumTimesExpanded++;
		TotalStringExpns += StringLength;
#endif
	}
	else {
		UCHR* Temp = new UCHR[Length];
		//save the current string
		memcpy(Temp, Buffer, Length);
		StrBuffAlloc(Length + StringLength);

		//index of the rest of the string
		INT ToCopy = InsertionPoint - 1;

		//pointer to the rest of the string
		UCHR *EndBit = Buffer + InsertionPoint - 1;

		//how many characters remain
		INT RemnantSize = Length - InsertionPoint + 1;
		memcpy(Buffer, Temp, ToCopy);
		memcpy(Buffer+ToCopy, OtherString.Buffer, StringLength);
		memcpy(EndBit + StringLength, Temp+ToCopy, RemnantSize);
		delete [] Temp;
	}
	Length += StringLength;
#ifdef METRICS
	NumTimesCopied++;
	TotalNumStrings++;
	TotalStringLength += Length;
#endif
}

STRINGINDEX STRING::Search(const CHR* CString) const {
  INT4 x, y, z, SLen, Match;
  SLen = strlen(CString);
  z = Length - SLen + 1;
  for (x=0; x<z; x++) {
    if (Buffer[x] == (UCHR)(*CString)) {
      Match = 1;
      for (y=1; y<SLen; y++) {
	if (Buffer[x+y] != (UCHR)(CString[y])) {
	  Match = 0;
	}
      }
      if (Match) {
	return (x+1);
      }
    }
  }
  return 0;
}

STRINGINDEX STRING::Search(const UCHR Character) const {
  STRINGINDEX x;
  if (Length == 0)
    return 0;
  x = 0;
  do {
    if (Buffer[x] == Character)
      return (x+1);
    x++;
  } while (x < Length);
  return 0;
}

STRINGINDEX STRING::SearchReverse(const CHR* CString) const {
  STRINGINDEX x;
  INT4 n = strlen(CString);
  if (n > Length)
    return 0;
  if (n == 0)
    return 0;	// Generate warning ER?
  x = Length - n + 1;
  if (x > 0)
    do {
      x--;
      if (StrNCmp((UCHR*)CString, Buffer+x, n) == 0)
	return (x + 1);
    } while (x > 0);
  return 0;
}

STRINGINDEX STRING::SearchReverse(const UCHR Character) const {
  STRINGINDEX x;
  if (Length == 0)
    return 0;
  x = Length;
  if (x > 0)
    do {
      x--;
      if (Buffer[x] == Character)
	return (x+1);
    } while (x > 0);
  return 0;
}

INT STRING::Replace(const CHR* CStringSearch, const CHR* CStringReplace) {
  STRING NewString, S;
  STRINGINDEX Position;
  INT4 CSLen = strlen(CStringSearch);
  INT Count = 0;
  while ( (Position=Search(CStringSearch)) != 0) {
    Count++;
    S = *this;
    S.EraseAfter(Position-1);
    NewString += S;
    NewString += CStringReplace;
    EraseBefore(Position + CSLen);
  }
  NewString += *this;
  *this = NewString;
  return Count;
}

INT STRING::Replace(const CHR* CStringSearch, const STRING& CStringReplace) {
  STRING NewString, S;
  STRINGINDEX Position;
  INT4 CSLen = strlen(CStringSearch);
  INT Count = 0;
  while ( (Position=Search(CStringSearch)) != 0) {
    Count++;
    S = *this;
    S.EraseAfter(Position-1);
    NewString += S;
    NewString += CStringReplace;
    EraseBefore(Position + CSLen);
  }
  NewString += *this;
  *this = NewString;
  return Count;
}
 
void STRING::EraseBefore(const STRINGINDEX Index) {
  if ( (Index <= 1) || (Length == 0) ) {
    return;
  }
  if (Index > Length) {
    Length = 0;
    return;
  }
  UCHR* Temp;
  INT4 CharsLeft = Length - Index + 1;
  Temp = new UCHR[CharsLeft];
  memcpy(Temp, Buffer + Index - 1, CharsLeft);
  memcpy(Buffer, Temp, CharsLeft);
#ifdef METRICS
  TotalNumStrings++;
  TotalStringLength += Length;
  NumTimesCopied++;
#endif
  delete [] Temp;
  Length = CharsLeft;
}

void STRING::EraseAfter(const STRINGINDEX Index) {
  if (Length == 0)
    return;
  if (Index < 1)
    Length = 0;
  else
    Length = Index;
}

void STRING::UpperCase() {
  STRINGINDEX x;
  for (x=0; x<Length; x++) {
    Buffer[x] = toupper(Buffer[x]);
  }
}

void STRING::GetCString(CHR* CStringBuffer, const INT BufferSize) const {
  STRINGINDEX ShortLength = BufferSize - 1;
  if (Length < ShortLength)
    ShortLength = Length;
  if (Buffer)
    memcpy(CStringBuffer, Buffer, ShortLength);
  CStringBuffer[ShortLength] = '\0';
}

CHR* STRING::NewCString() const {
  return ((CHR*)NewUCString());
}

UCHR* STRING::NewUCString() const {
  UCHR* p = new UCHR[Length+1];
  if (Buffer) {
    memcpy(p, Buffer, Length);
  }
  p[Length] = '\0';
  return p;
}

void STRING::WriteFile(const STRING& FileName) const {
	PFILE fp;
	fp = fopen(FileName, "wb");
	if (!fp) {
		perror(FileName);
		exit(1);
	}
	else {
		if (Buffer) {
			fwrite(Buffer, 1, Length, fp);
		}
		fclose(fp);
	}
}

void STRING::ReadFile(const STRING& FileName) {
	PFILE fp;
	if (Buffer)
		delete [] Buffer;
	fp = fopen(FileName, "rb");
	if (fp) {
		fseek(fp, 0, 2);
		Length = ftell(fp);
		if (Length) {
			fseek(fp, 0, 0);
			Buffer = new UCHR[Length];
			fread(Buffer, 1, Length, fp);
		} else {
			Buffer = 0;
		}
		fclose(fp);
	} else {
		Buffer = 0;
		Length = 0;
	}
}

GDT_BOOLEAN STRING::IsNumber() {
  STRINGINDEX x;
  for (x=0; x<Length; x++) {
    if (!((isdigit(Buffer[x])) 
	  || (Buffer[x] == '.')
	  || (Buffer[x] == '-')
	  || (Buffer[x] == '+')))
      return(GDT_FALSE);
  }
  return(GDT_TRUE);
}

GDT_BOOLEAN STRING::IsPrint() {
  STRINGINDEX x;
  for (x=0; x<Length; x++) {
    if (!isprint(Buffer[x]))
      return(GDT_FALSE);
  }
  return(GDT_TRUE);
}

void STRING::MakePrintable() {
  STRINGINDEX x;
  for (x=0; x<Length; x++) {
    if (!isprint(Buffer[x]))
      Buffer[x] = ' ';
  }
  return;
}

STRING::~STRING() {
  if (BufferSize)
//    delete [] Buffer;
    delete Buffer;
}

PFILE fopen(const STRING& FileName, const CHR* Type) {
  PFILE fp;
  CHR* fn = FileName.NewCString();
  fp = fopen(fn, Type);
  delete [] fn;
  return fp;
}

INT StrUnlink(const STRING& FileName) {
	INT ReturnCode;
	CHR* fn = FileName.NewCString();
	ReturnCode = unlink(fn);
	delete [] fn;
	return ReturnCode;
}

// General C String functions

// Thanks to Edward C. Zimmermann for fixes to these functions.

INT StrCaseCmp(const CHR* s1, const CHR* s2) {
	return StrCaseCmp((UCHR*)s1, (UCHR*)s2);
}

INT StrCaseCmp(const UCHR* s1, const UCHR* s2) {
	const UCHR* p1;
	const UCHR* p2;
	INT diff;
	p1 = s1;
	p2 = s2;
	while ((diff=(toupper(*p1) - toupper(*p2))) == 0) {
		if ( (*p1 == '\0') && (*p2 == '\0') ) {
			break;
		}
		p1++;
		p2++;
	}
	return diff;
}

INT StrNCaseCmp(const CHR* s1, const CHR* s2, const INT4 n) {
	return StrNCaseCmp((UCHR*)s1, (UCHR*)s2, n);
}

INT StrNCaseCmp(const UCHR* s1, const UCHR* s2, const INT4 n) {
	const UCHR* p1;
	const UCHR* p2;
	INT diff;
	INT x = 0;
	p1 = s1;
	p2 = s2;
	while ((diff=(toupper(*p1) - toupper(*p2))) == 0) {
		x++;
		if ( (x >= n) || ((*p1 == '\0') && (*p2 == '\0')) ) {
			break;
		}
		p1++;
		p2++;
	}
	return diff;
}

INT StrNCmp(const CHR* s1, const CHR* s2, const INT4 n) {
	return StrNCmp((UCHR*)s1, (UCHR*)s2, n);
}

INT StrNCmp(const UCHR* s1, const UCHR* s2, const INT4 n) {
  const UCHR* p1;
  const UCHR* p2;
  INT diff;
  INT4 x = 0;
  p1 = s1;
  p2 = s2;
  while ((diff=(*p1 - *p2)) == 0) {
    x++;
    if ( (x >= n) || ((*p1 == '\0') && (*p2 == '\0')) ) {
      break;
    }
    p1++;
    p2++;
  }
  return diff;
}

void perror(const STRING &s) {
	CHR *mesg;
	mesg = s.NewCString();
	perror(mesg);
	delete mesg;
}

INT rename(const STRING From, const STRING To) {
  CHR *from = From.NewCString();
  CHR *to = To.NewCString();

#if defined(_MSDOS) || defined(_WIN32)

  /*
   * MSDOS / WIN32 rename doesn't remove an existing file so
   * we have to do it ourselves.
   */

  remove(to);
#endif

  INT val = rename(from, to);
  delete [] from;
  delete [] to;
  return val;
}
