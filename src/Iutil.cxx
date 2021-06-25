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
File:		Iutil.cxx
Version:	1.01
Description:	Command-line utilities for Isearch databases
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdlib.h>
#ifdef UNIX
#include <unistd.h>
#endif
#include <errno.h>
#include <string.h>
#include <iostream.h>
#include <locale.h>

#if defined(_MSDOS) || defined(_WIN32)
#include <direct.h>
#endif

#include <ctype.h>
#include "idb.hxx"
#include "string.hxx"
#include "common.hxx"
#include "record.hxx"
#include "strlist.hxx"
#include "dtreg.hxx"

class IDBC : public IDB {
public:
	IDBC(const STRING& NewPathName, const STRING& NewFileName, const STRLIST& NewDocTypeOptions) :
			IDB(NewPathName, NewFileName, NewDocTypeOptions) { };
protected:
	void IndexingStatus(const INT StatusMessage, const PSTRING FileName,
			const INT WordCount) const {
/*
		switch (StatusMessage) {
			case (IndexingStatusReading):
				cout << "   Reading file ";
				FileName->Print();
				cout << " ..." << endl;
				break;
			case (IndexingStatusIndexing):
				cout << "   Indexing " << WordCount<< " words ..." << endl;
				break;
			case (IndexingStatusMerging):
				cout << "   Merging index ..." << endl;
				break;
		}
*/
	};
};

typedef IDBC* PIDBC;

#include "registry.hxx"

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << endl << "Iutil, Version " << IsearchVersion;
    cout << ", Copyright (c) 1995 MCNC/CNIDR" << endl << endl;
    cout << "Iutil [-d (X)]  // Use (X) as the root name for database files." << endl;
    cout << "      [-vi]  // View summary information about the database." << endl;
    cout << "      [-vf]  // View list of fields defined in the database." << endl;
    cout << "      [-v]  // View list of documents in the database." << endl;
    cout << "      [-newpaths]  // Prompt for new pathnames for files." << endl;
    cout << "      [-del]  // Mark individual documents (by key) to be deleted from database." << endl;
    cout << "      [-undel]  // Unmark documents (by key) that were marked for deletion." << endl;
#ifdef DICTIONARY
    cout << "      [-dict] //Generate a search dictionary for the index." << endl;
    cout << "      [-centroid] //create a centroid document for the database." << endl;
#endif
    cout << "      [-c]  // Cleanup database by removing unused data (useful after -del)." << endl;
    cout << "      [-erase]  // Erase the entire database." << endl;
    cout << "      [-gt (X)]  // Set (X) as the global document type for the database." << endl;
    cout << "      [-gt0]  // Clear the global document type for the database." << endl;
    cout << "      [-o (X)]  // Document type specific option." << endl << endl;
    cout << "Example:  Iutil -d POETRY -erase" << endl << endl;
    cout << "Document Types Supported:";
    DTREG dtreg(0);
    STRLIST DocTypeList;
    dtreg.GetDocTypeList(&DocTypeList);
    STRING s;
    INT x;
    INT y = DocTypeList.GetTotalEntries();
    for (x=1; x<=y; x++) {
      DocTypeList.GetEntry(x, &s);
      cout << "\t" << s;
    }
    cout << endl << endl;
    return 0;
  } else {
    cout << "Iutil " << IsearchVersion << endl;
  }

  if (!setlocale(LC_CTYPE,"")) {
    cout << "Warning: Failed to set the locale!" << endl;
  }

  STRLIST DocTypeOptions;
  STRING GlobalDoctype;
  INT SetGlobalDoctype = 0;
  CHR Cwd[256];
  getcwd(Cwd, 255);
  STRING Flag;
  STRING DBName;
  STRING Temp;
  STRLIST WordList;
  INT DebugFlag = 0;
  INT Skip = 0;
#ifdef DICTIONARY
  INT DictGen = 0;
  INT DoCentroid = 0;
#endif
  INT EraseAll = 0;
  INT PathChange = 0;
  INT DeleteByKey = 0;
  INT UndeleteByKey = 0;
  INT Cleanup = 0;
  INT View = 0;
  INT ViewInfo = 0;
  INT ViewFields = 0;
//	INT Recursive = 0;
//	INT AppendDb = 0;
//	UINT4 MemoryUsage = 0;
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
      if (Flag.Equals("-gt")) {
	if (++x >= argc) {
	  cout << "ERROR: No document type specified after -gt." << endl;
	  cout << "       Use -gt0 if you want no document type." << endl << endl;
	  return 0;
	}
	GlobalDoctype = argv[x];
	SetGlobalDoctype = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-gt0")) {
	GlobalDoctype = "";
	SetGlobalDoctype = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-debug")) {
	DebugFlag = 1;
	if (x+1 < argc) {
	  Temp = argv[x+1];
	  Temp.GetCString(Cwd, 256);
	  if (isdigit(Cwd[0])) {
	    Skip = Temp.GetInt();
	    x++;
	  }
	}
	LastUsed = x;
      }
#ifdef DICTIONARY
      if (Flag.Equals("-dict")) {
	DictGen = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-centroid")) {
	DoCentroid = 1;
	LastUsed = x;
      }
#endif
      if (Flag.Equals("-erase")) {
	EraseAll = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-newpaths")) {
	PathChange = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-v")) {
	View = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-vf")) {
	ViewFields = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-vi")) {
	ViewInfo = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-del")) {
	DeleteByKey = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-undel")) {
	UndeleteByKey = 1;
	LastUsed = x;
      }
      if (Flag.Equals("-c")) {
	Cleanup = 1;
	LastUsed = x;
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
  
  //	RECLIST reclist;
  //	RECORD record;
  //	STRING PathName, FileName;
  
  STRING S;
  INT NumWords = argc - x;
  INT z = x;
  for (z=0; z<NumWords; z++) {
    S = argv[z+x];
    WordList.AddEntry(S);
  }
  
	// we need to prevent bad combinations of options, such as -erase and -del together

  PIDBC pdb;
  STRING DBPathName, DBFileName;
  
  DBPathName = DBName;
  DBFileName = DBName;
  RemovePath(&DBFileName);
  RemoveFileName(&DBPathName);
  pdb = new IDBC(DBPathName, DBFileName, DocTypeOptions);
  
  if (DebugFlag) {
    pdb->DebugModeOn();
  }
  
  if (ViewInfo) {
    STRING S;
    INT x, y, z;
    pdb->GetDbFileStem(&S);
    cout << "Database name: " << S << endl;
    pdb->GetGlobalDocType(&S);
    if (S == "") {
      S = "(none)";
    }
    cout << "Global document type: " << S << endl;
    y = pdb->GetTotalRecords();
    cout << "Total number of documents: " << y << endl;
    z = 0;
    for (x=1; x<=y; x++) {
      if (pdb->GetDocumentDeleted(x)) {
	z++;
      }
    }
    cout << "Documents marked as deleted: " << z << endl;
  }
  
  if (!pdb->IsDbCompatible()) {
    cout << "The specified database is not compatible with this version of Iutil." << endl;
    cout << "Please use matching versions of Iindex, Isearch, and Iutil." << endl;
    delete pdb;
    return 0;
  }
  
  if (SetGlobalDoctype) {
    pdb->SetGlobalDocType(GlobalDoctype);
    if (GlobalDoctype == "") {
      cout << "Global document type cleared." << endl;
    } else {
      GlobalDoctype.UpperCase();
      cout << "Global document type set to " << GlobalDoctype << '.' << endl;
    }
  }
  
  if (EraseAll) {
    cout << "Erasing database files ..." << endl;
    pdb->KillAll();
    delete pdb;
    cout << "Database files erased." << endl;
    return 0;
  }
  
  if (PathChange) {
    cout << "Scanning database for file paths ..." << endl;
    cout << "Enter new path or <Return> to leave unchanged:" << endl;
    INT x, y;
    RECORD Record;
    PCHR p;
    STRING OldPath, NewPath;
    STRLIST PathList;
    CHR s[512];
    y = pdb->GetTotalRecords();
    for (x=1; x<=y; x++) {
      pdb->GetDocumentInfo(x, &Record);
      Record.GetPathName(&OldPath);
      p = OldPath.NewCString();
      PathList.GetValue(p, &NewPath);
      delete [] p;
      if (NewPath == "") {
	cout << "Path=[" << OldPath << "]" << endl;
	cout << "    > ";
	cin.getline(s, 512, '\n');
	if (s[0] == '\0') {
	  NewPath = OldPath;
	} else {
	  NewPath = s;
	}
	Record.SetPathName(NewPath);
	OldPath += "=";
	OldPath += NewPath;
	PathList.AddEntry(OldPath);
      } else {
	Record.SetPathName(NewPath);
      }
      pdb->SetDocumentInfo(x, Record);
    }
    cout << "Done." << endl;
  }
  
  if (DeleteByKey) {
    cout << "Marking documents as deleted ..." << endl;
    INT x, z;
    INT y = 0;
    STRING S;
    z = WordList.GetTotalEntries();
    for (x=1; x<=z; x++) {
      WordList.GetEntry(x, &S);
      y += pdb->DeleteByKey(S);
    }
    cout << y << " document(s) marked as deleted." << endl;
  }
  
  if (UndeleteByKey) {
    cout << "Removing deletion mark from documents ..."  << endl;
    INT x, z;
    INT y = 0;
    STRING S;
    z = WordList.GetTotalEntries();
    for (x=1; x<=z; x++) {
      WordList.GetEntry(x, &S);
      y += pdb->UndeleteByKey(S);
    }
    cout << "Deletion mark removed for " << y << " document(s)." << endl;
  }
  
  if (Cleanup) {
    cout << "Cleaning up database (removing deleted documents) ..." << endl;
    INT x = pdb->CleanupDb();
    cout << x << " document(s) were removed." << endl;
  }
  
#ifdef DICTIONARY 
  if (DictGen) {
    cout << "Creating dictionary ..." << endl;
    pdb->CreateDictionary();
  }
  
  if (DoCentroid) {
    cout << "Generating centroid document ..." << endl;
    pdb->CreateCentroid();
  }
#endif
  if (ViewFields) {
    cout << "The following fields are defined in this database:" << endl;
    DFDT Dfdt;
    DFD Dfd;
    STRING S;
    pdb->GetDfdt(&Dfdt);
    INT y = Dfdt.GetTotalEntries();
    INT x;
    for (x=1; x<=y; x++) {
      Dfdt.GetEntry(x, &Dfd);
      Dfd.GetFieldName(&S);
      cout << S << endl;
    }
  }
  
  if (View) {
    cout << "DocType: [Key] (Start - End) File" << endl;
    cout << "(* indicates deleted record)" << endl;
    RECORD Record;
    STRING S;
    INT y = pdb->GetTotalRecords();
    INT x;
    for (x=1; x<=y; x++) {
      pdb->GetDocumentInfo(x, &Record);
      Record.GetDocumentType(&S);
      if (S.Equals("")) {
	cout << "(none)";
      } else {
	S.Print();
      }
      cout << ": [";
      Record.GetKey(&S);
      S.Print();
      cout << "] ";
      cout << '(' << Record.GetRecordStart() << " - " << Record.GetRecordEnd() << ") ";
      Record.GetFullFileName(&S);
      S.Print();
      if (pdb->GetDocumentDeleted(x)) {
	cout << " *";
      }
      cout << endl;
    }
  }
  
  delete pdb;
  return 0;
}
