/*

File:        para.hxx
Version:     1
Description: class PARA - index paragraphs
Author:      Erik Scott, Scott Technologies, Inc.
*/


#ifndef PARA_HXX
#define PARA_HXX

#ifndef DOCTYPE_HXX
#include "defs.hxx"
#include "doctype.hxx"
#endif

class PARA : public DOCTYPE {
public:
   PARA(PIDBOBJ DbParent);
   void ParseRecords(const RECORD& FileRecord);
//   void Present(const RESULT& ResultRecord, const STRING& ElementSet,
//                PSTRING StringBuffer);
   ~PARA();
};

typedef PARA* PPARA;

#endif
