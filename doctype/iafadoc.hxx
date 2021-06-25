/*-@@@
File:		iafadoc.hxx
Version:	1.00
Description:	Class IAFADOC - Colon Tagged (IAFA-like) Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef IAFADOC_HXX
#define IAFADOC_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif
#include "colondoc.hxx"

class IAFADOC :  public COLONDOC {
public:
	IAFADOC(PIDBOBJ DbParent);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~IAFADOC();
};
typedef IAFADOC* PIAFADOC;

#endif
