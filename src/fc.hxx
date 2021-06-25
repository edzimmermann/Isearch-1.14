/*@@@
File:		fc.hxx
Version:	1.00
Description:	Class FC - Field Coordinates
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef FC_HXX
#define FC_HXX

#include <iostream.h>
#include "defs.hxx"

class FC {
public:
	FC();
	void SetFieldStart(const GPTYPE NewFieldStart);
	GPTYPE GetFieldStart();
	void SetFieldEnd(const GPTYPE NewFieldEnd);
	GPTYPE GetFieldEnd();
	void Write(PFILE fp) const;
	void Read(PFILE fp);
	void FlipBytes();
	friend ostream& operator<<(ostream& os, const FC& Fc);
	~FC();
private:
	GPTYPE FieldStart;
	GPTYPE FieldEnd;
};

typedef FC* PFC;

#endif
