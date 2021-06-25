/*-@@@
File:		firstline.hxx
Version:	1.00
Description:	Class FIRSTLINE - Text Document Type, first line is headline
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/
#ifndef FIRSTLINE_HXX

#ifndef DOCTYPE_HXX
#include "defs.hxx"
#include "doctype.hxx"
#endif

class FIRSTLINE : public DOCTYPE {
public:
	FIRSTLINE(PIDBOBJ DbParent);
	virtual void ParseFields(PRECORD NewRecord);
	virtual ~FIRSTLINE();
};

typedef FIRSTLINE* PFIRSTLINE;

#endif
