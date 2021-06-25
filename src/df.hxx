/*@@@
File:		df.hxx
Version:	1.00
Description:	Class DF - Data Field
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef DF_HXX
#define DF_HXX

#include "defs.hxx"
#include "string.hxx"
#include "fct.hxx"

class DF {
public:
	DF();
	DF& operator=(const DF& OtherDf);
	void SetFieldName(const STRING& NewFieldName);
	void GetFieldName(PSTRING StringBuffer) const;
	void SetFct(const FCT& NewFct);
	void GetFct(PFCT FctBuffer) const;
	void Write(PFILE fp) const;
	void Read(PFILE fp);
	~DF();
private:
	STRING FieldName;
	FCT Fct;
};

typedef DF* PDF;

#endif
