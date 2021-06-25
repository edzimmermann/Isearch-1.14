/*-@@@
File:		maildigest.hxx
Version:	1.00
Description:	Class MAILDIGEST - Mail Digest Document Type
Author:		Edward C. Zimmermann, edz@bsn.com
Copyright:	Basis Systeme netzwerk, Munich
@@@-*/

#ifndef MAILDIGEST_HXX
#define MAILDIGEST_HXX

#ifndef DTREG_HXX
# include "defs.hxx"
# include "doctype.hxx"
#endif
#include "mailfolder.hxx"

class MAILDIGEST :  public MAILFOLDER {
public:
	MAILDIGEST(PIDBOBJ DbParent);
	void ParseRecords(const RECORD& FileRecord);
	~MAILDIGEST();
};

typedef MAILDIGEST* PMAILDIGEST;

#endif
