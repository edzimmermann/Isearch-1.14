/*-@@@
File:		irlist.hxx
Version:	1.00
Description:	Class IRLIST - IRList Mail Digest Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef IRLIST_HXX
#define IRLIST_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif
#include "mailfolder.hxx"

class IRLIST :  public MAILFOLDER {
public:
	IRLIST(PIDBOBJ DbParent);
	void ParseRecords(const RECORD& FileRecord);
	~IRLIST();
};

typedef IRLIST* PIRLIST;

#endif
