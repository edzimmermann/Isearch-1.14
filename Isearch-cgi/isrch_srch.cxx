/***********************************************************************
Copyright Notice

Copyright (c) MCNC, Clearinghouse for Networked Information Discovery and
Retrieval, 1996. 

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
File:          	isrch_srch.cxx
Version:        1.04
Description:    CGI app that searches against Iindex-ed databases
Authors:        Kevin Gamiel, kgamiel@cnidr.org
		Tim Gemma, stone@k12.cnidr.org
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
#include "squery.hxx"
#include "rset.hxx"
#include "config.hxx"
#include "cgi-util.hxx"
#include "tokengen.hxx"
#include "infix2rpn.hxx"

#define SIMPLE 0
#define BOOLEAN 1
#define ADVANCED 2

INT Search(PCHR DBPath, PCHR DBName, STRING& query_str, STRING& ESName,
	INT Start, INT MaxHits, INT TYPE);

#define MAXHIT_DEFAULT 50



INT main(int argc, char **argv)
{
  PCHR db, term, field, weight, p, path;
  STRING query, ESName;
  CHR temp[512];
  INT Start, MaxHits, i, type, x, y, z, terms;

  cout << "Content-type: text/html\n\n";
  cout << "<head><title>Isearch Results</title></head>\n<body>" << endl;
  cout << "<a href=\"http://www.cnidr.org/\"><i>CNIDR</a> Isearch-cgi ";
  cout << VERSION << "</i><p>" << endl;

  CGIAPP cgidata;	// Retrieve the forms data!!!
  
  // Good for debugging form values
  //cgidata.Display();

  if ((db = cgidata.GetValueByName("DATABASE")) == NULL) {
    cout << "You must specify a database name.\n";
    exit(0);
  }

  /*
  Which result set record number should be displayed first?
  */
  p = cgidata.GetValueByName("START");
  if (p)
    Start = atoi(p);
  else
    Start = 1;

  /*
  How many result set records should be displayed?
  */
  p=cgidata.GetValueByName("MAXHITS");
  if (p)
    MaxHits = atoi(p);
  else
    MaxHits = MAXHIT_DEFAULT;

  p = cgidata.GetValueByName("ELEMENT_SET");
  if (p)
    ESName = p;
  else
    ESName = "B";

  query = "";

  // If they want URLs returned, there has to be a value for HTTP_PATH
  path=cgidata.GetValueByName("HTTP_PATH");

  p=cgidata.GetValueByName("SEARCH_TYPE");
  if (!p || !strcmp(p,"SIMPLE")) {
    type=SIMPLE;
  } else {
    if (!strcmp(p,"BOOLEAN"))
      type=BOOLEAN;
    if (!strcmp(p,"ADVANCED"))
      type=ADVANCED;
  }

  term = cgidata.GetValueByName("ISEARCH_TERM");
  if (term) {
    query = term;
  } else {
    if ((type==SIMPLE) || (type==BOOLEAN)) {
      terms=0;
    
      for (i=1;i <= 3-type;i++) {
        sprintf(temp, "TERM_%i", i);
        term = cgidata.GetValueByName(temp);
        x=0;
        y=0;
        if (term)
          while (term[x]!='\0') {     // Collapse multiple spaces to  single spaces
            if ((term[x]!=' ') || ((y!=0) && (term[x+1]!=' ') && (term[x+1]!='\0'))) {
              temp[y]=term[x];
              y++;
            }
            x++;
          }
        temp[y]='\0';
        if (term)
          strcpy(term,temp);
  
        sprintf(temp, "FIELD_%i", i);
        if ((field = cgidata.GetValueByName(temp)) != (PCHR)NULL) {
          if (!strcasecmp(field, "FULLTEXT"))
            field = (PCHR)NULL;
        }
    
        sprintf(temp, "WEIGHT_%i", i);
        weight = cgidata.GetValueByName(temp);
    
        if (term) {         // Parse out multiple words in a single term
          x=0;
          y=0;
          z=0;
          strcpy(temp,"");
          if (field) {      // If there's a field, stick at at the beginning
            strcpy(temp,field);
            x=strlen(field)+1;
            temp[x-1]='/';
          }
          while (term[y]!='\0') {   // End of word?  If so, put on the weight
            if (term[y]==' ') {
              if (weight) {
                temp[x]=':';
                strcpy(temp+x+1,weight);
                x=x+1+strlen(weight);
              }
              if (z && (type==BOOLEAN)) {
                strcpy(temp+x," OR");
                x=x+3;
              } else
                z++;
            }
            if ((y!=0) && (term[y-1]==' ') && (field)) {
              strcpy(temp+x,field);
              x=x+strlen(field)+1;
              temp[x-1]='/';
            }
            temp[x]=term[y];
            x++;
            y++;
          }
          if (weight && (temp[0]!='\0')) {  // Slap the weight onto the last term
            temp[x]=':';
            strcpy(temp+x+1,weight);
            x=x+1+strlen(weight);
          }
    
          while (z && (type==BOOLEAN)) {
            strcpy(temp+x," OR");
            x=x+3;
            z--;
          }
          temp[x]='\0';
          if (strlen(temp)>0)
            terms++;
          if (!query.Equals(""))
            query.Cat(" ");
          query.Cat(temp);
    
        }
        term = (PCHR)NULL;
      }
      if (query.Equals("")) {
        cout << "You must enter a query term\n";
        exit(0);
      }
      if ((terms>1) && (type==BOOLEAN)) {
        query.Cat(" ");
        query.Cat(cgidata.GetValueByName("OPERATOR"));
      }
    }
  }
	
  if (query.Equals("")) {
    cout << "You must enter a query term\n";
    exit(0);
  }

  Search(argv[1], db, query, ESName, Start, MaxHits, type);
	
  return 0;
}


/*
DBPath = full path to directory where database files reside, e.g. "/usr/dbs".
DBName = root database name, e.g. "MYDB".
Field = field in which to search.  NULL means full text search.
Term = term(s) for which to search.
ESName = field to display as headline.  NULL will choose whatever is available.
MaxHits = maximum number of hits to display

Returns number of hits on success, -1 on failure
*/
INT Search(PCHR DBPath, PCHR DBName, STRING& query_str, STRING& ESName,
	INT Start, INT MaxHits, INT type)
{
  PRSET prset;
  STRING DBPathName;
  STRING DBRootName;
  SQUERY query;
  IDB *pdb;
  INT HitCount, FieldCount, NextStart, NextCount, i;
  DFDT dfdt, rec_dfdt;
  DFD dfd;
  time_t StartTime, EndTime;
	
  if (ESName.Equals(""))
    ESName = "B";
  switch (type) {
    case ADVANCED: {
      // break the query string into tokens.
      STRLIST PhraseList;
      STRING StrTerm;
      TOKENGEN TokenGen(query_str);
  
      // this loop processes each term.
      query_str = "";
      INT IsBool=0;
      INT TotalPhrases = TokenGen.GetTotalEntries();
      for (i=1;i <= TotalPhrases;i++) {
        TokenGen.GetEntry(i, &StrTerm);
        if ( (StrTerm ^= "AND") || (StrTerm ^= "OR") || (StrTerm ^= "ANDNOT") ||
          (StrTerm == "||") || (StrTerm == "&&") ) {
          IsBool = 1;
        }
        //rebuild the query
        query_str += StrTerm;
        if (i < TotalPhrases)
          query_str += ' ';
      }
      if (IsBool) {
        STRING ProcessedQuery;
  
        INFIX2RPN Parser;
        Parser.Parse(query_str, &ProcessedQuery);
  
        if (!Parser.InputParsedOK()) {
          cout << "The Query <i>" << query_str << "</i>\n";
          cout << "was unparseable. If you think this is an error in this\n";
          cout << "gateway, send mail to <a href=\"mailto:stone@cnidr.org\">";
          cout << "stone@k12.cnidr.org</a>. Please include all relevant\n";
          cout << "information, including the URL of the page you're searching\n";
          cout << "in and the query that you entered.\n";
          exit(0);
        } else {
          query.SetRpnTerm(ProcessedQuery);
        }
      } else
        query.SetTerm(query_str);
      break;
    } 
    case BOOLEAN: query.SetRpnTerm(query_str); break;
    case SIMPLE: query.SetTerm(query_str); break;
  }
 
  // Open database
  DBPathName=DBPath;
  DBRootName=DBName;

  if ((pdb = new IDB(DBPathName, DBRootName)) == NULL) {
    printf("Failed to open database [%s]\n", DBName);
    return -1;
  }
	
  // Is the database valid?
  if (pdb->GetTotalRecords() <= 0) {
    cout << "Database " << DBRootName;
    cout << " does not exist or is corrupted\n";
    return -1;
  }
	
  // Execute the search
  time(&StartTime);
  prset=pdb->Search(query);
  time(&EndTime);

  // How many hits?
  HitCount = prset->GetTotalEntries();		
	
  INT FetchCount = HitCount > (Start + MaxHits - 1) ? MaxHits : (HitCount - Start + 1);

  cout << "<b>Operation Summary</b><p>\n";
  cout << "<i>Matching Record Count:</i> " << HitCount << "<br>\n";
  cout << "<i>Total Retrieved:</i> " << FetchCount << "<br>\n";
  cout << "<i>Interpreted Query:</i> " << query_str << "<br>\n";
  cout << "<i>Database Name:</i> " << DBRootName << "<br>\n";
  cout << "<i>Total Database Records:</i> " << pdb->GetTotalRecords();
  cout << "<br>\n";
  cout << "<i>Query Time:</i> " << (EndTime - StartTime);
  cout << " seconds<br>" << endl;

  if (HitCount == 0) {
    cout << "<p>\n<b>No matches found.</b>\n<p>" << endl;
    return 0;
  } else cout << "<HR>";

  pdb->GetDfdt(&dfdt);
  FieldCount = dfdt.GetTotalEntries();

  // Build the HTML output.  You'll probably want to customize here
  RESULT RsRecord;
  STRING File, RecordKey, Headline, Field, Fullname;
  DOUBLE Score;
  INT j, x, y;
  PCHR url, name, HttpPath;

  HttpPath=(char *)getenv("DOCUMENT_ROOT");

  for (i=Start;i <= (Start + FetchCount - 1);i++) {
    // Fetch the first hit
    prset->GetEntry(i, &RsRecord);
    // Construct a headline for this hit
    pdb->Present(RsRecord, ESName, &Headline);	

    // Get the name of the file
    RsRecord.GetFullFileName(&Fullname);
    RsRecord.GetFileName(&File);
    name=Fullname.NewCString();

    // Find the url
    if (HttpPath) {
      url=strstr(name,HttpPath);
      if (url)
        url=url+strlen(HttpPath);
    } else
      url=(PCHR)NULL;

    // Get the unique database key for this record to be use
    // in subsequent retrieval when URL is clicked.
    RsRecord.GetKey(&RecordKey);

    Score = prset->GetScaledScore(RsRecord.GetScore(),100);
		
    // Construct the URL
    cout << "<b>Filename:</b> ";
    if (url)
      cout << "<a href=\"" << url << "\">";
    cout << File;
    if (url)
      cout << "</a>";
    cout << "<br>" << endl;
    cout << "<b>Match Number:</b> " << i << " of " << HitCount << "<br>" << endl;
    cout << "<b>Score:</b> " << Score << "<br>";
    cout << "<b>Headline Field [";
    cout << ESName;
    cout << "]:</b> ";
    cout << Headline;

    pdb->GetRecordDfdt(RecordKey, &rec_dfdt);
    FieldCount = rec_dfdt.GetTotalEntries();

    cout << "<br><b>Select:</b> ";

    /* Files not within the given WWW path must be accessed with ifetch 
       for their full text */
    if (url==NULL) {
      cout << "<a href=\"ifetch?";
      cout << DBRootName;
      cout << "+";
      cout << RecordKey;
      cout << "+F\">[<i>Full</i>]</a>  ";
    } else 		// Just print the URL
      cout << "<a href=\"" << url << "\">[<i>Full</i>]</a>  ";

    // Provide optional retrieval of each field we know about
    for(j=1;j <= FieldCount;j++) {
      rec_dfdt.GetEntry(j, &dfd);
      dfd.GetFieldName(&Field);
      cout << "<a href=\"ifetch?";
      cout << DBRootName;
      cout << "+";
      cout << RecordKey;
      cout << "+";
      cout << Field;
      cout << "\">[<i>";
      cout << Field;
      cout << "</i>]</a>  ";
    }

    if ((i + 1) <= HitCount)
      cout << "<HR>";
  }

  /*
  If we were unable to display all hits, lets give them a hyperlink
  to display the next MaxHits of them.
  */
  INT Remaining = (HitCount - (Start + MaxHits - 1));
  if (Remaining > 0) {
    NextStart = Start + MaxHits;
    NextCount = Remaining < MaxHits ? Remaining : MaxHits;

    // Display a mini-form with CGI variables
    cout << "<FORM ACTION=\"/cgi-bin/isearch\" METHOD=\"POST\">\n";
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"START\" VALUE=\"";
    cout << NextStart << "\">";
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"MAXHITS\" VALUE=\"";
    cout << MaxHits << "\">";
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"ISEARCH_TERM\" VALUE=\"";
    cout << query_str << "\">";
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"SEARCH_TYPE\" VALUE=\"";
    switch (type) {
      case SIMPLE: cout << "SIMPLE"; break;
      case BOOLEAN: cout << "BOOLEAN"; break;
      case ADVANCED: cout << "ADVANCED"; break;
    }
    cout << "\">";
    if (HttpPath) {
      cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"HTTP_PATH\" VALUE=\"";
      cout << HttpPath << "\">";
    }
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"ELEMENT_SET\" VALUE=\"";
    cout << ESName << "\">";
    cout << "<INPUT TYPE=\"HIDDEN\" NAME=\"DATABASE\" VALUE=\"";
    cout << DBName << "\">";
    cout << "<INPUT TYPE=\"SUBMIT\" VALUE=\"Retrieve Next ";
    cout << NextCount << "\">\n";
    cout << "</FORM>" << endl;
  }
  cout << "\n\n</body>\n" << endl;

  return HitCount;
}
