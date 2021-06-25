/*

File:        ftp.cxx
Version:     1
Description: class FTP - index files based on their filename
Author:      Erik Scott, Scott Technologies, Inc.
*/

#include <ctype.h>
#include "ftp.hxx"

FTP::FTP(PIDBOBJ DbParent) : DOCTYPE(DbParent) {
}




void FTP::Present(const RESULT& ResultRecord, const STRING& ElementSet,
		STRING* StringBufferPtr) {
	
*StringBufferPtr = "";
// Basic strategy:  on a "B" present, show the first line.  On an "F" present,
// show everything *but* the first line.  Simple enough, right?

STRING myBuff;
ResultRecord.GetRecordData(&myBuff);
STRINGINDEX firstNL = myBuff.Search('\n');
if (firstNL == 0) {
   cout << "FTP::Present() -- Can't find first Newline in file to present.\n";
   return;
   }

if (ElementSet.Equals("F")) {
   myBuff.EraseBefore(firstNL+1);
   }
else if (ElementSet.Equals("B")) {
   myBuff.EraseAfter(firstNL);
   }

*StringBufferPtr = myBuff;


}


FTP::~FTP() {
}
