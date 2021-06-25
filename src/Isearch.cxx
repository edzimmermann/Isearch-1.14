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
File:		Isearch.cxx
Version:	1.01
Description:	Command-line search utility
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <iomanip.h>
#include <locale.h>

#include "idb.hxx"
#include "string.hxx"
#include "common.hxx"
#include "squery.hxx"
#include "rset.hxx"
#include "dtreg.hxx"
#include "infix2rpn.hxx"

int main(int argc, char** argv) {
	if (argc < 2) {
		cout << endl << "Isearch, Version " << IsearchVersion;
		cout << ", Copyright (c) 1995 MCNC/CNIDR" << endl << endl;
		cout << "Isearch [-d (X)]  // Search database with root name (X)." << endl;
		cout << "        [-p (X)]  // Present element set (X) with results." << endl;
		cout << "        [-q]  // Print results and exit immediately." << endl;
		cout << "        [-and]  // Perform boolean \"and\" on results." << endl;
		cout << "        [-rpn]  // Interpret as an RPN query." << endl;
		cout << "        [-infix]  // Interpret as a boolean algebra query." << endl;
		cout << "        [-prefix (X)]  // Add prefix (X) to matched terms in document." << endl;
		cout << "        [-suffix (X)]  // Add suffix (X) to matched terms in document." << endl;
		cout << "        [-byterange]  // Print the byte range of each document within" << endl;
		cout << "                      // the file that contains it." << endl;
		cout << "        [-startdoc (X)]  // Display result set starting with the (X)th" << endl;
		cout << "                         // document in the list." << endl;
		cout << "        [-enddoc (X)]  // Display result set ending with the (X)th document" << endl;
		cout << "                       // in the list." << endl;
		cout << "        [-o (X)]  // Document type specific option." << endl;
		cout << "        (X) (Y) (...)  // Search for words (X), (Y), etc." << endl;
		cout << "                       // [fieldname/]searchterm[*][:n]" << endl;
		cout << "                       // Prefix with fieldname/ for fielded searching." << endl;
		cout << "                       // Append * for right truncation." << endl;
//		cout << "                       // Append ~ for soundex search." << endl;
		cout << "                       // Append :n for term weighting (default=1)." << endl;
		cout << "                       //   (Use negative values to lower rank.)" << endl;
		cout << endl;
		cout << "Examples: Isearch -d POETRY truth \"beaut*\" urn:2" << endl;
		cout << "          Isearch -d WEBPAGES title/library" << endl;
		cout << "          Isearch -d STORIES -rpn title/cat title/dog or title/mouse and" << endl;
		cout << "          Isearch  -d PRUFROCK -infix '(ether and table) or mermaids'" << endl;
		cout << "          Isearch  -d BIBLE -infix '(Saul||Goliath)&&David'" << endl;
		cout << "                               // (title/cat or title/dog) and title/mouse" << endl << endl;
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
		cout << "Isearch " << IsearchVersion << endl;
	}

  if (!setlocale(LC_CTYPE,"")) {
    cout << "Warning: Failed to set the locale!" << endl;
  }

	STRLIST DocTypeOptions;
	STRING Flag;
	STRING DBName;
	STRING ElementSet;
	STRING TermPrefix, TermSuffix;
	STRING StartDoc, EndDoc;
	INT DebugFlag = 0;
	INT QuitFlag = 0;
	INT ByteRangeFlag = 0;
	INT BooleanAnd = 0;
	INT RpnQuery = 0;
	INT InfixQuery = 0;
	INT x = 0;
	INT LastUsed = 0;
	ElementSet = "B";
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
			if (Flag.Equals("-p")) {
				if (++x >= argc) {
					cout << "ERROR: No element set specified after -p." << endl << endl;
					return 0;
				}
				ElementSet = argv[x];
				LastUsed = x;
			}
			if (Flag.Equals("-prefix")) {
				if (++x >= argc) {
					cout << "ERROR: No prefix specified after -prefix." << endl << endl;
					return 0;
				}
				TermPrefix = argv[x];
				LastUsed = x;
			}
			if (Flag.Equals("-suffix")) {
				if (++x >= argc) {
					cout << "ERROR: No suffix specified after -suffix." << endl << endl;
					return 0;
				}
				TermSuffix = argv[x];
				LastUsed = x;
			}
			if (Flag.Equals("-startdoc")) {
				if (++x >= argc) {
					cout << "ERROR: No value specified after -startdoc." << endl << endl;
					return 0;
				}
				StartDoc = argv[x];
				LastUsed = x;
			}
			if (Flag.Equals("-enddoc")) {
				if (++x >= argc) {
					cout << "ERROR: No value specified after -enddoc." << endl << endl;
					return 0;
				}
				EndDoc = argv[x];
				LastUsed = x;
			}
			if (Flag.Equals("-q")) {
				QuitFlag = 1;
				LastUsed = x;
			}
			if (Flag.Equals("-byterange")) {
				ByteRangeFlag = 1;
				LastUsed = x;
			}
			if (Flag.Equals("-and")) {
				BooleanAnd = 1;
				LastUsed = x;
			}
			if (Flag.Equals("-rpn")) {
				RpnQuery = 1;
				LastUsed = x;
			}
			if (Flag.Equals("-infix")) {
				InfixQuery = 1;
				LastUsed = x;
			}
			if (Flag.Equals("-debug")) {
				DebugFlag = 1;
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

	if ( (RpnQuery) && (BooleanAnd) ) {
		cout << "ERROR: The -rpn and -and options can not be used together." << endl;
	}
	if ( (InfixQuery) && (BooleanAnd) ) {
		cout << "ERROR: The -infix and -and options can not be used together." << endl;
		return 0;
	}
	if ( (RpnQuery) && (InfixQuery) ) {
		cout << "ERROR: The -rpn and -infix options can not be used together." << endl;
		return 0;
    }


	// this should be in the engine
	PFILE fp;
	STRING temp;
	temp = DBName;
	temp.Cat(".inx");
	fp = fopen(temp, "rb");
	if (fp) {
		fclose(fp);
	} else {
		cout << "ERROR: Database not found." << endl;
		return 0;
	}
	temp = DBName;
	temp.Cat(".mdt");
	fp = fopen(temp, "rb");
	if (fp) {
		fclose(fp);
	} else {
		cout << "ERROR: Database not found." << endl;
		return 0;
	}

	x = LastUsed + 1;
	if (x >= argc) {
		return 0;
	}

	INT NumWords = argc - x;
	INT z = x;
//	STRING WordList[NumWords];
	PSTRING WordList = new STRING[NumWords];
	for (z=0; z<NumWords; z++) {
		WordList[z] = argv[z+x];
	}

	PIDB pdb;
	STRING DBPathName, DBFileName;
	STRING PathName, FileName;
	SQUERY squery;
	PRSET prset;
	RESULT result;
	INT t, n;

	DBPathName = DBName;
	DBFileName = DBName;
	RemovePath(&DBFileName);
	RemoveFileName(&DBPathName);
	pdb = new IDB(DBPathName, DBFileName, DocTypeOptions);

	if (DebugFlag) {
		pdb->DebugModeOn();
	}

	if (!pdb->IsDbCompatible()) {
		cout << "The specified database is not compatible with this version of Isearch." << endl;
		cout << "Please use matching versions of Iindex, Isearch, and Iutil." << endl;
		delete [] WordList;
		delete pdb;
		return 0;
	}

	cout << "Searching database ";
	DBName.Print();
	cout << ':' << endl;
	STRING QueryString;
	for (z=0; z<NumWords; z++) {
		if (z != 0) {
			QueryString.Cat(' ');
		}
		QueryString.Cat(WordList[z]);
	}
	if (RpnQuery || InfixQuery) {
		if (InfixQuery) {
			STRING TempString;
			INFIX2RPN *Parser;
			Parser = new INFIX2RPN(QueryString, &TempString);
			if (Parser->InputParsedOK()) {
				QueryString = TempString;
				delete Parser;
			}
			else {
				if (Parser->GetErrorMessage(&TempString)) {
					cout << "INFIX QUERY ERROR : " << TempString << endl;
					return 0;
				}
				else {
					cout << "INFIX QUERY ERROR: Unable to parse"  << endl;
					return 0;
				}
			}
		}
		squery.SetRpnTerm(QueryString);
	} else {
		squery.SetTerm(QueryString);
	}
	if (BooleanAnd) {
		prset = pdb->AndSearch(squery);
	} else {
		prset = pdb->Search(squery);
	}
	n = prset->GetTotalEntries();

	STRING RecordSyntax = SutrsRecordSyntax;
	pdb->BeginRsetPresent(RecordSyntax);

	cout << endl << n << " document(s) matched your query, ";

	// Display only documents in -startdoc/-enddoc range
	INT x1, x2;
	x1 = StartDoc.GetInt();
	x2 = EndDoc.GetInt();
	if ( (x1 != 0) || (x2 != 0) ) {
		PRSET NewPrset = new RSET();
		for (t=x1; t<=x2; t++) {
			if ( (t >= 1) && (t <= n) ) {
				prset->GetEntry(t, &result);
				NewPrset->AddEntry(result);
			}
		}
		delete prset;
		prset = NewPrset;
	}
	n = prset->GetTotalEntries();
	cout << n << " document(s) displayed." << endl << endl;

	CHR Selection[80];
	CHR s[256];
	INT FileNum;
	STRING BriefString;
	STRING Element, TempElementSet;
	GDT_BOOLEAN FirstRun = GDT_TRUE;
	STRLIST BriefList;
	STRING TotalBrief;
	do {
		if (n != 0) {
			cout << "      Score   File" << endl;
		}
		for (t=1; t<=n; t++) {
			prset->GetEntry(t, &result);
			cout << setw(4) << t << '.';
			cout << setw(6) << prset->GetScaledScore(result.GetScore(), 100) << "   ";
			result.GetPathName(&PathName);
			result.GetFileName(&FileName);
			PathName.Print();
			FileName.Print();
			cout << endl;
			if (ByteRangeFlag) {
				cout << "              [ " << result.GetRecordStart() <<
						" - " << result.GetRecordEnd() << " ]" << endl;
			}
			if (FirstRun) {
				TotalBrief = "";
				TempElementSet = ElementSet;
				while (!TempElementSet.Equals("")) {
					Element = TempElementSet;
					if ( (x=TempElementSet.Search(',')) ) {
						Element.EraseAfter(x-1);
						TempElementSet.EraseBefore(x+1);
					} else {
						TempElementSet = "";
					}
					pdb->Present(result, Element, RecordSyntax,
							&BriefString);
					TotalBrief += BriefString;
				}
				BriefList.AddEntry(TotalBrief);
			} else {
				BriefList.GetEntry(t, &TotalBrief);
			}
			if (TotalBrief.GetLength() > 0) {
				cout << TotalBrief << endl;
			}
		}
		pdb->EndRsetPresent(RecordSyntax);
		if ( (QuitFlag) || (n == 0) ) {
			FileNum = 0;
		} else {
			cout << endl << "Select file #: ";
			cin.getline(Selection, 5, '\n');
			FileNum = atoi(Selection);
		}
		if ( (FileNum > n) || (FileNum < 0) ) {
			cout << endl << "Select a number between 1 and " << n << '.' << endl;
		}
		if ( (FileNum != 0) && (FileNum <= n) && (FileNum >= 1) ) {
			prset->GetEntry(FileNum, &result);

			STRING Buf;
			STRING Full;
			Full = "F";
			if ( (TermPrefix.Equals("")) && (TermSuffix.Equals("")) ) {
				pdb->Present(result, Full, RecordSyntax, &Buf);
			} else {
				result.GetHighlightedRecord(TermPrefix, TermSuffix, &Buf);
			}
			Buf.Print();

			cout << "Press <Return> to select another file: ";
			cin.getline(s, 5, '\n');
			cout << endl;
		}
		if (FirstRun == GDT_TRUE) {
			FirstRun = GDT_FALSE;
		}
	} while (FileNum != 0);

	delete [] WordList;
	delete prset;
	delete pdb;
	return 0;
}
