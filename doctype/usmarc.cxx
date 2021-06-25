/*
File:        usmarc.cxx
Version:     1
Description: class USMARC - MARC records for library use
Author:      Erik Scott, Scott Technologies, Inc.
*/

#include <ctype.h>
#include <string.h>  /* For strstr() in ParseRecords */
#include "marc.hxx"
#include "usmarc.hxx"
#include "defs.hxx"  // to get record syntaxes

#define EOL "\n";

USMARC::USMARC(PIDBOBJ DbParent) : DOCTYPE::DOCTYPE(DbParent) {
}



void USMARC::ParseRecords(const RECORD& FileRecord) {

//Finding the range of a MARC record is easy:  The first five bytes of any
//record MARC record contains a zero-filled representation of the record
//length.  We assume that your files will consist of a bunch of MARC
//records concatenated into one big file, so we:
//   while not end of file do
//       read five bytes
//       make an int out of them
//       indicate that range as a record
//       increment the file pointer to the start of the next record.
//   done

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

int RS, RE;  
  
if(fseek(fp, 0, 2) == -1) {
	cout << "USMARC::ParseRecords(): Seek failed - ";
	cout << fn << "\n";
	fclose(fp);
	return;	
	}
	
GPTYPE FileStart, FileEnd, FileLength;
FileStart = 0;
FileEnd = ftell(fp);
if(FileEnd == 0) {
	cout << "USMARC::ParseRecords(): Skipping ";
	cout << " zero-length record -" << fn << "...\n";
	fclose(fp);
	return;
	}
if(fseek(fp, FileStart, 0) == -1) {
	cout << "USMARC::ParseRecords(): Seek failed - " << fn << "\n";
	fclose(fp);
	return;	
	}
	
FileLength = FileEnd - FileStart;

int bytePos = 0; // we're going to start reading with bytePos = 0 so we
                 // always know how many bytes we've read.
RS=0;            // we also know the first record will begin @ 0.

int marcLength;
char LenBuff[16];
char c;
int toBurn, count;

// els - next line was just "if" (???)
while (FileLength >= bytePos + 5) { // if there are more records to read
   LenBuff[0]=getc(fp);
   LenBuff[1]=getc(fp);
   LenBuff[2]=getc(fp);
   LenBuff[3]=getc(fp);
   LenBuff[4]=getc(fp);  // brutal, yes, but also simple.
   LenBuff[5]='\0';      // make a null-term string, and do it every time.
   bytePos +=5;          // we read 5 bytes, so increment our Pos by 5.
   sscanf(LenBuff,"%d", &marcLength);  // turn the string into an int

   if (marcLength <= 0) {
   	cout << "Something went awry trying to read MARC record Length in "
   	        << fn << " \n";
   	return;
   	}
   // else we must have a valid marcLength now, so lets burn some characters
   for (toBurn = marcLength - 5, count = 0; count < toBurn; c=getc(fp) , count++, bytePos++);
   Record.SetRecordStart(RS);
   Record.SetRecordEnd  ((RS+marcLength)-1); // former bug : 1 over end of buffer!!
   
   Db->DocTypeAddRecord(Record);
   RS = RS+marcLength;
   } 
	
}


PCHR RecBuffer;
GPTYPE ActualLength=0;
GPTYPE RecStart, RecEnd, RecLength;
GPTYPE marcNumDirEntries;
GPTYPE marcRecordLength;
GPTYPE marcBaseAddr;

typedef struct mde {
   char field[4];
   char length[5];
   char offset[6];
   } marc_dir_entry;

marc_dir_entry *marcDir;

   

void readFileContents(PRECORD NewRecord) {
STRING fn;
PCHR   file;
PFILE  fp;

        NewRecord->GetFullFileName(&fn);
        file = fn.NewCString();
        fp = fopen(fn, "rb");
        if (!fp) {
                cout << "USMARC::ParseRecords(): Failed to open file\n\t";
                perror(file);
                return;
        }
                // Determine the start and size of the record
        RecStart = NewRecord->GetRecordStart();
        RecEnd = NewRecord->GetRecordEnd();

        if (RecEnd == 0) {
                if(fseek(fp, 0, 2) == -1) {
                        cout << "USMARC::ParseRecords(): Seek failed - ";
                        cout << fn << "\n";
                        fclose(fp);
                        return;
                }
                RecStart = 0;
                RecEnd = ftell(fp);
                if(RecEnd == 0) {
                        cout << "USMARC::ParseRecords(): Skipping ";
                        cout << " zero-length record -" << fn << "...\n";
                        fclose(fp);
                        return;
                }
                //RecEnd -= 1;
        }
        if(fseek(fp, RecStart, 0) == -1) {
                cout << "USMARC::ParseRecords(): Seek failed - " << fn << "\n";
                fclose(fp);
                return;
        }
        RecLength = RecEnd - RecStart;

        RecBuffer = new CHR[RecLength + 1];
        if(!RecBuffer) {
                cout << "USMARC::ParseRecords(): Failed to allocate ";
                cout << RecLength + 1 << " bytes - " << fn << "\n";
                fclose(fp);
                return;
        }

        ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
        if(ActualLength == 0) {
                cout << "USMARC::ParseRecords(): Failed to fread\n\t";
                perror(file);
                delete [] RecBuffer;
                fclose(fp);
                return;
        }
        fclose(fp);
        if(ActualLength != RecLength) {
                cout << "USMARC::ParseRecords(): Failed to fread ";
                cout << RecLength << " bytes.  Actually read " << ActualLength;
                cout << " bytes - " << fn << "\n";
                delete [] RecBuffer;
                return;
        }
        RecBuffer[RecLength]='\0';

}

void readRecordLength(void) {
char lenstr[6];
int i;
for (i=0; i<5; i++) lenstr[i]=RecBuffer[i];
lenstr[5]='\0';
sscanf(lenstr,"%d",&marcRecordLength);
}

void readBaseAddr(void) {
char lenstr[6];
int i;
for (i=12; i<17; i++) lenstr[i-12]=RecBuffer[i];
lenstr[5]='\0';
sscanf(lenstr,"%d",&marcBaseAddr);
}


void readMarcStructure(PRECORD NewRecord) {

readFileContents(NewRecord);
readRecordLength(); // sets global "marcRecordLength"
readBaseAddr();     // sets global "marcBaseAddr"

marcNumDirEntries = ( (marcBaseAddr-25) / 12);
int i;
marcDir = new marc_dir_entry[marcNumDirEntries];

int bytenum,j;
for (i=0; i< marcNumDirEntries; i++) {
   // read that dir entry.
   // seek to 24 + (numdirentry*12), read the field (3 bytes), the length
   // (4 bytes), and the starting address (5 bytes).  Remember to null-terminate
   // each component.  Remember the starting address is relative to
   // marcBaseAddr.
   bytenum = 24+(i*12);
   for (j=0; j<3; j++) marcDir[i].field[j]=RecBuffer[bytenum+j];
   marcDir[i].field[3]='\0';
   for (j=0; j<4; j++) marcDir[i].length[j]=RecBuffer[bytenum+3+j];
   marcDir[i].length[4]='\0';
   for (j=0; j<5; j++) marcDir[i].offset[j]=RecBuffer[bytenum+7+j];
   marcDir[i].offset[5]='\0';
   }


}




// The function usefulMarcField is used to decide if you want to index a given
// field. It should be used by both ParseFields and ParseWords, since it's a bad
// idea to have a field with no words or words without a field.
// The idea is that we will pass in a field as a null-terminated string, 
// like "110", and the function will return 0 is we should ignore it and non-0
// if we should index it.  For now, I'm going to say "index everything" even
// though that isn't the best policy.

int usefulMarcField(char *fieldStr) {

return 1;

}

// marcFieldNumToName takes a character string representation of a marc field
// number and tries to return a field name that would be sensible to ordinary,
// unsophisticated users.  These users have a bibliographic vocabulary that
// contains "author", "subject", "title", and little else.  They couldn't
// care less if the author was a 110 corporate author or a 111 meeting name
// author, especially since something like proceedings of the world wide web
// consortium, for example, could very easily go either way.
// Real Librarians(TM) will want to do some hacking in here. :-)
//
void marcFieldNumToName(char *fieldStr, char *fieldName) {
int fieldNum;

sscanf(fieldStr,"%d",&fieldNum);  // I *like* sscanf. It makes me feel good.

fieldName[0]='\0';  // This means that if nothing else happens, return null str.

switch (fieldNum) {
  case 20: strcpy(fieldName,"isbn"); break;
  case 22: strcpy(fieldName,"issn"); break;
  case 10: strcpy(fieldName,"lccn"); break;
  case 82: strcpy(fieldName,"dewey"); break;   // as in dewey decimal number, which we learned in elementary school and then found out was useless in college.
  case 100: strcpy(fieldName,"author"); break; // really should be "main entry-personal name", but I'm trying to hide some detail
  case 245: strcpy(fieldName,"title"); break;
  case 260: strcpy(fieldName,"publisher"); break;
  case 490: strcpy(fieldName,"title"); break;  // really should be "series statement"
  case 500: strcpy(fieldName,"note"); break;
  case 650: strcpy(fieldName,"subject"); break;
  case 740: strcpy(fieldName,"title"); break; // officially "added entry - uncontrolled related/analytical title", but dammit Jim I'm a engineer, not a librarian
  case 830: strcpy(fieldName,"title"); break; // "series added entry - uniform title "
  default: fieldName[0]='\0'; break;
  }
}



void USMARC::ParseFields(PRECORD NewRecord) {

// Right now, we're just going to call readMarcStructure()
// and use this to debug that.  Joy joy.

readMarcStructure(NewRecord);

// Now the strategy is:
// for each dir entry,
//    check to see if we should add this field
//    if so, add it as its MARC field number (like 110 for corp author)
//    check to see if that field has been given a name (like "author").
//      if so, then add it again using the name we give it.

int i;
STRING FieldName;
FC fc;
PFCT pfct;
DF df;
PDFT pdft;
DFD dfd;
int fieldLength, fieldOffset;
char newFieldName[256];

pdft = new DFT();
if(!pdft) {
	cout << "EMACSINFO::ParseRecords(): Failed to allocate DFT \n";
	delete [] RecBuffer;
	delete [] marcDir;
	return;
	}
for (i=0; i< marcNumDirEntries; i++) {
   if (usefulMarcField(marcDir[i].field)) { // is it a field we're interested in?
      sscanf(marcDir[i].length,"%d",&fieldLength);
      sscanf(marcDir[i].offset,"%d",&fieldOffset);
      
      FieldName = marcDir[i].field;
      dfd.SetFieldName(FieldName);
      Db->DfdtAddEntry(dfd);
      fc.SetFieldStart(fieldOffset+marcBaseAddr);
      fc.SetFieldEnd(fieldOffset+marcBaseAddr+fieldLength-1);
      pfct = new FCT();
      pfct->AddEntry(fc);
      df.SetFct(*pfct);
      df.SetFieldName(FieldName);
      pdft->AddEntry(df);
      delete pfct;
      
      marcFieldNumToName(marcDir[i].field, newFieldName);
      if (newFieldName[0]!='\0') {  // if we want the same field to have two names
         FieldName = newFieldName;
         dfd.SetFieldName(FieldName);
         Db->DfdtAddEntry(dfd);
         fc.SetFieldStart(fieldOffset+marcBaseAddr);
         fc.SetFieldEnd(fieldOffset+marcBaseAddr+fieldLength-1);
         pfct = new FCT();
         pfct->AddEntry(fc);
         df.SetFct(*pfct);
         df.SetFieldName(FieldName);
         pdft->AddEntry(df);
         delete pfct;
         } // end of if we have a second name
      }    // end of if usefulMarcField
   }       // end of for loop


	NewRecord->SetDft(*pdft);
	delete pdft;


}          // end of function



	
				
GPTYPE USMARC::ParseWords(CHR* DataBuffer, INT DataLength, INT DataOffset,
				GPTYPE* GpBuffer, INT GpLength) {				

INT GpListSize = 0;
INT Position = 0;
INT i,fieldLength, fieldOffset;
INT endingPosition;


for (i=0; i< marcNumDirEntries; i++) {
   sscanf(marcDir[i].length,"%d",&fieldLength);
   sscanf(marcDir[i].offset,"%d",&fieldOffset);
   Position = fieldOffset+marcBaseAddr+2; // the +2 is to skip the two-character "indicator" at the front of all USMARC fields.
   endingPosition = fieldOffset + marcBaseAddr + fieldLength -2;  // we don't need the trailer field delimiter, either.

int e;

   
int ender = Position+5;

	while (Position < endingPosition) {
		if (RecBuffer[Position]=='\037') {
		   Position +=2; // skip over the subfield indicator
		   }
		while ( (Position < endingPosition) &&
				(!isalnum(DataBuffer[Position])) ) {
			Position++;
		}
		if ( (Position < endingPosition) &&
		     (!(Db->IsStopWord(DataBuffer + Position, DataLength))) ) {
			GpBuffer[GpListSize++] = DataOffset + Position;
		}
		while ( (Position < endingPosition) &&
				(isalnum(DataBuffer[Position])) ) {
			Position++;
		}
	} // end of while loop;
   } // end of for i in dir entries
   
   // before we leave, we should free some leaks (er, I mean, "blocks of memory")
   delete [] RecBuffer;
   delete [] marcDir;
   
   return GpListSize;
}	// Return # of GP's added to GpBuffer


// The following super-cheeseball stunt is being done because dtconf
// has a one-to-one relationship between doctypes and .cxx files.  So we cheat
// like big dogs.
//
// Of course, if we're trying to build this as part of Isite/zdist, then
// we need to make sure we *don't* include these!
//
/*
#include "memcntl.c"
#include "marclib.c"
#include "marc.cxx"
*/
void USMARC::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		const STRING& RecordSyntax, STRING* StringBufferPtr) {
	

STRING myBuff;
PFILE fp;
ResultRecord.GetRecordData(&myBuff);

if (RecordSyntax == UsmarcRecordSyntax) {
  // hey, if they want MARC, just shove it at 'em.  They'll figure it out.
  *StringBufferPtr = myBuff;
  return;  // don't forget to bail out now.
  }

if (RecordSyntax == SutrsRecordSyntax) {
   // "Sutrs" is the Z39.50-compliant way to say "ASCII".
   

   CHR *TempFile = tmpnam(NULL);
   if((fp = fopen(TempFile, "w")) == NULL) {
	perror(TempFile);
	exit(1);
	}

   MARC *m;
   m = new MARC(myBuff);
   m->Print(fp);
   delete m;
   fclose(fp);
   // Now read the file back in
   
   fp = fopen(TempFile, "rb");
        if (!fp) {
                cout << "USMARC::Present(): Failed to open file\n";
                return;
        }
                // Determine the start and size of the record
        RecEnd = 0;
        if (RecEnd == 0) {
                if(fseek(fp, 0, 2) == -1) {
                        cout << "USMARC::ParseRecords(): Seek failed - ";
                        cout << TempFile << "\n";
                        fclose(fp);
                        return;
                }
                RecStart = 0;
                RecEnd = ftell(fp);
                if(RecEnd == 0) {
                        cout << "USMARC::Present(): problem: ";
                        cout << " zero-length record -" << TempFile << "...\n";
                        fclose(fp);
                        return;
                }
                //RecEnd -= 1;
        }
        if(fseek(fp, RecStart, 0) == -1) {
                cout << "USMARC::Present(): Seek failed - " << TempFile << "\n";
                fclose(fp);
                return;
        }
        RecLength = RecEnd - RecStart;

        RecBuffer = new CHR[RecLength + 1];
        if(!RecBuffer) {
                cout << "USMARC::Present(): Failed to allocate ";
                cout << RecLength + 1 << " bytes - " << TempFile << "\n";
                fclose(fp);
                return;
        }

        ActualLength = (GPTYPE)fread(RecBuffer, 1, RecLength, fp);
        if(ActualLength == 0) {
                cout << "USMARC::Present(): Failed to fread\n";
                delete [] RecBuffer;
                fclose(fp);
                return;
        }
        fclose(fp);
        if(ActualLength != RecLength) {
                cout << "USMARC::Present(): Failed to fread ";
                cout << RecLength << " bytes.  Actually read " << ActualLength;
                cout << " bytes - " << TempFile << "\n";
                delete [] RecBuffer;
                return;
        }
        RecBuffer[RecLength]='\0';


   *StringBufferPtr = RecBuffer;
   return;
   }
// insert something here to handle html if you want to.

} 


USMARC::~USMARC() {
}
