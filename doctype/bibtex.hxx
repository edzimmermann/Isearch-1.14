/*

File:        bibtex.hxx
Version:     1
Description: class BIBTEX - index BibTEX files
Author:      Erik Scott, Scott Technologies, Inc.
*/


#ifndef BIBTEX_HXX
#define BIBTEX_HXX

#ifndef DOCTYPE_HXX
#include "defs.hxx"
#include "doctype.hxx"
#endif

class BIBTEX : public DOCTYPE {
public:
   BIBTEX(PIDBOBJ DbParent);
   void ParseRecords(const RECORD& FileRecord);
   void ParseFields(PRECORD NewRecord);
   void Present(const RESULT& ResultRecord, const STRING& ElementSet,
                PSTRING StringBuffer);
   ~BIBTEX();
};

typedef BIBTEX* PBIBTEX;

#endif
