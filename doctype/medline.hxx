/*-@@@
File:		medline.hxx
Version:	1.00
Description:	Class MEDLINE - MEDLINE-like Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef MEDLINE_HXX
#define MEDLINE_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif

// Generic Medline format
class MEDLINE :  public DOCTYPE {
public:
	MEDLINE(PIDBOBJ DbParent);
	void AddFieldDefs();
	void ParseRecords(const RECORD& FileRecord);
	void ParseFields(PRECORD NewRecord);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~MEDLINE();
// hooks into the guts of the field parser
	virtual PCHR UnifiedName (PCHR tag) const; // for children to play with
};
typedef MEDLINE* PMEDLINE;

#endif
