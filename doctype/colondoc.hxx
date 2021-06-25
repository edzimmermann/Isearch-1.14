/*-@@@
File:		colondoc.hxx
Version:	1.00
Description:	Class COLONDOC - Colon Tagged (IAFA-like) Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef COLONDOC_HXX
#define COLONDOC_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif

class COLONDOC :  public DOCTYPE {
public:
	COLONDOC(PIDBOBJ DbParent);
	void AddFieldDefs();
	void ParseRecords(const RECORD& FileRecord);
	void ParseFields(PRECORD NewRecord);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~COLONDOC();
// hooks into the guts of the field parser
	virtual PCHR UnifiedName (PCHR tag) const; // for children to play with
};
typedef COLONDOC* PCOLONDOC;

#endif
