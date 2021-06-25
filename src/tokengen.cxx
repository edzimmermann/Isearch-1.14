
/************************************************************************
Copyright Notice

Copyright (c) MCNC, Clearinghouse for Networked Information Discovery and
Retrieval, 1995. 

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

#include <ctype.h>
#include "tokengen.hxx"
#include "string.hxx"
#include "strlist.hxx"
#include "gdt.h"

SIZE_T TOKENGEN::GetTotalEntries(void) {
        DoParse();
        return TokenList.GetTotalEntries();
}

void TOKENGEN::SetQuoteStripping(GDT_BOOLEAN DoItOrNot) {
        DoStripQuotes = DoItOrNot;
        HaveParsed = GDT_FALSE;
        DoParse();
}

TOKENGEN::TOKENGEN(const STRING &InString) 
        : DoStripQuotes (GDT_FALSE), HaveParsed(GDT_FALSE)
{
        InCharP = InString.NewCString();
}

TOKENGEN::~TOKENGEN() {
        delete InCharP;
}

void TOKENGEN::DoParse(void) {

        if (HaveParsed)
                return;

        char *Next;
        STRING TokenStr;

        Next = InCharP;

        INT i;
        for ( i=1; (Next = nexttoken(Next, &TokenStr)); i++ )
                TokenList.SetEntry(i, TokenStr);
        
        HaveParsed = GDT_TRUE;
}


void TOKENGEN::GetEntry(const SIZE_T Index, STRING* StringEntry) {
        DoParse();
        TokenList.GetEntry(Index, StringEntry);
}

char *TOKENGEN::nexttoken(char *input, STRING *token) {

        int istoken=0;
        *token = "";

        while(*input) {
                if ( isspace(*input) ) {
                        if (!istoken) {
                                input++;
                                continue;
                        }
                        else {
                                input++;
                                return input;
                        }
                }

#ifdef UNARYNOT
                if ( (*input ==  '(') || (*input == ')') ) {
#else
                if ( (*input ==  '(') || (*input == ')') || (*input == '!') ) {
#endif
                        if (!istoken) {
                                //NOTE: STRING = char. doesn't work.
                                *token += *input;
                                input++;
                                return input;
                        }
                        else {
                                return input;
                        }
                }

                if ( *input == '"') {
                        //quoted strings are literals and should be returned as one token
                        CHR *BeginQuote;
                        if (!DoStripQuotes)
                                BeginQuote = input;
                        else
                                BeginQuote = ++input;
                                
                        do {
                                *token += *input;
                                input++;
                        } while (*input != '"' && *input);
                        
                        if (*input == '"') {
                                if (!DoStripQuotes)
                                        *token += *input; 
                                input++;
                                istoken = 1;
                                //special case for weighted terms
                                if ( *input == ':' && isdigit(*(input+1)) ) {
                                        do {
                                                *token += *input; 
                                                input++;
                                        } while (isdigit(*input)); 
                                }
                        } 
                        else {
                                //if quotes aren't matched, parse it
                                //as part of a single term.
                                input = ++BeginQuote;
                                token->EraseAfter(token->SearchReverse('"'));
                        }
                        continue;
                }

                if ( *input == '&' || *input == '|' ) {
                //looks like a C-style operator
                        if ( (*input == *(input+1)) || 
                                ( *input == '&' && *(input+1) == '!') ) {
                                //it IS a C-style operator
                                if (!istoken) {
                                        //if we're looking for a token, grab it
                                        //again, STRING = char, doesn't work.
                                        *token += *input;
                                        input++; 
                                        *token += *input;
                                        input++;
                                        return input;
                                }
                                else {
                                        return input;
                                }
                        }
                        else {
                                //must be part of something else
                                istoken = 1;
                                *token += *input;
                                input++;
                                continue;
                        }
                }

                istoken = 1;
                *token += *input;
                input++;
                continue;
        }

        if (istoken)
                return input;
        else 
                return NULL;
}

