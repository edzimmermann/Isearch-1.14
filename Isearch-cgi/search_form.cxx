/***********************************************************************
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
File:          	search_form.cxx
Version:        1.03
Description:    CGI app that builds a search html form for Iindex-ed databases
Author:         Kevin Gamiel, kgamiel@cnidr.org
@@@*/

#include <iostream.h>
#include "gdt.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "idb.hxx"
#include "string.hxx"
#include "common.hxx"
#include "config.hxx"

#define SIMPLE 1
#define BOOLEAN 2
#define ADVANCED 3

int main(int argc, char **argv)
{
  PCHR db, dbpath, www;
  CHR temp[64];
  INT type;

  if (argc < 3) {
    cout << "This file produces a custom HTML search page based upon a ";
    cout << "specified Isearch database.\n" << endl;
    cout << "Usage:  search_form [-page type] <dbpath> <dbname>\n";
    cout << "\nThe following page types are available:\n";
    cout << "  -simple\n  -boolean\n  -advanced" << endl;
    exit(0);
  }
  type=0;
  strcpy(temp,argv[1]);
  if (temp[0]=='-') {
    if (!strcmp(temp,"-simple")) {
      type=SIMPLE;
    } else {
      if (!strcmp(temp,"-boolean")) {
        type=BOOLEAN;
      } else {
        if (!strcmp(temp,"-advanced")) {
          type=ADVANCED;
        } else {
          cout << "Form type " << temp << " not recognized.\n\n";
          exit(0);
        }
      }
    }
    dbpath = argv[2];
    db = argv[3];
    www = argv[4];
  } else {
    dbpath = argv[1];
    db = argv[2];
    www = argv[3];
  }


	
  STRING DBPathName;
  STRING DBRootName;
  IDB *pdb;
  INT FieldCount;
  DFDT dfdt;
  DFD dfd;
	
  // Open database
  if (type>0) {
    DBPathName = argv[2];
    DBRootName = argv[3];
  } else {
    DBPathName = argv[1];
    DBRootName = argv[2];
    type=SIMPLE;
  }

  pdb = new IDB(DBPathName, DBRootName);
	
  // Is the database valid?
  if (pdb->GetTotalRecords() <= 0) {
    cout << "Database " << DBRootName;
    cout << " does not exist or is corrupted\n";
    exit(1);
  }

  cout << "<html><head>";

  switch (type) {
    case SIMPLE: {
      cout << "<title>Simple Search Page</title>";
      cout << "</head>\n<body>\n";
      cout << "<h2>Simple Search</h2>";
      break;}
    case BOOLEAN: {
      cout << "<title>Boolean Search Page</title>";
      cout << "</head>\n<body>\n";
      cout << "<h2>Boolean Search</h2>";
      break; }
    case ADVANCED: {
      cout << "<title>Advanced Search Page</title>";
      cout << "</head>\n<body>\n";
      cout << "<h2>Advanced Search</h2>";
      break; }
  }
  cout << "<!-- Isearch-cgi search page, produced by Isearch-cgi's search_form -->\n";
  cout << "<a href=\"http://www.cnidr.org/\"><i>CNIDR</a> Isearch-cgi ";
  cout << VERSION << "</i><p>" << endl;

  cout << "Database Name: " << DBRootName << "<br>\n";
  cout << "Total Records: " << pdb->GetTotalRecords() << "<br>\n";

  cout << "<form method=POST action=\"/cgi-bin/isearch\">\n";
  
  cout << "<input name=\"DATABASE\" type=hidden value=\"" << DBRootName;
  cout << "\">\n";
  cout << "<input name=\"SEARCH_TYPE\" type=hidden value=\"";
  switch (type) {
    case SIMPLE: cout << "SIMPLE"; break;
    case BOOLEAN: cout << "BOOLEAN"; break;
    case ADVANCED: cout << "ADVANCED"; break;
  }
  cout << "\">\n";
  if (www) {
    cout << "<input name=\"HTTP_PATH\" type=hidden value=\"" << www;
    cout << "\">\n";
  }

  pdb->GetDfdt(&dfdt);
  FieldCount = dfdt.GetTotalEntries();

  STRING Field;
  INT j;

  switch (type) {
    case SIMPLE: {
      for (INT i=1;i <= 3;i++) {
        sprintf(temp, "FIELD_%i", i);
        cout << "Field: <select name=\"" << temp << "\">\n";
        cout << "<option selected value=\"FULLTEXT\">FullText\n";
    
        for(j=1;j <= FieldCount;j++) {
          dfdt.GetEntry(j, &dfd);
          dfd.GetFieldName(&Field);
          cout << "<option>" << Field << "\n";
        }
        cout << "</select>\n\n";
        sprintf(temp, "TERM_%i", i);
        cout << "Term: <input name=\"" << temp << "\">\n";
        sprintf(temp, "WEIGHT_%i", i);
        cout << "Weight: <select name=\"" << temp << "\">\n";
        cout << "<option selected>1\n";
        cout << "<option>2\n";
        cout << "<option>3\n";
        cout << "</select>\n";
    
        cout << "<br>\n";	
      }
      break;
    }
    case BOOLEAN: {
      for (INT i=1;i <= 2;i++) {
        sprintf(temp, "FIELD_%i", i);
        cout << "Field: <select name=\"" << temp << "\">\n";
        cout << "<option selected value=\"FULLTEXT\">FullText\n";
    
        for(j=1;j <= FieldCount;j++) {
          dfdt.GetEntry(j, &dfd);
          dfd.GetFieldName(&Field);
          cout << "<option>" << Field << "\n";
        }
        cout << "</select>\n\n";
        sprintf(temp, "TERM_%i", i);
        cout << "Term: <input name=\"" << temp << "\">\n";
        sprintf(temp, "WEIGHT_%i", i);
        cout << "Weight: <select name=\"" << temp << "\">\n";
        cout << "<option selected>1\n";
        cout << "<option>2\n";
        cout << "<option>3\n";
        cout << "</select>\n";
  
        if (i==1) {
          cout << "<p>\n<ul>\n<select name=\"OPERATOR\">\n";
          cout << "<option selected>AND\n<option>OR\n<option>ANDNOT" << endl;
          cout << "</select>\n</ul>\n<p>" << endl;
        } else 
          cout << "<br>\n";	
      }
      break;
    }
    case ADVANCED: {
      cout << "<b>Query:</b>" << endl;
      cout << "<textarea rows=2 cols=75 maxcols=80 name=\"ISEARCH_TERM\">" << endl;
      cout << "</textarea><br>" << endl;
      break;
    }
  }


  cout << "<HR>\n\n";

  cout << "Select Field to Display as Headline: ";
  cout << "<select name=\"ELEMENT_SET\">\n";
  if(FieldCount == 0) {
    cout << "<option selected value=\"B\">Brief\n";
  } 
  for(j=1;j <= FieldCount;j++) {
    dfdt.GetEntry(j, &dfd);
    dfd.GetFieldName(&Field);
    cout << "<option>" << Field << "\n";
  }
  cout << "</select>\n";
  cout << "<p>\n";
  cout << "Enter maximum number of hits to retrieve: ";
  cout << "<input size=4 name=\"MAXHITS\" value=\"50\"><p>\n";
  cout << "<input type=\"submit\" value=\" Submit Query \">";
  cout << "<input type=reset value=\" Clear Entries \">";

  cout << "</form>\n\n";

  if (type==ADVANCED) {
    cout << "<hr>\nExamples of advanced searches:<ul>" << endl;
    cout << "<li>roses and red" << endl;
    cout << "<li>(title/dogs and h1/cats) or h3/monkeys" << endl;
    cout << "<li>(img/stars and img/stripes) or (bald and eagle)\n</ul>" << endl;
  }
  cout << "</body>\n</html>\n";

 return 0;
}
