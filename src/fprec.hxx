/*@@@
File:		fprec.hxx
Version:	1.00
Description:	Class FPREC - File Pointer Record
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef FPREC_HXX
#define FPREC_HXX

#include "defs.hxx"
#include "string.hxx"

class FPREC {
public:
	FPREC();
	FPREC& operator=(const FPREC& OtherFprec);
	void SetFileName(const STRING& NewFileName);
	void GetFileName(PSTRING StringBuffer) const;
	void SetFilePointer(const PFILE NewFilePointer);
	PFILE GetFilePointer() const;
	void SetPriority(const INT NewPriority);
	INT GetPriority() const;
	void SetClosed(const GDT_BOOLEAN NewClosed);
	GDT_BOOLEAN GetClosed() const;
	void SetOpenMode(const STRING& NewOpenMode);
	void GetOpenMode(PSTRING StringBuffer) const;
	~FPREC();
private:
	STRING FileName;
	PFILE FilePointer;
	STRING OpenMode;
	INT Priority;
	GDT_BOOLEAN Closed;
};

typedef FPREC* PFPREC;

#endif
