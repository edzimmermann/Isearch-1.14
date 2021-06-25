/*@@@
File:		dfdt.hxx
Version:	1.00
Description:	Class DFDT - Data Field Definitions Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DFDT_HXX
#define DFDT_HXX

#include "defs.hxx"
#include "dfd.hxx"

class DFDT {
public:
	DFDT();
	DFDT& operator=(const DFDT& OtherDfdt);
	void Initialize();
	void LoadTable(const STRING& FileName);
	void SaveTable(const STRING& FileName);
	void AddEntry(const DFD& DfdRecord);
	void GetEntry(const INT Index, PDFD DfdRecord) const;
	void GetDfdRecord(const STRING& FieldName, PDFD DfdRecord) const;
	INT GetNewFileNumber() const;
	void Expand();
	void CleanUp();
	void Resize(const INT Entries);
	INT GetTotalEntries() const;
	INT GetChanged() const;
	~DFDT();
private:
	PDFD Table;
	INT TotalEntries;
	INT MaxEntries;
	INT Changed;
};

typedef DFDT* PDFDT;

#endif
