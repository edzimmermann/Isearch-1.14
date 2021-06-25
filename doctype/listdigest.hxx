/*-@@@
File:		listdigest.hxx
Version:	1.00
Description:	Class LISTDIGEST - Listserver Mail Digest Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef LISTDIGEST_HXX
#define LISTDIGEST_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif
#include "mailfolder.hxx"

class LISTDIGEST :  public MAILFOLDER {
public:
	LISTDIGEST(PIDBOBJ DbParent);
	void ParseRecords(const RECORD& FileRecord);
	~LISTDIGEST();
};

typedef LISTDIGEST* PLISTDIGEST;

#endif
