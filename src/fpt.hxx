/*@@@
File:		fpt.hxx
Version:	1.00
Description:	Class FPT - File Pointer Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef FPT_HXX
#define FPT_HXX

#include "defs.hxx"
#include "string.hxx"
#include "fprec.hxx"

typedef PFILE* PPFILE;

class FPT {
public:
	FPT();
	FPT(const INT TableSize);
	PFILE ffopen(const STRING& FileName, const PCHR Type);
	INT ffclose(PFILE FilePointer);
	void CloseAll();
	~FPT();
private:
	void Init(const INT TableSize);
	INT Lookup(const STRING& FileName);
	INT Lookup(const PFILE FilePointer);
	void HighPriority(const INT Index);
	void LowPriority(const INT Index);
	PFPREC Table;
	INT TotalEntries;
	INT MaximumEntries;
};

typedef FPT* PFPT;

#endif
