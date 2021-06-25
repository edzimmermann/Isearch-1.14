/*-@@@
File:		sgmlnorm.hxx
Version:	1.00
Description:	Class SGMLGILS
Author:		Kevin Gamiel
Copyright:	CNIDR
@@@-*/

#ifndef SGMLGILS_HXX
#define SGMLGILS_HXX

#include "sgmlnorm.hxx"

class SGMLGILS:public SGMLNORM {
  public:
  SGMLGILS (PIDBOBJ DbParent);
  void Present (const RESULT & ResultRecord, const STRING & ElementSet,
		const STRING & RecordSyntax, PSTRING StringBuffer);
  void GetSUTRSRecord (const RESULT & ResultRecord, const STRING & ElementSet,
		PSTRING StringBuffer);

  ~SGMLGILS ();
};

typedef SGMLGILS *PSGMLGILS;

#endif
