/*@@@
File:		fct.hxx
Version:	1.00
Description:	Class FCT - Field Coordinate Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef FCT_HXX
#define FCT_HXX

#include "defs.hxx"
#include "fc.hxx"
#include "vlist.hxx"

class FCT : public VLIST {
public:
	FCT();
	FCT& operator=(const FCT& OtherFct);
	void AddEntry(const FC& FcRecord);
	void GetEntry(const INT Index, FC* FcRecord) const;
	void SortByFc();
	void Write(PFILE fp) const;
	void Read(PFILE fp);
	void Print(ostream& Os) const;
	void SubtractOffset(const GPTYPE GpOffset);
	friend ostream& operator<<(ostream& os, const FCT& Fct);
private:
	FC Fc;
};

typedef FCT* PFCT;

#endif
