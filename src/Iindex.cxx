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
File:		Iindex.cxx
Version:	1.02
Description:	Command-line indexer
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream.h>
#include <locale.h>

#if defined(_MSDOS) || defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "idb.hxx"
#include "common.hxx"
#include "record.hxx"
#include "strlist.hxx"
#include "dtreg.hxx"
#include "string.hxx"

#ifndef NO_MMAP
#include <sys/types.h>
#include <sys/mman.h>
#endif

class IDBC : public IDB {
public:
	IDBC(const STRING& NewPathName, const STRING& NewFileName, const STRLIST& NewDocTypeOptions) :
			IDB(NewPathName, NewFileName, NewDocTypeOptions) { };
protected:
	void IndexingStatus(const INT StatusMessage, const PSTRING FileName,
			const INT Count) const {
		switch (StatusMessage) {
			case (IndexingStatusParsingFiles):
				cout << "   Parsing files ..." << endl;
				break;
			case (IndexingStatusParsingDocument):
				cout << "   Parsing " << *FileName;
				cout << " ..." << endl;
				break;
			case (IndexingStatusIndexing):
				cout << "   Indexing " << Count << " words ..." << endl;
				break;
			case (IndexingStatusMerging):
				cout << "   Merging index ..." << endl;
				break;
		}
	};
};

typedef IDBC* PIDBC;

STRING Separator;
STRING DocumentType;
UINT4 MemoryUsage = 0;

void AddFile(PIDB IdbPtr, STRING& PathName, STRING& FileName) 
{
  RECORD Record;
  Record.SetPathName(PathName);
  Record.SetFileName(FileName);
  Record.SetDocumentType(DocumentType);
  if (Separator.Equals("")) {
    Record.SetRecordStart(0);
    Record.SetRecordEnd(0);
    IdbPtr->AddRecord(Record);
  } else {
#ifndef NO_MMAP
    STRING Fn;
    Record.GetFullFileName(&Fn);
    FILE* Fp = fopen(Fn, "rb");
    if (!Fp) {
      return;
    }
    LONG FileSize = GetFileSize(Fp);
    INT FileDesc = fileno(Fp);
    CHR* Buffer;
    Buffer = (CHR*) mmap((caddr_t)0, FileSize, PROT_READ, MAP_PRIVATE,
			 FileDesc, 0);
    Record.SetRecordStart(0);
    CHR* Position = Buffer;
    CHR* Found;
    GDT_BOOLEAN Done;
    CHR* EndOfBuffer = Buffer + FileSize;
    CHR* Sep = Separator.NewCString();
    CHR SepChar = Sep[0];
    GPTYPE Offset;
    SIZE_T SepLength = Separator.GetLength();
    do {
	 Done = GDT_FALSE;
	 while (Done == GDT_FALSE) {
	   while ( (Position < EndOfBuffer) && (*Position != SepChar) ) {
	     Position++;
	   }
	   if (Position >= EndOfBuffer) {
	     Done = GDT_TRUE;
	     Found = 0;
	   } else {
	     if ((Position + SepLength) <= EndOfBuffer) {
	       if (strncmp(Sep, Position, SepLength) == 0) {
		 Found = Position;
		 Done = GDT_TRUE;
	       }
	     }
	   }
	   if (Done == GDT_FALSE) {
	     Position++;
	   }
	 }
	 if (Found) {
	   Offset = (GPTYPE)((UINT4)Found - (UINT4)Buffer);
	   /* the separator marks the beginning of the next 
	      record. (offset - 1), then marks the end of 
	      the current record. we must make sure that the
	      end of the current record is past the beginning 
	      of the current record.

	      Don't do this for the start of the file where
	      Offset = 0, 'cause Offset is unsigned and you'll get
	      in big trouble when you compute Offset-1!
	    */
//	   if ((Offset > 0) && ( (Offset - 1) > Record.GetRecordStart())) {
//	   if ( Offset == 0 )
//	     Position++;
//	   else if ( (Offset - 1) > Record.GetRecordStart()) {
	   if (Offset > Record.GetRecordStart()) {
	     Record.SetRecordEnd(Offset -1);
	     IdbPtr->AddRecord(Record);
	     Record.SetRecordStart(Offset);
	     Position = Found + strlen(Sep);
	   } else {
	     Position++;
	   }
	 }
       } while (Found);
    if ((FileSize - 1) > Record.GetRecordStart()) {
//      Record.SetRecordEnd(FileSize);  // I think this is right - aw3
      Record.SetRecordEnd(FileSize-1);
      IdbPtr->AddRecord(Record);
    }
    delete [] Sep;
    fclose(Fp);
#else
    GPTYPE Start = 0;
    GPTYPE Position = 0;
    GPTYPE SavePosition = 0;
    GPTYPE RecordEnd;
    GPTYPE C;
    STRING Fn;
    CHR Ch, Sch;
    STRINGINDEX Slen = Separator.GetLength();
    PCHR Buffer = new CHR[Slen+1];
    Sch = Separator.GetChr(1);
    Record.GetFullFileName(&Fn);
    PFILE Fp = fopen(Fn, "rb");
    if (!Fp) {
      return;
    }
    while (fread(&Ch, 1, 1, Fp) == 1) {
      SavePosition = Position;
      Position++;
      if (Ch == Sch) {
	*Buffer = Ch;
	C = fread(Buffer + 1, 1, Slen - 1, Fp);
	Position += C;
	Buffer[C+1] = '\0';
	if (Separator.Equals(Buffer)) {
	  Record.SetRecordStart(Start);
	  if (SavePosition == 0) {
	    RecordEnd = 0;
	  } else {
	    RecordEnd = SavePosition - 1;
	  }
	  if (RecordEnd > Start) {
	    Record.SetRecordEnd(RecordEnd);
	    IdbPtr->AddRecord(Record);
	    Start = SavePosition;
	  }
	} else {
	  // Rewind, so we won't skip over
	  // the beginning of a separator string.
	  // (thanks to Jae W. Chang for this fix)
	  Position -= C;
	  fseek(Fp, Position, 0);
	}
      }
    }
    Record.SetRecordStart(Start);
    if (SavePosition == 0) {
      RecordEnd = 0;
    } else {
      RecordEnd = Position - 1;
    }
    if (RecordEnd > Start) {
      Record.SetRecordEnd(RecordEnd);
      IdbPtr->AddRecord(Record);
    }
    fclose(Fp);
    delete [] Buffer;
#endif
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << endl << "Iindex, Version " << IsearchVersion;
    cout << ", Copyright (c) 1995 MCNC/CNIDR" << endl << endl;
    cout << "Iindex [-d (X)] // Use (X) as the root name for database files." << endl;
    cout << "       [-a] // Add to existing database, instead of replacing it." << endl;
    cout << "       [-m (X)] // Load (X) megabytes of data at a time for indexing" << endl;
    cout << " // (default=1)." << endl;
    cout << "       [-s (X)] // Treat (X) as a separator for multiple documents within" << endl;
    cout << " // a single file." << endl;
    cout << "       [-t (X)] // Index as files of document type (X)." << endl;
    cout << "       [-f (X)] // Read list of file names to be indexed from file (X)." << endl;
    cout << "       [-r] // Recursively descend subdirectories." << endl;
    cout << "       [-o (X)] // Document type specific option." << endl;
    cout << "       (X) (Y) (...) // Index files (X), (Y), etc." << endl << endl;
    cout << "Examples:  Iindex -d POETRY *.doc *.txt" << endl;
    cout << "           Iindex -d WEBPAGES -t SGMLTAG *.html" << endl << endl;
    cout << "Document Types Supported:";
    DTREG dtreg(0);
    STRLIST DocTypeList;
    dtreg.GetDocTypeList(&DocTypeList);
    STRING s;
    INT x;
    INT y = DocTypeList.GetTotalEntries();
    for (x=1; x<=y; x++) {
      DocTypeList.GetEntry(x, &s);
      cout << '\t' << s;
    }
    cout << "\n\n";
    return 0;
  } else {
    cout << "Iindex " << IsearchVersion << endl;
  }

  if (!setlocale(LC_CTYPE,"")) {
    cout << "Warning: Failed to set the locale!" << endl;
  }

  STRLIST DocTypeOptions;
  CHR Cwd[256];
  getcwd(Cwd, 255);
  STRING Flag;
  STRING DBName;
  STRING FileList;
  INT DebugFlag = 0;
  INT Recursive = 0;
  INT AppendDb = 0;
  INT x = 0;
  INT LastUsed = 0;
  while (x < argc) {
    if (argv[x][0] == '-') {
      Flag = argv[x];
      if (Flag.Equals("-o")) {
	if (++x >= argc) {
	  cout << "ERROR: No option specified after -o." << endl << endl;
	  return 0;
	}
	STRING S;
	S = argv[x];
	DocTypeOptions.AddEntry(S);
	LastUsed = x;
      }
      if (Flag.Equals("-d")) {
	if (++x >= argc) {
	  cout << "ERROR: No database name specified after -d." << endl << endl;
	  return 0;
	}
	DBName = argv[x];
	LastUsed = x;
      }
      if (Flag.Equals("-f")) {
	if (++x >= argc) {
	  cout << "ERROR: No file name specified after -f." << endl << endl;
	  return 0;
	}
	FileList = argv[x];
	LastUsed = x;
      }
      if (Flag.Equals("-t")) {
	if (++x >= argc) {
	  cout << "ERROR: No document type name specified after -dt." << endl << endl;
	  return 0;
	}
	DocumentType = argv[x];
	LastUsed = x;
      }
      if (Flag.Equals("-s")) {
	if (++x >= argc) {
	  cout << "ERROR: No separator string specified after -s." << endl << endl;
	  return 0;
	}
	Separator = argv[x];
	LastUsed = x;
      }
      if (Flag.Equals("-m")) {
	if (++x >= argc) {
	  cout << "ERROR: No memory usage specified after -m." << endl << endl;
	  return 0;
	}
	MemoryUsage = strtol(argv[x], NULL, 10);
	LastUsed = x;
      }
      if (Flag.Equals("-a")) {
	AppendDb = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-r")) {
	Recursive = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-debug")) {
	DebugFlag = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-erase")) {
	cout << "Please use Iutil for erasing databases; it is no longer supported in Iindex." << endl;
	return 0;
      }
    }
    x++;
  }
  
  if (DBName.Equals("")) {
    DBName = IsearchDefaultDbName;
    //		cout << "ERROR: No database name specified!" << endl;
    //		return 0;
  }
  
  x = LastUsed + 1;
  
  INT NumFiles = argc - x;
  INT z = x;
  
  if ( (FileList.Equals("")) && (NumFiles == 0) ) {
    cout << "ERROR: No files specified for indexing!" << endl;
    return 0;
  }
  
  if ( (!FileList.Equals("")) && (NumFiles != 0) ) {
    cout << "ERROR: Unable to handle -f and file names at the same time." << endl;
    return 0;
  }
  
  //	RECLIST reclist;
  //	RECORD record;
  STRING PathName, FileName;
  
  cout << "Building document list ..." << endl;
  
  PIDBC pdb;
  STRING DBPathName, DBFileName;
  
  DBPathName = DBName;
  DBFileName = DBName;
  RemovePath(&DBFileName);
  RemoveFileName(&DBPathName);

  if (!AppendDb) {
    STRING KillFile;
    PCHR cKillFile;

    KillFile = DBName;
    KillFile.Cat(".dbi");
    StrUnlink(KillFile);

    KillFile = DBName;
    KillFile.Cat(".dfd");
    StrUnlink(KillFile);

    KillFile = DBName;
    KillFile.Cat(".inx");
    StrUnlink(KillFile);

    KillFile = DBName;
    KillFile.Cat(".mdg");
    StrUnlink(KillFile);

    KillFile = DBName;
    KillFile.Cat(".mdk");
    StrUnlink(KillFile);

    KillFile = DBName;
    KillFile.Cat(".mdt");
    StrUnlink(KillFile);

#if defined(_MSDOS) || defined(_WIN32)

  /*
   * For the moment you are out of luck if you have more then
   * 99 fields in an existing database.  You'll have to remove
   * the old database by hand before building the new database.
   */

    KillFile = "del ";
    KillFile.Cat(DBName);
    KillFile.Cat(".0*");
    cKillFile = KillFile.NewCString();
    system(cKillFile);
    delete cKillFile;
#else
    KillFile = "rm -f ";
    KillFile.Cat(DBName);
    KillFile.Cat(".[0-9]*");
    cKillFile = KillFile.NewCString();
    system(cKillFile);
    delete cKillFile;
#endif
  }

  pdb = new IDBC(DBPathName, DBFileName, DocTypeOptions);
  
  if (!pdb->ValidateDocType(DocumentType)) {
    cout << "ERROR: Unknown document type specified." << endl;
    delete pdb;
    return 0;
  }
  
  if (DebugFlag) {
    pdb->DebugModeOn();
  }
  
  if (!AppendDb) {
    pdb->KillAll();
  } else {
    if (!pdb->IsDbCompatible()) {
      cout << "The specified database is not compatible with this version of Iindex." << endl;
      cout << "You cannot append to a database created with a different version." << endl;
      delete pdb;
      return 0;
    }
  }
  
  // Set Global Document Type to match -t if there isn't already one
  if (DocumentType != "") {
    STRING GlobalDoctype;
    pdb->GetGlobalDocType(&GlobalDoctype);
    if (GlobalDoctype == "") {
      pdb->SetGlobalDocType(DocumentType);
    }
  }
  
  STRING AppTemp;
  CHR pAppTemp[80];
  pdb->ComposeDbFn(&AppTemp, ".ii~");
  AppTemp.GetCString(pAppTemp, 80);
  
  if ( FileList.Equals("") ) {
    INT y;
    STRING TheFile;
    for (z=0; z<NumFiles; z++) {
      TheFile = argv[z+x];
      if ( (!(pdb->IsSystemFile(TheFile))) &&
	  (strcmp(argv[z+x], "core") != 0) ) {
	y = chdir(argv[z+x]);
	if (y == -1) {
	  PathName = argv[z+x];
	  FileName = argv[z+x];
	  RemovePath(&FileName);
	  RemoveFileName(&PathName);
	  AddFile(pdb, PathName, FileName);
	} else {
	  chdir(Cwd);
	  if (Recursive) {
	    CHR tempbuf[256];
	    // Ultrix doesn't seem to like -follow.
	    //sprintf(tempbuf, "find %s -name \"*\" -follow -print > %s",
	    sprintf(tempbuf, "find %s -name \"*\" -print > %s",
		    argv[z+x], pAppTemp);
	    system(tempbuf);
	    
	    
	    PFILE fp = fopen(AppTemp, "r");
	    CHR s[256], t[256];
	    STRING v;
	    if (!fp) {
	      cout << "ERROR: Can't generate file list (-r)." << endl;
	      delete pdb;
	      return 0;
	    }
	    while (fgets(s, 255, fp) != NULL) {
	      if (s[strlen(s)-1] == '\n') {
		s[strlen(s)-1] = '\0';
	      }
	      v = s;
	      RemovePath(&v);
	      v.GetCString(t, 256);
	      TheFile = t;
	      if ( (!(pdb->IsSystemFile(TheFile))) &&
		  (strcmp(t, pAppTemp) != 0) &&
		  (strcmp(t, "core") != 0) &&
		  (strlen(t) > 0) ) {
		y = chdir(s);
		if (y == -1) {
		  PathName = s;
		  FileName = s;
		  RemovePath(&FileName);
		  RemoveFileName(&PathName);
		  AddFile(pdb, PathName, FileName);
		} else {
		  chdir(Cwd);
		}
	      }
	    }
	    fclose(fp);
	    StrUnlink(AppTemp);
	  }
	}
      }
    }
  }
  
  if ( !FileList.Equals("") ) {
    PFILE fp = fopen(FileList, "r");
    CHR s[256];
    if (!fp) {
      cout << "ERROR: Can't find file list (-f)." << endl;
      delete pdb;
      return 0;
    }
    while (fgets(s, 255, fp) != NULL) {
      if (s[strlen(s)-1] == '\n') {
	s[strlen(s)-1] = '\0';
      }
      PathName = s;
      FileName = s;
      RemovePath(&FileName);
      RemoveFileName(&PathName);
      AddFile(pdb, PathName, FileName);
    }
    fclose(fp);
  }
  
  if (AppendDb) {
    cout << "Adding to database ";
  } else {
    cout << "Building database ";
  }
  DBName.Print();
  cout << ":\n";
  
  if (MemoryUsage != 0) {
    pdb->SetIndexingMemory(MemoryUsage*1024*1024);
  }
  
  pdb->Index();
  
  delete pdb;
  cout << "Database files saved to disk." << endl;
  
  unlink("tmp");
  return 0;
}
