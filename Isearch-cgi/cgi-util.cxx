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
File:          	cgi-util.cxx
Version:        1.04
Description:    CGI utilities
Authors:        Kevin Gamiel, kgamiel@cnidr.org
		Tim Gemma, stone@k12.cnidr.org
@@@*/

#include <string.h>
#include <ctype.h>
#include "cgi-util.hxx"

/*
# Class: CGIAPP
# Method: GetInput
# Comments: Reads Forms data from standard input, parses out arguments.
*/

void CGIAPP::GetInput() {
  INT ContentLen, x, y, z, len;
  CHR temp1[256], temp2[256], temp3[256];
  PCHR meth, p, query;

  if ((meth = (char *)getenv("REQUEST_METHOD"))==NULL) {
    cout << "Unable to get request_method" << endl;
    exit(1);
  }
  if (!strcmp(meth,"POST")) {
    if ((p=(char *)getenv("CONTENT_LENGTH")))
      ContentLen = atoi(p);
    Method=POST;
  } else {
    if (!strcmp(meth,"GET")) {
      query = (char *)getenv("QUERY_STRING");
      Method=GET;
    } else {
      cout << "This program is to be referenced with a METHOD of POST or GET.\n";
      cout << "If you don't understand this, see this ";
      cout << "<A HREF=\"http://www.ncsa.uiuc.edu/SDG/Software/Mosaic/";
      cout << "Docs/fill-out-forms/overview.html\">forms overview</A>" << endl;
      exit (1);
    }
  }
  /* Build the list of cgi entries */
  if (Method==POST) {
    entry_count=0;
    for (x = 0; ContentLen>0; x++) {
      cin.getline(temp1,ContentLen+1,'&');
      entry_count++;
      len=strlen(temp1);
      ContentLen=ContentLen-(len+1);
      for (y=0;temp1[y]!='=' && y<len;y++) {
        temp2[y]=temp1[y];
      }
      temp2[y]='\0';
      y++;
      for (z=0;y<len;y++) {
        temp3[z]=temp1[y];
        z++;
      }
      temp3[z]='\0';
      plustospace(temp3);
      unescape_url(temp3);
      name[x]=new CHR[strlen(temp2)+1];
      strcpy(name[x],temp2);
      value[x]=new CHR[strlen(temp3)+1];
      strcpy(value[x],temp3);
    }
  } else {  /* Get */
    entry_count=0;
    y=0;
    z=0;
    plustospace(query);
    unescape_url(query);
    len=strlen(query);
    for (x=0;y<len;x++) {
      while ((query[y]!='=') && (query[y]!='&') && (y<len)) {
        temp1[z]=query[y];
        z++;
        y++;
      }
      temp1[z]='\0';
      z=0;
      if (query[y]=='=') {
        y++;
        while ((query[y]!='&') && (y<len)) {
          temp2[z]=query[y];
          z++;
          y++;
        }
      }
      y++;
      temp2[z]='\0';
      if (temp2[0]=='\0') {
        name[x]=new CHR[1];
        strcpy(name[x],"");
        value[x]=new CHR[strlen(temp1)+1];
        strcpy(value[x],temp1);
      } else {
        name[x]=new CHR[strlen(temp1)+1];
        strcpy(name[x],temp1);
        value[x]=new CHR[strlen(temp2)+1];
        strcpy(value[x],temp2);
      }
      entry_count++;
      z=0;
    }
  }  /* Get */
}


CGIAPP::CGIAPP() {
  GetInput();
}

void CGIAPP::Display() {
  INT x;
  for (x=0;x<entry_count;x++) {
    cout << name[x] << " = " << value[x] << "<br>\n";
  }
}

PCHR CGIAPP::GetName(INT4 i) {
  return name[i];
}

PCHR CGIAPP::GetValue(INT4 i) {
  return value[i];
}

PCHR CGIAPP::GetValueByName(const PCHR field) {
  if ((field==NULL) || (field[0]=='\0'))
    return NULL;
  INT i;
  for (i=0;i<entry_count;i++) {
    if ((name[i]==NULL)||(value[i]==NULL))
      return NULL;
    if (!strcmp(name[i], field)) {
      if (value[i] != NULL) {
        return value[i];
      }
      return NULL;
    }
  }
  return NULL;

}

CGIAPP::~CGIAPP() {
  delete [] name;
  delete [] value;
}


CHR x2c(PCHR what) {
    CHR digit;
    digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
    digit *= 16;
    digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
    return(digit);
}


void unescape_url(PCHR url) {
  INT x,y;
  for(x=0,y=0;url[y];++x,++y) {
    if((url[x] = url[y]) == '%') {
      url[x] = x2c(&url[y+1]);
      y+=2;
    }
  }
  url[x] = '\0';
}

void plustospace(PCHR str) {
  INT x;
  for(x=0;str[x];x++)
    if(str[x] == '+')
      str[x] = ' ';
}

void spacetoplus(PCHR str) {
  INT x;
  for(x=0;str[x];x++)
    if(str[x] == ' ')
      str[x] = '+';
}

PCHR c2x(CHR what) {
  PCHR out=new CHR[4];
  sprintf(out, "%%%2x", what);
  return out;
}

void escape_url(PCHR url, PCHR out) {
  out[0] = '\0';
  INT x, y;
  for(x=0,y=0;url[x];++x,++y) {
    if (isalnum(url[x]) || (url[x] == ' ')) {
      out[y] = url[x];
    } else {
      PCHR esc = c2x(url[x]);
      INT plus = strlen(esc);
      out[y] = '\0';
      strcat(out, esc);
      y += (plus - 1);
      delete esc;
    }
  }
  spacetoplus(out);
}


