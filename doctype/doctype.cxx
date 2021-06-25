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
File:		doctype.cxx
Version:	1.01
Description:	Class DOCTYPE - Document Type
Author:		Nassib Nassar, nrn@cnidr.org
Modifications:  Archie Warnock (warnock@clark.net)
@@@*/
#include <string.h>
#include <ctype.h>
#include "doctype.hxx"

DOCTYPE::DOCTYPE(IDBOBJ* DbParent) {
	Db = DbParent;
}

void DOCTYPE::BeforeIndexing() {
}

void DOCTYPE::LoadFieldTable() {
  STRLIST StrList;
  STRING FieldTypeFilename;
  Db->GetDocTypeOptions(&StrList);
  StrList.GetValue("FIELDTYPE", &FieldTypeFilename);

  if (FieldTypeFilename.GetLength() == 0) {
    cout << "No fieldtype file specified.  Assuming all fields are text.";
    cout << endl;
    cout << "Make sure you use the correct doctype option:" << endl;
    cout << endl;
    cout << "    -o fieldtype=<filename>" << endl;
    return;
  }

  STRING Field_and_Type;
  PCHR b, pBuf;
  INT4 RecStart, RecEnd, len, ActualLength;
  PFILE fp = fopen(FieldTypeFilename, "r");

  // Let's bring the entire file into memory
  if (!fp) {
    cout << "Specified fieldtype file not found.  Assuming all fields are text.";
    cout << endl;
    cout << "Make sure you use the correct doctype option:" << endl;
    cout << endl;
    cout << "    -o fieldtype=<filename>" << endl;
    return;
  }
  fseek(fp, 0, 2);
  RecStart = 0;
  RecEnd = ftell(fp);

  fseek(fp, RecStart, 0);
  len = RecEnd - RecStart;
  b = new CHR[len + 1];
  ActualLength = fread(b, 1, len, fp);
  b[ActualLength] = '\0';
  fclose(fp);
  pBuf = strtok(b,"\n");

  do {
    Field_and_Type = pBuf;
    Db->FieldTypes.AddEntry(Field_and_Type);
  } while ( (pBuf = strtok(NULL,"\n")) );

  delete [] b;
}

void DOCTYPE::AddFieldDefs() {
}

void DOCTYPE::ParseRecords(const RECORD& FileRecord) {
	Db->DocTypeAddRecord(FileRecord);
}

GPTYPE DOCTYPE::ParseWords(
	      //@ManMemo: Pointer to document text buffer.
	      CHR* DataBuffer,
              //@ManMemo: Length of document text buffer in # of characters.
              INT DataLength,
              //@ManMemo: Offset that must be added to all GP positions because GP space is shared with other documents.
              INT DataOffset,
              //@ManMemo: Pointer to document (word-beginning) GP buffer.
              GPTYPE* GpBuffer,
              //@ManMemo: Length of document GP buffer in # of GPTYPE elements, i.e. sizeof(GPTYPE).
              INT GpLength
              ) {     // This code began life as INDEX::BuildGpList().
  INT GpListSize = 0;
  INT Position = 0;
  while (Position < DataLength) {
    while ( (Position < DataLength) &&
	   (!isalnum(DataBuffer[Position])) ) {
      Position++;
    }
    if ( (Position < DataLength) &&
	(!(Db->IsStopWord(DataBuffer + Position,
			  DataLength - Position))) ) {
      if (GpListSize >= GpLength) {
         cout << "GpListSize >= GpLength" << endl;
         exit(1);
      }
      GpBuffer[GpListSize++] = DataOffset + Position;
    }
    while ( (Position < DataLength) &&
	   (isalnum(DataBuffer[Position])) ) {
      Position++;
    }
  }
  return GpListSize;
}     // Return # of GP's added to GpBuffer

/*
void DOCTYPE::SelectRegions(const RECORD& Record, FCT* FctPtr) {
	// Select the entire document as one region.
	FC Fc;
	qFc.SetFieldStart(0);
	Fc.SetFieldEnd(Record.GetRecordStart() - Record.GetRecordEnd());
	FctPtr->AddEntry(Fc);
}
*/

void DOCTYPE::ParseFields(RECORD* NewRecordPtr) {
}

DOUBLE DOCTYPE::ParseDateSingle(const PCHR Buffer){
  STRING Hold;
  Hold = Buffer;
  if (Hold.IsNumber())
    return(Hold.GetFloat());
  else
    return 0;
}
 
void DOCTYPE::ParseDateRange(const PCHR Buffer, DOUBLE* fStart, 
                           DOUBLE* fEnd){
}

DOUBLE DOCTYPE::ParseNumeric(const PCHR Buffer){
  STRING Hold;
  Hold = Buffer;
  if (Hold.IsNumber())
    return(Hold.GetFloat());
  else
    return 0;
}

void DOCTYPE::ParseGPoly(const PCHR Buffer){
}

void DOCTYPE::AfterIndexing() {
}

void DOCTYPE::BeforeSearching(SQUERY* SearchQueryPtr) {
}

//void DOCTYPE::AfterSearching(RSET* ResultSetPtr) {
PRSET DOCTYPE::AfterSearching(RSET* ResultSetPtr) {
  return ResultSetPtr;
}

void DOCTYPE::BeforeRset(const STRING& RecordSyntax) {
}

void DOCTYPE::AfterRset(const STRING& RecordSyntax) {
}

void DOCTYPE::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		STRING* StringBufferPtr) {
  *StringBufferPtr = "";
  if (ElementSet.Equals("F")) {
    ResultRecord.GetRecordData(StringBufferPtr);
    return;
  }
  if (Db->DfdtGetTotalEntries() == 0) {
    return;
  }
  STRING FieldName;
  if (ElementSet.Equals("B")) {
    DFD Dfd;
    Db->DfdtGetEntry(1, &Dfd);
    Dfd.GetFieldName(&FieldName);
  } else {
    FieldName = ElementSet;
  }
  STRLIST Strlist;
  GDT_BOOLEAN Status;
  Status = Db->GetFieldData(ResultRecord, FieldName, &Strlist);
  if (Status)
    Strlist.Join("\n", StringBufferPtr);
  else
    *StringBufferPtr = "";

}

void DOCTYPE::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		      const STRING& RecordSyntax, STRING* StringBufferPtr) {
  Present(ResultRecord, ElementSet, StringBufferPtr);
}

DOCTYPE::~DOCTYPE() {
}
