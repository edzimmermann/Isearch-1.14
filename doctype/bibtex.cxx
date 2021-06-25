/*

File:        bibtex.cxx
Version:     1
Description: class BIBTEX - index documents by paragraphs 
Author:      Erik Scott, Scott Technologies, Inc.
*/

#include <ctype.h>
#include "bibtex.hxx"

BIBTEX::BIBTEX(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
}

void BIBTEX::ParseRecords(const RECORD& FileRecord) {


  GPTYPE Start = 0;
  GPTYPE i = 0;
  int    lastBrace = 0;  // this is an int because I need signed.
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
	cout << "BIBTEX::ParseRecords(): Seek failed - ";
	cout << fn << "\n";
	fclose(fp);
	return;	
	}
	
RecStart = 0;
RecEnd = ftell(fp);
if(RecEnd == 0) {
	cout << "BIBTEX::ParseRecords(): Skipping ";
	cout << " zero-length record -" << fn << "...\n";
	fclose(fp);
	return;
	}


if(fseek(fp, RecStart, 0) == -1) {
	cout << "BIBTEX::ParseRecords(): Seek failed - " << fn << "\n";
	fclose(fp);
	return;	
	}
	
RecLength = RecEnd - RecStart;
	
RecBuffer = new CHR[RecLength + 2];
if(!RecBuffer) {
	cout << "BIBTEX::ParseRecords(): Failed to allocate ";
	cout << RecLength + 1 << " bytes - " << fn << "\n";
	fclose(fp);
	return;
	}

ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
if(ActualLength == 0) {
	cout << "BIBTEX::ParseRecords(): Failed to fread\n";
	delete [] RecBuffer;
	fclose(fp);
	return;
	}
fclose(fp);
if(ActualLength != RecLength) {
	cout << "BIBTEX::ParseRecords(): Failed to fread ";
	cout << RecLength << " bytes.  Actually read " << ActualLength;
	cout << " bytes - " << fn << "\n";
	delete [] RecBuffer;
	return;
	}

RecBuffer[ActualLength]='\0';  // NULL-terminate the buffer for strfns

int w;
for (w=ActualLength; w>0; w--) 
   if (RecBuffer[w]=='}') {
      lastBrace = w;
      w=-1; // to break out of loop.
      }
      
for (i=Start; i<= ActualLength; i++) {
   if (RecBuffer[i]=='}') {  // did we find the end of a record?  Good.
      if (i==lastBrace) {  // we're on the very last one.
         i=ActualLength-1; // so we mark it at the very end, after the whitespace.
         }
      Record.SetRecordStart(Start);
      Record.SetRecordEnd(i);
      Db->DocTypeAddRecord(Record);
      Start = i+1;
      }
   }

}

//
//
// The new goal:  scan the record looking for (title = ") and (") pairs
// and mark them as a field named "title".
//
//

void BIBTEX::ParseFields(PRECORD NewRecord) {
	PFILE 	fp;
	STRING 	fn;
	GPTYPE 	RecStart, 
		RecEnd, 
		RecLength, 
		ActualLength;
	PCHR 	RecBuffer;
	PCHR 	file;
	

	// Open the file
	NewRecord->GetFullFileName(&fn);
	file = fn.NewCString();
	fp = fopen(fn, "rb");
	if (!fp) {
		cout << "BIBTEX::ParseRecords(): Failed to open file\n\t";
		perror(file);
		return;
	}

	// Determine the start and size of the record
	RecStart = NewRecord->GetRecordStart();
	RecEnd = NewRecord->GetRecordEnd();
	
	if (RecEnd == 0) {
		if(fseek(fp, 0, 2) == -1) {
			cout << "BIBTEX::ParseRecords(): Seek failed - ";
			cout << fn << "\n";
			fclose(fp);
			return;	
		}
		RecStart = 0;
		RecEnd = ftell(fp);
		if(RecEnd == 0) {
			cout << "BIBTEX::ParseRecords(): Skipping ";
			cout << " zero-length record -" << fn << "...\n";
			fclose(fp);
			return;
		}
		//RecEnd -= 1;
	}

	// Make two copies of the record in memory
	if(fseek(fp, RecStart, 0) == -1) {
		cout << "BIBTEX::ParseRecords(): Seek failed - " << fn << "\n";
		fclose(fp);
		return;	
	}
	RecLength = RecEnd - RecStart;
	
	RecBuffer = new CHR[RecLength + 1];
	if(!RecBuffer) {
		cout << "BIBTEX::ParseRecords(): Failed to allocate ";
		cout << RecLength + 1 << " bytes - " << fn << "\n";
		fclose(fp);
		return;
	}

	ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
	if(ActualLength == 0) {
		cout << "BIBTEX::ParseRecords(): Failed to fread\n\t";
		perror(file);
		delete [] RecBuffer;
		fclose(fp);
		return;
	}
	fclose(fp);
	if(ActualLength != RecLength) {
		cout << "BIBTEX::ParseRecords(): Failed to fread ";
		cout << RecLength << " bytes.  Actually read " << ActualLength;
		cout << " bytes - " << fn << "\n";
		delete [] RecBuffer;
		return;
	}
	RecBuffer[RecLength]='\0';
	
	

	// Parse the record and add fields to record structure
	STRING FieldName;
	FC fc;
	PFCT pfct;
	DF df;
	PDFT pdft;
	PCHR p;
	INT val_start;
	INT val_end;
	INT val_len;
	DFD dfd;

	pdft = new DFT();
	if(!pdft) {
		cout << "BIBTEX::ParseRecords(): Failed to allocate DFT - ";
		cout << fn << "\n";
		delete [] RecBuffer;
		return;
		}

// OK - we need to scan RecBuffer and find the "title" element, then fast-
// forward to the next '"' character.  That will be the title field val_start.
// Then we go forward again until we see another '"' character, and that will
// be the field end.
// If someone (a) knows how BibTeX represents a literal quotation mark and
// (b) wants to hack this to do the right thing, be my guest.  I'm a long-
// time [nt]roff user, myself. :-)

int i;
int state;
#define LOOKING 1
#define INQUOTES 2

// Basically, the following God-awful excuse for a state machine will
// look for title not occuring inside quotation marks.

val_start = 0;
state = LOOKING;
for (i=0; i< ActualLength; i++) {
   if (state==LOOKING) {
      if (RecBuffer[i]=='t')
      if (RecBuffer[i+1]=='i')
      if (RecBuffer[i+2]=='t')
      if (RecBuffer[i+3]=='l')
      if (RecBuffer[i+4]=='e') {
         // look for the quotation mark
         i=i+5;
         for (;(i<ActualLength) && (val_start==0); i++) {
            if (RecBuffer[i]=='"') {
               val_start = i;
               }
            }
         if (i==ActualLength) {
            cout << "Cannot find quote mark after title.\n";
            return;
            }
         for (i=val_start+1; (RecBuffer[i]!='"') && (i<ActualLength); i++);
         if (i==ActualLength) {
            cout << "couldn't find ending quote.\n";
            return;
            }
         else {
            val_end = i;
            i=ActualLength + 2; // a side effect to ensure exiting.
            }
         } // end of if we found title
      else if (RecBuffer[i]=='"') state=INQUOTES;
      } // end of looking
   else if (state == INQUOTES) {
      if (RecBuffer[i]=='"') state=LOOKING;
      }
   }   // end of for loop
      

	// We have a tag pair
	FieldName = "title";
	dfd.SetFieldName(FieldName);
	Db->DfdtAddEntry(dfd);
	fc.SetFieldStart(val_start);
	fc.SetFieldEnd(val_end);
	pfct = new FCT();
	pfct->AddEntry(fc);
	df.SetFct(*pfct);
	df.SetFieldName(FieldName);
	pdft->AddEntry(df);
	delete pfct;

	NewRecord->SetDft(*pdft);
	delete pdft;
	delete [] RecBuffer;

}



void BIBTEX::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		STRING* StringBufferPtr) {
	*StringBufferPtr = "";
	
	// rationale:  If an F present, dump the whole buffer.
	// Otherwise, if there are fields, then on any othjer present
	// return the "title" field value.
	
	if (ElementSet.Equals("F")) {
		ResultRecord.GetRecordData(StringBufferPtr);
		return;
	}
	if (Db->DfdtGetTotalEntries() == 0) {
		return;
	}
	STRING FieldName;
	FieldName = "title";
	
	Db->GetFieldData(ResultRecord, FieldName, StringBufferPtr);
}


BIBTEX::~BIBTEX() {
}
