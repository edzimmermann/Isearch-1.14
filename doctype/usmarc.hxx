/*

File:        usmarc.hxx
Version:     1
Description: class USMARC - MARC records for library use
Author:      Erik Scott, Scott Technologies, Inc.
*/


#ifndef USMARC_HXX
#define USMARC_HXX

#ifndef DOCTYPE_HXX
#include "defs.hxx"
#include "doctype.hxx"
#endif

class USMARC : public DOCTYPE {
public:
   USMARC(PIDBOBJ DbParent);
   void ParseRecords(const RECORD& FileRecord);   
   void ParseFields(PRECORD NewRecord);
   GPTYPE ParseWords(CHR* DataBuffer, INT DataLength, INT DataOffset, GPTYPE* GpBuffer, INT GpLength);
   void Present(const RESULT& ResultRecord, const STRING& ElementSet,
                const STRING& RecordSyntax, STRING* StringBuffer);
   ~USMARC();
};

typedef USMARC* PUSMARC;

#endif
