/*

File:        oneline.cxx
Version:     1
Description: class ONELINE - index documents one line long (like phonebooks)
Author:      Erik Scott, Scott Technologies, Inc.
*/

#include <ctype.h>
#include "oneline.hxx"

ONELINE::ONELINE(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
}

void ONELINE::ParseRecords(const RECORD& FileRecord) {

  GPTYPE Start = 0;
  GPTYPE Position = 0;
  GPTYPE Pos = 0;

  STRING Fn;
  FileRecord.GetFullFileName (&Fn);
  PFILE Fp = fopen (Fn, "rb");
  if (!Fp)
    {
      cout << "Could not access '" << Fn << "'\n";
      return;			// File not accessed

    }



  RECORD Record;
  STRING s;
  FileRecord.GetPathName(&s);
  Record.SetPathName( s );

  FileRecord.GetFileName(&s);
  Record.SetFileName( s );

  FileRecord.GetDocumentType(&s);
  Record.SetDocumentType ( s );
  
  int ci = 0;
  while (ci != EOF) {
    for (; (ci != '\n') && (ci != EOF); ci=fgetc(Fp), Position = Position + 1) ;
    if (Start != Position) {
      Record.SetRecordStart(Start);
      if (ci != EOF) 
         Pos = Position-1;
      else
         Pos = Position-2;
      Record.SetRecordEnd(Pos);
      Db->DocTypeAddRecord(Record);
      }
    Start=Position;
    if (ci=='\n') ci=0;// save an EOF, but hide a newline so it will loop again
    }
fclose(Fp);

}

ONELINE::~ONELINE() {
}
