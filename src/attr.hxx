/*@@@
File:		attr.hxx
Version:	1.00
Description:	Class ATTR - Attribute
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef ATTR_HXX
#define ATTR_HXX

#include "defs.hxx"
#include "string.hxx"

class ATTR {
public:
	ATTR();
	ATTR& operator=(const ATTR& OtherAttr);
	void SetSetId(const STRING& NewSetId);
	void GetSetId(PSTRING StringBuffer) const;
	void SetAttrType(const INT NewAttrType);
	INT GetAttrType() const;
	void SetAttrValue(const STRING& NewAttrValue);
	void GetAttrValue(PSTRING StringBuffer) const;
	void SetAttrValue(const INT NewAttrValue);
	INT GetAttrValue() const;
	~ATTR();
private:
	STRING SetId;
	INT AttrType;
	STRING AttrValue;
};

typedef ATTR* PATTR;

#endif
