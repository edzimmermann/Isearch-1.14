/*@@@
File:		dft.hxx
Version:	1.00
Description:	Class DFT - Data Field Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DFT_HXX
#define DFT_HXX

#include "defs.hxx"
#include "df.hxx"

class DFT {
public:
	DFT();
	void Init();
	DFT& operator=(const DFT& OtherDft);
	void AddEntry(const DF& DfRecord);
	void GetEntry(const INT Index, PDF DfRecord) const;
	void Expand();
	void CleanUp();
	void Resize(const INT Entries);
	INT GetTotalEntries() const;
	void Write(PFILE fp) const;
	void Read(PFILE fp);
	~DFT();
private:
	PDF Table;
	INT TotalEntries;
	INT MaxEntries;
};

typedef DFT* PDFT;

#endif
