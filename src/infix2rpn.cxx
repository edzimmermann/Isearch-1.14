
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


#include "infix2rpn.hxx"
#include "gdt.h"
#include "string.hxx"
#include "strlist.hxx"
#include "strstack.hxx"

INFIX2RPN::INFIX2RPN()  {
//nothin.
}

INFIX2RPN::INFIX2RPN(const STRING &StrInput, STRING *StrOutput) {
        Parse(StrInput, StrOutput);
}


void INFIX2RPN::Parse(const STRING &StrInput, STRING *StrOutput) {

        STRSTACK TheStack;
        STRSTACK OperandStack;
        TOKENGEN *TokenList;
        STRING token, TmpVal;

        TokenList = new TOKENGEN(StrInput);

        *StrOutput = "";
        TermsWithNoOps = 0;
        GDT_BOOLEAN LastTokenWasTerm = GDT_FALSE; 

        INT Size = TokenList->GetTotalEntries();
        for (INT i = 1; i <= Size; i++) {
                TokenList->GetEntry(i, &token);

                if (token == "(") {
                        TheStack.Push("(");
                }
                else if (token == ")") {
                        TmpVal = "";
                        while (TheStack.Examine(&TmpVal) && TmpVal != "(") {
                                TheStack.Pop(&TmpVal);
                                *StrOutput +=  TmpVal;
                                *StrOutput += " ";
                                TermsWithNoOps--;
                        }
                        TheStack.Pop(&TmpVal); //get rid of left paren
                }
                else if ( (token ^= "AND") || (token == "&&") ) {
                        LastTokenWasTerm = GDT_FALSE;
                        ProcessOp(BoolAND, &TheStack, StrOutput);
                }
                else if ( (token ^= "OR") || (token == "||") ) {
                        LastTokenWasTerm = GDT_FALSE;
                        ProcessOp(BoolOR, &TheStack, StrOutput);
                }
                else if ( (token ^= "ANDNOT") || (token == "&!") ) {
                        LastTokenWasTerm = GDT_FALSE;
                        ProcessOp(BoolNOT, &TheStack, StrOutput);
                }
                else if ( token ^= "NEAR" ) {
                        LastTokenWasTerm = GDT_FALSE;
                        ProcessOp(ProxNEAR, &TheStack, StrOutput);
                }
#ifdef UNARYNOT
                else if ( (token ^= "NOT") || (token ^= "!") ) {
                        ProcessOp(UnNOT, &TheStack, StrOutput);
                }
#endif
                else {
                        if (!LastTokenWasTerm) {
                                *StrOutput += token;
                                *StrOutput += " ";
                                OperandStack.Push(token);
                                TermsWithNoOps++;
                                LastTokenWasTerm = GDT_TRUE;
                        }
                        else {
                                //two terms in a row is invalid in infix.
                                //so we set TermsWithNoOps impossibly high and bail.
                                TermsWithNoOps = 6969;
                                STRING Error, TmpOper;
                                Error = "Two operands: ";
                                OperandStack.Pop(&TmpOper);
                                Error +=  TmpOper;
                                Error += ", ";
                                Error += token;
                                Error += ", without an operator";
                                RegisterError(Error);
                                break;
                        }
                }
        }
        delete TokenList;
        TmpVal = "";
        while (! TheStack.IsEmpty()) {
                TheStack.Pop(&TmpVal);
                TermsWithNoOps--;
                *StrOutput += TmpVal;
                if (!TheStack.IsEmpty())
                        *StrOutput += " ";
        }
}

GDT_BOOLEAN INFIX2RPN::InputParsedOK(void) {
        //with no unary not or other weird operators, 
        //you should have n-1 operators for n terms
        return (TermsWithNoOps==1?GDT_TRUE:GDT_FALSE);
}

void INFIX2RPN::RegisterError(const STRING &Error) {
        ErrorMessage = Error;
}

GDT_BOOLEAN INFIX2RPN::GetErrorMessage(STRING *Error) const {
        if (ErrorMessage != "") {
                *Error = ErrorMessage;
                return GDT_TRUE;
        }
        else
                return GDT_FALSE;
}






void INFIX2RPN::ProcessOp(const operators op, STRSTACK *TheStack,  STRING *result) {
STRING TmpVal = "";
#ifdef UNARYNOT
        //I'll need a special treatment for TermsWithNoOps for unarynot (if ever 
        // implemented).
        if (op != BoolNOT) {
#endif
                while ( (TheStack->Examine(&TmpVal)) && (TmpVal != "(") ) {
                        TheStack->Pop(&TmpVal);
                        *result += StandardizeOpName(TmpVal);
                        *result += " ";
                        TermsWithNoOps--;
                }
#ifdef UNARYNOT 
        }
#endif
        TheStack->Push(op2string(op));
}

//standardizes the various possible representations of
//the various operators.
CHR *INFIX2RPN::StandardizeOpName(const STRING op) {
        if ( (op ^= "AND") || (op == "&&") )
                return "AND";
        else if ( (op ^= "OR") || (op == "||") )
                return "OR";
        else if ( (op ^= "ANDNOT") || (op == "&!") )
                return "ANDNOT";
        else if ( op ^= "NEAR" )
                return "NEAR";
#ifdef UNARYNOT
        else if ( (op ^= "NOT") || (op == "!") )
                return "NOT";
#endif

	return "";
}


//converts the internal operator token name to a standard string
CHR *INFIX2RPN::op2string(const operators op) {
        switch(op) {
                case LeftParen:
                case NOP:
                        //shouldn't happen, but makes gcc -WALL happy.
                        cerr << "LeftParen || NOP?" << endl;
                        break;
                case BoolOR:
                        return "OR";
                        break;
                case BoolAND:
                        return "AND";
                        break;
                case BoolNOT:
                        return "ANDNOT";
                        break;
                case ProxNEAR:
                        return "NEAR";
                        break;
#ifdef UNARYNOT
                case UnNOT:
                        return "NOT";
                        break;
#endif
        }
//        return NULL;
        return "";
}

