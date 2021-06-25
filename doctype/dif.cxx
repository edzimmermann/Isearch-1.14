/*@@@
File:		dif.cxx
Version:	1.02
Description:	Class DIF - SGML-like Text w/ static output files
Author:		Archie Warnock, warnock@clark.net
                Bug fixes from Ed Zimmerman
@@@*/

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "glist.hxx"
#include "gstack.hxx"
#include "strstack.hxx"
#include "dif.hxx"

#define MaxDIFSize 10240

#define SUTRS_OID  "1.2.840.10003.5.101"
#define USMARC_OID "1.2.840.10003.5.10"
#define HTML_OID   "1.2.840.10003.5.1000.34.1"
#define SGML_OID   "1.2.840.10003.5.1000.34.2"

#define HTML_DICT "display_html.dic"

#define DICT_ENV 0
#define DICT_COMPILED 1

void transform(char *diffile, char *dictfile, int env_flag, char *output);
static PCHR *parse_tags (PCHR b, GPTYPE len);
static PCHR *parse_Groups (PCHR b, GPTYPE len);


// Local prototypes
DIF::DIF(PIDBOBJ DbParent) : COLONDOC(DbParent) {
}

void DIF::ParseFields (PRECORD NewRecord)
{
  STRING fn;
  NewRecord->GetFullFileName (&fn);
  PFILE fp = fopen (fn, "rb");
  if (!fp) {
    return;		// ERROR
  }

  INT4 RecStart = NewRecord->GetRecordStart ();
  INT4 RecEnd = NewRecord->GetRecordEnd ();
  if ((RecEnd == 0) || (RecEnd == -1)) {
    fseek (fp, 0, 2);
    RecStart = 0;
    RecEnd = ftell (fp);
  }
  fseek (fp, RecStart, 0);
  GPTYPE RecLength = RecEnd - RecStart;
  PCHR RecBuffer = new CHR[RecLength + 1];
  GPTYPE ActualLength = fread (RecBuffer, 1, RecLength, fp);
  fclose (fp);
  RecBuffer[ActualLength] = '\0';

  PCHR *groups = parse_Groups (RecBuffer, ActualLength);
  PCHR *tags = parse_tags (RecBuffer, ActualLength);

  if (tags == NULL || tags[0] == NULL) {
    STRING doctype;
    NewRecord->GetDocumentType(&doctype);
    if (tags) {
      delete tags;
      cout << "Warning: No `" << doctype << "' fields/tags in \"" 
	<< fn << "\"\n";
    } else {
      cout << "Unable to parse `" << doctype << "' record in \"" 
	<< fn << "\"\n";
    }
    delete [] RecBuffer;
    return;
  }

  FC fc;
  DF df;
  PDFT pdft;
  pdft = new DFT ();
  DFD dfd;
  STRING FieldName;
  // Walk though tags, skipping the groups
  for (PCHR * tags_ptr = tags; *tags_ptr; tags_ptr++) {
    
    PCHR p = tags_ptr[1]; 			// end of field
    if (p == NULL) 				// If no end of field
      p = &RecBuffer[RecLength]; 		// use end of buffer
    						// eg "Author:"

    PCHR unified_name = UnifiedName(*tags_ptr);
#if WANT_MISC
    // Throw "unclassified" into Misc
    FieldName = unified_name ? unified_name: "Misc";
#else
    // Ignore "unclassified" fields
    if (unified_name == NULL) 
      continue; // ignore these
    FieldName = unified_name;
#endif

    INT val_start, val_len;

    if ( !(FieldName.CaseEquals("Group")) ) {
      size_t off = strlen (*tags_ptr) + 1;
      val_start = (*tags_ptr + off) - RecBuffer;

      // Skip while space after the ':'
      while (isspace (RecBuffer[val_start]))
	val_start++, off++;

      // Also leave off the \n
      val_len = (p - *tags_ptr) - off - 1;

      // Strip potential trailing while space
      while (val_len > 0 && isspace (RecBuffer[val_len + val_start]))
	val_len--;
      if (val_len <= 0) 
	continue; // Don't bother with empty fields

      if (FieldName.CaseEquals("Entry_ID")) {
	PCHR entry_id = new CHR [val_len+2];
	strncpy(entry_id,(*tags_ptr+off),val_len+1);
	entry_id[val_len+1] = '\0';
	cout << "DIF " << entry_id << endl;
	delete [] entry_id;
      }

      dfd.SetFieldName (FieldName);
#ifdef AW_DEBUG
      cout << "Wrote field ->" << FieldName << ", between " 
	<< val_start << ", and " << val_start+val_len << endl;
#endif /* AW_DEBUG */
      Db->DfdtAddEntry (dfd);
      fc.SetFieldStart (val_start);
      fc.SetFieldEnd (val_start + val_len);
      PFCT pfct = new FCT ();
      pfct->AddEntry (fc);
      df.SetFct (*pfct);
      df.SetFieldName (FieldName);
      pdft->AddEntry (df);
      delete pfct;
    }

  }

  // Now, deal with the groups
  GSTACK pGroups;
  STRSTACK pFieldNames;
#ifdef AW_DEBUG
  cout << "starting at " << groups[0] - RecBuffer << endl;
#endif /* AW_DEBUG */
  for (PCHR * tags_ptr1 = groups; *tags_ptr1; tags_ptr1++) {
    PCHR p = tags_ptr1[1]; 			// end of field
    if (p == NULL) 				// If no end of field
      p = &RecBuffer[RecLength]; 		// use end of buffer
    
    INT val_start, val_len;

    FieldName = *tags_ptr1;
    if (FieldName.CaseEquals("Group")) {

      size_t off = strlen (*tags_ptr1) + 1;
      val_start = (*tags_ptr1 + off) - RecBuffer;

      // Skip while space after the ':'
      while (isspace (RecBuffer[val_start])) {
	val_start++; 
	off++;
      }

      // Now, RecBuffer[val_start] should point to the first word after
      // "GROUP", so it's the new fieldname

      val_len = 0;
      FieldName = "";
      while ( !strchr(" \f\r\v\n\t\0",RecBuffer[val_start+val_len]) ) {
	FieldName.SetChr(val_len+1,RecBuffer[val_start+val_len-1]);
#ifdef AW_DEBUG
	cout << "->" << FieldName << "<-, val_len=" << val_len << endl;
#endif /* AW_DEBUG */
	val_len++;
      }

      PINT Val = new INT;
      *Val = val_start;

      pGroups.Push(Val);
#ifdef AW_DEBUG
      cout << "Pushing group " << FieldName << " starting at " << val_start;
      cout << ", Stack depth is " << pGroups.GetSize() << endl;
#endif /* AW_DEBUG */
      pFieldNames.Push(FieldName);

    }
    //    else if (ThisTag.CaseEquals("End_Group")) {
    else {
      PCHR p;
      PINT pStart;
      INT val_end;

      val_end = *tags_ptr1 - RecBuffer;

      pFieldNames.Pop(&FieldName);
      
      pStart = (PINT)pGroups.Top();
      pGroups.Pop();
      val_start = *pStart;
#ifdef AW_DEBUG
      cout << "Popping group " << FieldName << " starting at " << val_start;
      cout << ", Stack depth is " << pGroups.GetSize() << endl;
#endif /* AW_DEBUG */
      delete pStart;

      dfd.SetFieldName (FieldName);
#ifdef AW_DEBUG
      cout << "Wrote group ->" << FieldName << ", between " 
	<< val_start << ", and " << val_end << endl;
#endif /* AW_DEBUG */
      Db->DfdtAddEntry (dfd);
      fc.SetFieldStart (val_start);
      fc.SetFieldEnd (val_end);
      PFCT pfct = new FCT ();
      pfct->AddEntry (fc);
      df.SetFct (*pfct);
      df.SetFieldName (FieldName);
      pdft->AddEntry (df);
      delete pfct;

    }
  }

  NewRecord->SetDft (*pdft);
  delete pdft;
  delete[]RecBuffer;
  delete tags;
}

void DIF::Present(const RESULT& ResultRecord, const STRING& ElementSet, 
		     const STRING& RecordSyntax, PSTRING StringBufferPtr)
{
  *StringBufferPtr = "";

  if (ElementSet.Equals("B")) {
    STRLIST Strlist;
    STRING TitleTag,Title;
    PCHR headline;
    GDT_BOOLEAN Status;

    TitleTag = "Entry_Title"; // Brief headline is "title"
    Status = Db->GetFieldData(ResultRecord, TitleTag, &Strlist);
    if (Status) {
      Strlist.Join("\n",&Title);
      Title.Replace("\n"," ");
      Title.Replace("\r"," ");
    } else
      Title = "No Title";
    if (RecordSyntax.Equals(HTML_OID)) {
      STRING Temp;
      Temp = "<LI>";
      Temp.Cat(Title);
      *StringBufferPtr = Temp;
    } else
      *StringBufferPtr = Title;
  } else {
    PCHR pDictFile,pRawData,pFormattedData;
    STRING DataBuffer;

    ResultRecord.GetRecordData(&DataBuffer);

    if (RecordSyntax.Equals(HTML_OID)) {
      pRawData = DataBuffer.NewCString();
      pFormattedData = new CHR[MaxDIFSize+1];
      if (pFormattedData != NULL ) {
	transform(pRawData,HTML_DICT,DICT_COMPILED,pFormattedData);
	DataBuffer = "<PRE>";
	DataBuffer.Cat(pFormattedData);
	DataBuffer.Cat("</PRE>");
	*StringBufferPtr = DataBuffer;
	delete [] pFormattedData;
      } else {
	DataBuffer = "<PRE>";
	DataBuffer.Cat(pRawData);
	DataBuffer.Cat("</PRE>");
	*StringBufferPtr = DataBuffer;
      }
      delete pRawData;
    } else
      *StringBufferPtr=DataBuffer;
  }
  return;
}

DIF::~DIF() {
}

void transform(char *diffile, char *dictfile, int env_flag, char *output)
{
  INT DIFlen;
  PCHR tmp;
  DIFlen = strlen(diffile);
  if (DIFlen <= MaxDIFSize) {
    strcpy(output,diffile);
  } else
    strcpy(output,"Can't fit file in buffer\n");
  return;
}

/*-
   What:        Given a buffer of ColonTag (eg. IAFA) data:
   returns a list of char* to all characters pointing to the TAG

   Colon Records:
TAG1: ...
.....
TAG2: ...
TAG3: ...
...
....

1) Fields are continued when the line has no tag
2) Field names may NOT contain white space
3) The space BEFORE field names MAY contain white space
4) Between the field name and the ':' NO white space is
   allowed.

-*/
#define TAG_GROW_SIZE 144

static PCHR *parse_tags (PCHR b, GPTYPE len)
{
  PCHR *t;			// array of pointers to first char of tags
  size_t tc = 0;		// tag count

  size_t max_num_tags;	// max num tags preallocated
  enum { HUNTING, STARTED, CONTINUING } State = HUNTING;

  /* You should allocate these as you need them, but for now... */
  max_num_tags = TAG_GROW_SIZE;
  t = new PCHR [max_num_tags];
  for (GPTYPE i = 0; i < len; i++) {
    if (b[i] == '\r' || b[i] == '\v')
      continue; // Skip over
    if (State == HUNTING && !isspace(b[i])) {
      t[tc] = &b[i];
      State = STARTED;
    }
    else if ((State == STARTED) && (b[i] == ' ' || b[i] == '\t')) {
      State = CONTINUING;
    }
    else if ((State == STARTED) && (b[i] == ':')) {
      b[i] = '\0';
      // Expand memory if needed
      if (++tc == max_num_tags - 1) {
	// allocate more space
	max_num_tags += TAG_GROW_SIZE;
	PCHR *New = new PCHR [max_num_tags];
	if (New == NULL) {
	  delete [] t;
	  return NULL; // NO MORE CORE!
	}
	memcpy(New, t, tc*sizeof(PCHR));
	delete [] t;
	t = New;
      }
      State = CONTINUING;
    }
    else if ((State == CONTINUING || State == STARTED) && (b[i] == '\n')) {
      State = HUNTING;
    }
/*
    else if ((State == CONTINUING || State == STARTED) && (b[i] == ' ')) {
      State = HUNTING;
    }
    else if ((State == CONTINUING || State == STARTED) && (b[i] == '\t')) {
      State = HUNTING;
    }
*/
/* Define XXXX above to NOT allow white space
   before field names */
#if XXXX
    else if (State == HUNTING)
      State = CONTINUING;
#endif
  }
  t[tc] = (PCHR) NULL;
  return t;
}

static PCHR *parse_Groups (PCHR b, GPTYPE len)
{
  PCHR *t, *starts, *ends;	// array of pointers to first char of tags
  PCHR where,found;
  size_t tc = 0;		// tag count
  size_t tc_start = 0;		// tag count
  size_t tc_end = 0;		// tag count
  size_t max_num_tags;		// max num tags for which space is allocated

  // You should allocate these as you need them, but for now...
  max_num_tags = TAG_GROW_SIZE;
  t = new PCHR [max_num_tags];
  starts = new PCHR [max_num_tags];
  ends = new PCHR [max_num_tags];

  where = b;
  while ( (found=strstr(where,"Group:")) ) {
	 
    starts[tc_start] = found;
#ifdef AW_DEBUG
    cout << "Group offset: " << found - b << endl;
#endif /* AW_DEBUG */
    where = found+strlen("Group:")-1;

    // Expand memory if needed
    if (++tc_start == max_num_tags - 1) {
      // allocate more space
      max_num_tags += TAG_GROW_SIZE;
      PCHR *New = new PCHR [max_num_tags];
      if (New == NULL) {
	delete [] starts;
	return NULL; // NO MORE CORE!
      }
      memcpy(New, starts, tc_start*sizeof(PCHR));
      delete [] starts;
      starts = New;
    }
  }

  where = b;
  while ( (found=strstr(where,"End_Group")) ) {
    ends[tc_end] = found;
#ifdef AW_DEBUG
    cout << "End_Group offset: " << found - b << endl;    
#endif /* AW_DEBUG */
    where = found+strlen("End_Group");

    // Expand memory if needed
    if (++tc_end == max_num_tags - 1) {
      // allocate more space
      max_num_tags += TAG_GROW_SIZE;
      PCHR *New = new PCHR [max_num_tags];
      if (New == NULL) {
	delete [] ends;
	return NULL; // NO MORE CORE!
      }
      memcpy(New, ends, tc_end*sizeof(PCHR));
      delete [] ends;
      ends = New;
    }
  }

  // Bail out if we don't have the same number of starting and ending tags
  if (tc_start != tc_end) {
    cout << "Warning: unable to parse Groups.  Mismatched starting and ending tags." << endl;
    t[0]=(PCHR)NULL;
    return t;
  }	 
    

  // Now, sort the list on the addresses
  INT n_Group=0, n_End=0;
  INT start, end;

  do {
    start = starts[n_Group] - b;
    end = ends[n_End] - b;
#ifdef AW_DEBUG
    cout << "Comparing start: " << start << " end: " << end << endl;
#endif /* AW_DEBUG */
    if (start < end) {
      t[tc] = starts[n_Group];
#ifdef AW_DEBUG
      cout << "Saving starting offset: " << start << endl;
#endif /* AW_DEBUG */
      n_Group++;
    } else {
      t[tc] = ends[n_End];
#ifdef AW_DEBUG
      cout << "Saving ending offset: " << end << endl;
#endif /* AW_DEBUG */
      n_End++;
    }

    // Expand memory if needed
    if (++tc == max_num_tags - 1) {
      // allocate more space
      max_num_tags += TAG_GROW_SIZE;
      PCHR *New = new PCHR [max_num_tags];
      if (New == NULL) {
	delete [] t;
	return NULL; // NO MORE CORE!
      }
      memcpy(New, t, tc*sizeof(PCHR));
      delete [] t;
      t = New;
    }

  } while ( (n_Group < tc_start) && (n_End < tc_end) );

  // All the Group tags should be done, but there might still be some
  // remaining End_Group tags, so we need to put them in, too

  while (n_End < tc_end) {
    end = ends[n_End] - b;
#ifdef AW_DEBUG
    cout << "Leftover End_Group offset: " << end << endl;    
#endif /* AW_DEBUG */
    t[tc] = ends[n_End];
    n_End++;

    // Expand memory if needed
    if (++tc == max_num_tags - 1) {
      // allocate more space
      max_num_tags += TAG_GROW_SIZE;
      PCHR *New = new PCHR [max_num_tags];
      if (New == NULL) {
	delete [] t;
	return NULL; // NO MORE CORE!
      }
      memcpy(New, t, tc*sizeof(PCHR));
      delete [] t;
      t = New;
    }
  }

  t[tc]=(PCHR)NULL;
  return t;
}
