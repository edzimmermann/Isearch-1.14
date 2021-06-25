/*

File:        para.cxx
Version:     1
Description: class PARA - index documents by paragraphs 
Author:      Erik Scott, Scott Technologies, Inc.
*/

#include <ctype.h>
#include "para.hxx"

PARA::PARA(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
}

void PARA::ParseRecords(const RECORD& FileRecord) {


  GPTYPE Start = 0;
  GPTYPE i = 0;
  PCHR   RecBuffer;
  GPTYPE RecStart, RecEnd, RecLength;
  GPTYPE ActualLength=0;
  
  STRING fn;
  FileRecord.GetFullFileName (&fn);
  PFILE fp = fopen (fn, "rb");
  if (!fp)
    {
      cout << "Could not access '" << fn << "'\n";
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
  
  

if(fseek(fp, 0, 2) == -1) {
	cout << "PARA::ParseRecords(): Seek failed - ";
	cout << fn << "\n";
	fclose(fp);
	return;	
	}
	
RecStart = 0;
RecEnd = ftell(fp);
if(RecEnd == 0) {
	cout << "PARA::ParseRecords(): Skipping ";
	cout << " zero-length record -" << fn << "...\n";
	fclose(fp);
	return;
	}


if(fseek(fp, RecStart, 0) == -1) {
	cout << "PARA::ParseRecords(): Seek failed - " << fn << "\n";
	fclose(fp);
	return;	
	}
	
RecLength = RecEnd - RecStart;
	
RecBuffer = new CHR[RecLength + 2];
if(!RecBuffer) {
	cout << "PARA::ParseRecords(): Failed to allocate ";
	cout << RecLength + 1 << " bytes - " << fn << "\n";
	fclose(fp);
	return;
	}

ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
if(ActualLength == 0) {
	cout << "PARA::ParseRecords(): Failed to fread\n";
	delete [] RecBuffer;
	fclose(fp);
	return;
	}
fclose(fp);
if(ActualLength != RecLength) {
	cout << "PARA::ParseRecords(): Failed to fread ";
	cout << RecLength << " bytes.  Actually read " << ActualLength;
	cout << " bytes - " << fn << "\n";
	delete [] RecBuffer;
	return;
	}

RecBuffer[ActualLength]='\0';  // NULL-terminate the buffer for strfns

// Now we can loop, scan for "\n\n", and use that to mark beginnings and
// endings.


Start = 0; int j=0;
for (i=0; i< ActualLength-1; i++) {
	if ( (RecBuffer[i]=='\n') && (RecBuffer[i+1]=='\n') ) {
		// We found a para marker, didn't we?
		if ( (i-1) > Start) {
			// Now we need to burn "\n"s until we get to the start of the
			// new para.
			for (j=i; (j < ActualLength) && (RecBuffer[j]=='\n'); j++);
			Record.SetRecordStart(Start);
			Record.SetRecordEnd(j-1);
			Db->DocTypeAddRecord(Record);
			Start = i = j;
			}

		}
	}

// Add the last record entry now

if (Start != ActualLength) {
	Record.SetRecordStart(Start);
	Record.SetRecordEnd(ActualLength-1);

	Db->DocTypeAddRecord(Record);
	}
	

}

PARA::~PARA() {
}
