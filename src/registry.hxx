/************************************************************************
Copyright Notice

Copyright (c) MCNC, Clearinghouse for Networked Information Discovery and
Retrieval, 1994. 

Permission to use, copy, modify, distribute, and sell this software and
its documentation, in whole or in part, for any purpose is hereby granted
without fee, provided that

1. The above copyright notice and this permission notice appear in all
copies of the software and related documentation. Notices of copyright
and/or attribution which appear at the beginning of any file included in
this distribution must remain intact. 

2. Users of this software agree to make their best efforts (a) to return
to MCNC any improvements or extensions that they make, so that these may
be included in future releases; and (b) to inform MCNC/CNIDR of noteworthy
uses of this software. 

3. The names of MCNC and Clearinghouse for Networked Information Discovery
and Retrieval may not be used in any advertising or publicity relating to
the software without the specific, prior written permission of MCNC/CNIDR. 

THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. 

IN NO EVENT SHALL MCNC/CNIDR BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE
POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
************************************************************************/

/*@@@
File:		registry.hxx
Version:	1.00
Description:	Class REGISTRY - Structured Profile Registry
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/

#ifndef REGISTRY_HXX
#define REGISTRY_HXX

#include "gdt.h"
#include "string.hxx"
#include "strlist.hxx"

class REGISTRY {
public:
	REGISTRY(const STRING& Title);
	REGISTRY(const PCHR Title);
	REGISTRY& operator=(const REGISTRY& OtherRegistry);
	void SetData(const STRLIST& Position, const STRLIST& Value);
	void AddData(const STRLIST& Position, const STRLIST& Value);
	void GetData(const STRLIST& Position, PSTRLIST StrlistBuffer);
	void SaveToFile(const STRING& FileName, const STRLIST& Position);
	void LoadFromFile(const STRING& FileName, const STRLIST& Position);
	void AddFromFile(const STRING& FileName, const STRLIST& Position);
	// Note: ProfileGetString() and ProfileWriteString() consider
	// a comma-delimited string in Entry to be a list of values, and
	// will store each as a separate node in the registry.  That is
	// the deepest level of nesting that these two functions allow.
	// For access to full nesting, I recommend using SetData()/AddData()
	// and GetData().
	void ProfileGetString(const STRING& Section, const STRING& Entry,
			const STRING& Default, PSTRING StringBuffer);
	void ProfileWriteString(const STRING& Section, const STRING& Entry,
			const STRING& StringData);
	// Note: ProfileLoadFromFile() and ProfileAddFromFile() also parse
	// a comma-delimited list (e.g., tag=val1,val2,val3) into multiple nodes.
	void ProfileLoadFromFile(const STRING& FileName, const STRLIST& Position);
	void ProfileAddFromFile(const STRING& FileName, const STRLIST& Position);
	void ProfileWrite(ostream& os, const STRING& FileName, const STRLIST& Position);
	friend ostream & operator<<(ostream& os, const REGISTRY& Registry);
	~REGISTRY();
private:
	void ProfileAddFromFile(const STRING& FileName);
	void LoadFromFile(const STRING& FileName);
	void AddFromFile(const STRING& FileName);
	void Print(ostream& os, const INT Level) const;
	void ProfilePrint(ostream& os, const INT Level) const;
	void GetData(PSTRLIST StrlistBuffer);
	void DeleteChildren();
	REGISTRY* FindNode(const STRING& Position);
	REGISTRY* GetNode(const STRING& Position);
	REGISTRY* FindNode(const STRLIST& Position);
	REGISTRY* GetNode(const STRLIST& Position);
	void SetData(const STRLIST& Value);
	void AddData(const STRLIST& Value);
	STRING Data;
	REGISTRY* Next;
	REGISTRY* Child;
};

typedef REGISTRY* PREGISTRY;

#endif
