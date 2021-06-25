/*-@@@
File:		filmline.hxx
Version:	1.00
Description:	Class FILMLINE - FILMLINE v1.x Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef FILMLINE_HXX
#define FILMLINE_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif
#include "medline.hxx"

// Filmline v 1.x Interchange format
class FILMLINE :  public MEDLINE {
public:
	FILMLINE(PIDBOBJ DbParent);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~FILMLINE();
// hooks into the guts of the Medline field parser
	PCHR UnifiedName (PCHR tag) const; 
};
typedef FILMLINE* PFILMLINE;


#endif
