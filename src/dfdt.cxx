/*@@@
File:		dfdt.cxx
Version:	1.00
Description:	Class DFDT - Data Field Definitions Table
Author:		Nassib Nassar, nrn@cnidr.org
@@@*/
#include <stdlib.h> //for exit()
#include <string.h>

#include "dfdt.hxx"
#include "common.hxx"

DFDT::DFDT() {
  Initialize();
}

void DFDT::Initialize() {
  Table = new DFD[5];
  TotalEntries = 0;
  MaxEntries = 5;
  Changed = 0;
}

DFDT& DFDT::operator=(const DFDT& OtherDfdt) {
  if (Table) {
    delete [] Table;
  }
  Initialize();
  INT y = OtherDfdt.GetTotalEntries();
  INT x;
  DFD dfd;
  for (x=1; x<=y; x++) {
    OtherDfdt.GetEntry(x, &dfd);
    AddEntry(dfd);
  }
  return *this;
}

void DFDT::LoadTable(const STRING& FileName) {
  PCHR b;
  INT4 RecStart,RecEnd,ActualLength,len;

  if (Table) {
    delete [] Table;
  }
  Initialize();
  PFILE fp = fopen(FileName, "r");

  // Let's bring the entire file into memory
  if (!fp) {
    return;
  }
  fseek(fp, 0, 2);
  RecStart = 0;
  RecEnd = ftell(fp);

  fseek(fp, RecStart, 0);
  len = RecEnd - RecStart;
  b = new CHR[len + 1];
  ActualLength = fread(b, 1, len, fp);
  b[ActualLength] = '\0';
  fclose(fp);

  INT x,y;
  STRING s;
  INT DfdCount, AttrCount;
  DFD dfd;
  ATTR attr;
  PATTRLIST AttrList;
  PCHR pBuf;

  // Get the # of fields from the start of the buffer
  pBuf = strtok(b,"\n");
  DfdCount = atoi(pBuf);

  // Run through the buffer, looking for newlines
  // Get the file number, the # of attribs, the attribute IDs,
  // attribute types and values
  for (x=0; x<DfdCount; x++) {
    AttrList = new ATTRLIST();
    pBuf = strtok(NULL,"\n");         // Get the file #
    dfd.SetFileNumber(atoi(pBuf));    // Save it
    pBuf = strtok(NULL,"\n");         // Get the # of attributes
    AttrCount = atoi(pBuf);
    for (y=0;y<AttrCount;y++) {
      pBuf = strtok(NULL,"\n");
      s = pBuf;
      attr.SetSetId(s);
      pBuf = strtok(NULL,"\n");
      attr.SetAttrType(atoi(pBuf));
      pBuf = strtok(NULL,"\n");
      s = pBuf;
      attr.SetAttrValue(s);
      AttrList->AddEntry(attr);
    }
    dfd.SetAttributes(*AttrList);
    delete AttrList;
    AddEntry(dfd);
  }
  delete b;
  Changed = 0;
}

void DFDT::SaveTable(const STRING& FileName) {
	if (TotalEntries == 0) {
		StrUnlink(FileName);
		return;
	}
	PFILE fp = fopen(FileName, "w");
	if (!fp) {
		perror(FileName);
		exit(1);
	}
	else {
		INT x, y, z;
		STRING s;
		PATTRLIST AttrList;
		ATTR Attr;
		fprintf(fp, "%d\n", TotalEntries);
		for (x=0; x<TotalEntries; x++) {
			AttrList = new ATTRLIST();
//			Table[x].GetFieldName(&s);
//			s.Print(fp);
			fprintf(fp, "%d\n", Table[x].GetFileNumber());
			Table[x].GetAttributes(AttrList);
			z = AttrList->GetTotalEntries();
			fprintf(fp, "%d\n", z);
			for (y=1; y<=z; y++) {
				AttrList->GetEntry(y, &Attr);
				Attr.GetSetId(&s);
				s.Print(fp);
				fprintf(fp, "\n%d\n", Attr.GetAttrType());
				Attr.GetAttrValue(&s);
				s.Print(fp);
				fprintf(fp, "\n");
			}
			delete AttrList;
		}
		fclose(fp);
	}
	Changed = 0;
}

void DFDT::AddEntry(const DFD& DfdRecord) {
	Changed = 1;
	DFD Dfd;
	Dfd = DfdRecord;
	INT x;
	STRING f1, f2;
	// linear!
	for (x=0; x<TotalEntries; x++) {
		Table[x].GetFieldName(&f1);
		Dfd.GetFieldName(&f2);
		if (f1.Equals(f2)) {
			Dfd.SetFileNumber(Table[x].GetFileNumber());
			Table[x] = Dfd;
			return;
		}
	}

	if (TotalEntries == MaxEntries)
		Expand();
	Dfd.SetFileNumber(GetNewFileNumber());
	Table[TotalEntries] = Dfd;
	TotalEntries = TotalEntries + 1;
}

void DFDT::GetEntry(const INT Index, PDFD DfdRecord) const {
	if ( (Index > 0) && (Index <= TotalEntries) ) {
		*DfdRecord = Table[Index-1];
	}
}

void DFDT::GetDfdRecord(const STRING& FieldName, PDFD DfdRecord) const {
	INT x = 0;
	STRING s;
	STRING Field;
	Field = FieldName;
	Field.UpperCase();
	while (x < TotalEntries) {
		Table[x].GetFieldName(&s);
		if (s.Equals(Field)) {
			*DfdRecord = Table[x];
		}
		x++;
	}
	// ER
}

INT DFDT::GetNewFileNumber() const {
	INT x = 1;
	INT y;
	INT Found;
	do {
		y = 0;
		Found = 0;
		while ( (y < TotalEntries) && (!Found) ) {
			if (Table[y].GetFileNumber() == x) {
				Found = 1;
			}
			y++;
		}
		if (!Found) {
			return x;
		}
		x++;
	} while (x < 1000);
	// ER - can't take any more fields
	return 0;
}

void DFDT::Expand() {
	Resize(TotalEntries+10);
}

void DFDT::CleanUp() {
	Resize(TotalEntries);
}

void DFDT::Resize(const INT Entries) {
	PDFD Temp = new DFD[Entries];
	INT RecsToCopy;
	INT x;
	if (Entries >= TotalEntries) {
		RecsToCopy = TotalEntries;
	} else {
		RecsToCopy = Entries;
		TotalEntries = Entries;
	}
	for (x=0; x<RecsToCopy; x++) {
		Temp[x] = Table[x];
	}
	if (Table)
		delete [] Table;
	Table = Temp;
	MaxEntries = Entries;
}

INT DFDT::GetTotalEntries() const {
	return TotalEntries;
}

INT DFDT::GetChanged() const {
	return Changed;
}

DFDT::~DFDT() {
	if (Table)
		delete [] Table;
}
