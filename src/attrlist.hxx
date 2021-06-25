/*@@@
File:		attrlist.hxx
Version:	1.00
Description:	Class ATTRLIST - Attribute List
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef ATTRLIST_HXX
#define ATTRLIST_HXX

#include "defs.hxx"
#include "attr.hxx"

class ATTRLIST {
public:
	ATTRLIST();
	void Init();
	ATTRLIST& operator=(const ATTRLIST& OtherAttrlist);
	void AddEntry(const ATTR& AttrRecord);
	void GetEntry(const INT Index, PATTR AttrRecord) const;
	void SetEntry(const INT Index, const ATTR& AttrRecord);
	void DeleteEntry(const INT Index);
	void Expand();
	void CleanUp();
	void Resize(const INT Entries);
	INT GetTotalEntries() const;
	INT Lookup(const STRING& SetId, const INT AttrType) const;
	INT Lookup(const STRING& SetId, const INT AttrType, const INT AttrValue) const;
	void SetValue(const STRING& SetId, const INT AttrType, const STRING& AttrValue);
	void SetValue(const STRING& SetId, const INT AttrType, const INT AttrValue);
	void ClearAttr(const STRING& SetId, const INT AttrType, const INT AttrValue);
	GDT_BOOLEAN GetValue(const STRING& SetId, const INT AttrType, PSTRING StringBuffer) const;
	GDT_BOOLEAN GetValue(const STRING& SetId, const INT AttrType, PINT IntBuffer) const;
	void AttrSetFieldName(const STRING& FieldName);
	GDT_BOOLEAN AttrGetFieldName(PSTRING StringBuffer) const;
	void AttrSetRightTruncation(const GDT_BOOLEAN RightTruncation);
	GDT_BOOLEAN AttrGetRightTruncation() const;
	void AttrSetTermWeight(const STRING& TermWeight);
	GDT_BOOLEAN AttrGetTermWeight(PSTRING StringBuffer) const;
	~ATTRLIST();
private:
	PATTR Table;
	INT TotalEntries;
	INT MaxEntries;
};

typedef ATTRLIST* PATTRLIST;

#endif
