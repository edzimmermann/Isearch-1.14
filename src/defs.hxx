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
File:		defs.hxx
Version:	1.00
Description:	General definitions
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DEFS_HXX
#define DEFS_HXX

#ifdef UNIX
#include <unistd.h>
#endif
#include "gdt.h"

const PCHR IsearchDefaultDbName = "ISEARCH";

const PCHR IsearchVersion = "1.14";
const INT IsearchMagicNumber = 6;

typedef UINT4 GPTYPE;
typedef GPTYPE* PGPTYPE;

const PCHR Bib1AttributeSet = "1.2.840.10003.3.1";
const PCHR StasAttributeSet = "1.2.840.10003.3.1000.6.1";

const PCHR IsearchAttributeSet = "1.2.840.10003.3.1000.34.1";
const INT IsearchFieldAttr = 1;
const INT IsearchWeightAttr = 2;

// Record Syntaxes
const PCHR SutrsRecordSyntax = "1.2.840.10003.5.101";
const PCHR UsmarcRecordSyntax = "1.2.840.10003.5.10";
const PCHR HtmlRecordSyntax = "1.2.840.10003.5.1000.34.1";

// Op Types
const INT TypeOperand = 1;
const INT TypeOperator = 2;
// Operand Types
const INT TypeTerm = 1;
const INT TypeRset = 2;
// Operator Types
const INT OperatorOr = 1;
const INT OperatorAnd = 2;
const INT OperatorAndNot = 3;
const INT OperatorNear = 15;
const INT OperatorCharProx = 16;

const INT DocumentKeySize = 16;
const INT DocumentTypeSize = 16;
const INT DocFileNameSize = 80;
const INT DocPathNameSize = 128;
const INT StringCompLength = 64;	// how many characters to compare

const INT IndexingStatusParsingDocument = 1;
const INT IndexingStatusIndexing = 2;
const INT IndexingStatusMerging = 3;
const INT IndexingStatusParsingFiles = 4;

const PCHR DbExtDbInfo = ".dbi";
const PCHR DbExtIndex = ".inx";
const PCHR DbExtMdt = ".mdt";
const PCHR DbExtMdtKeyIndex = ".mdk";
const PCHR DbExtMdtGpIndex = ".mdg";
const PCHR DbExtDfd = ".dfd";
const PCHR DbExtIndexQueue1 = ".iq1";
const PCHR DbExtIndexQueue2 = ".iq2";
const PCHR DbExtTemp = ".tmp";
const PCHR DbExtDict = ".dic";
const PCHR DbExtSparse = ".spr";
const PCHR DbExtCentroid = ".cen";

#ifdef PLATFORM_MSVC
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#define COUT cout

#endif
