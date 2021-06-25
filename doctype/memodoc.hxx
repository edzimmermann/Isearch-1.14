/*-@@@
File:		memodoc.hxx
Version:	1.00
Description:	Class MEMODOC - Colon Tagged Memo Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef MEMODOC_HXX
#define MEMODOC_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif

class MEMODOC :  public DOCTYPE {
public:
	MEMODOC(PIDBOBJ DbParent);
	void AddFieldDefs();
	void ParseRecords(const RECORD& FileRecord);
	void ParseFields(PRECORD NewRecord);
	void Present(const RESULT& ResultRecord, const STRING& ElementSet,
		PSTRING StringBuffer);
	~MEMODOC();
};
typedef MEMODOC* PMEMODOC;

#endif
