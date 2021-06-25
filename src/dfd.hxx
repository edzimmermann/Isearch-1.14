/*@@@
File:		dfd.hxx
Version:	1.00
Description:	Class DFD - Data Field Definition
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DFD_HXX
#define DFD_HXX

#include "defs.hxx"
#include "string.hxx"
#include "attrlist.hxx"

class DFD {
public:
	DFD();
	DFD& operator=(const DFD& OtherDfd);
	void SetFieldName(const STRING& NewFieldName);
	void GetFieldName(PSTRING StringBuffer) const;
	void SetFileNumber(const INT NewFileNumber);
	INT GetFileNumber() const;
	void SetAttributes(const ATTRLIST& NewAttributes);
	void GetAttributes(PATTRLIST AttributesBuffer) const;
	~DFD();
private:
//	STRING FieldName;
	INT FileNumber;
	ATTRLIST Attributes;
};

typedef DFD* PDFD;

#endif
