/*-@@@
File:		referbib.hxx
Version:	1.00
Description:	Class REFERBIB - Refer Bibliographic records Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef REFERBIB_HXX
#define REFERBIB_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif

class REFERBIB :  public DOCTYPE {
public:
	REFERBIB(PIDBOBJ DbParent);
	void AddFieldDefs();
	void ParseRecords(const RECORD& FileRecord);
	void ParseFields(PRECORD NewRecord);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~REFERBIB();
// hooks into the guts of the field parser
	virtual PCHR UnifiedName (PCHR tag) const; // for children to play with
};
typedef REFERBIB* PREFERBIB;

#endif
