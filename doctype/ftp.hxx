/*

File:        ftp.hxx
Version:     1
Description: class FTP - first line is headline, rest is real body
Author:      Erik Scott, Scott Technologies, Inc.
*/


#ifndef FTP_HXX
#define FTP_HXX

#ifndef DOCTYPE_HXX
#include "defs.hxx"
#include "doctype.hxx"
#endif

class FTP : public DOCTYPE {
public:
   FTP(PIDBOBJ DbParent);
   //void ParseRecords(const RECORD& FileRecord);
   void Present(const RESULT& ResultRecord, const STRING& ElementSet,
                PSTRING StringBuffer);
   ~FTP();
};

typedef FTP* PFTP;

#endif
