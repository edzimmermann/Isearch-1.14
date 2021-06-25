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
File:		soundex.cxx
Version:	1.00
Description:	Soundex support for STRING class
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include "soundex.hxx"

void SoundexEncode(const STRING& EnglishWord, PSTRING StringBuffer) {
	STRING s1, s2;
	s1 = EnglishWord;
	s1.UpperCase();
	INT x, y;
	y = s1.GetLength();
	CHR ch, ch2;
	s2 += s1.GetChr(1);
	for (x=2; x<=y; x++) {
		ch = s1.GetChr(x);
		switch (ch) {
			case ' ':
			case 'A':
			case 'E':
			case 'H':
			case 'I':
			case 'O':
			case 'U':
			case 'W':
			case 'Y':
				ch2 = '0';
				break;
			case 'B':
			case 'F':
			case 'P':
			case 'V':
				ch2 = '1';
				break;
			case 'C':
			case 'G':
			case 'J':
			case 'K':
			case 'Q':
			case 'S':
			case 'X':
			case 'Z':
				ch2 = '2';
				break;
			case 'D':
			case 'T':
				ch2 = '3';
				break;
			case 'L':
				ch2 = '4';
				break;
			case 'M':
			case 'N':
				ch2 = '5';
				break;
			case 'R':
				ch2 = '6';
				break;
			default:
				ch2 = ch;
				break;
		}
		s2 += ch2;
	}
	s1 = "";
	y = s2.GetLength();
	for (x=1; x<=y; x++) {
		ch = s2.GetChr(x);
		if (ch != '0') {
			s1 += ch;
		}
	}
	s2 = "";
	ch = '\0';
	y = s1.GetLength();
	for (x=1; x<=y; x++) {
		ch2 = s1.GetChr(x);
		if (ch2 != ch) {
			s2 += ch2;
		}
		ch = ch2;
	}
	if ( (y=s2.GetLength()) > 4 ) {
		s2.EraseAfter(4);
	} else {
		y = 4 - y;
		for (x=1; x<=y; x++) {
			s2 += '0';
		}
	}
	*StringBuffer = s2;
}
