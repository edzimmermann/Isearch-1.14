
/*** Local Configurations for BSn doctypes ****/


// HTML
#define STRICT_HTML     	1 /* 0 ==> Accept Anything, 1==> accept only "known" tags */

// Mail
#define RESTRICT_MAIL_FIELDS	1 /* 0 ==> Accept anything, 1==> only those in list */
#define SHOW_MAIL_DATE   	0 /* 0 ==> Headline "From: Subject" 1==> "From dd/mm/yy: Subject" */ 

// Bibliographic Formats

// Use Native or Unified names by default?
#define USE_UNIFIED_NAMES 1

// Define Below to individualy modify above behaviour
//#define REFER_UNIFIED_NAMES 1 
//#define MEDLINE_UNIFIED_NAMES 1
//#define FILMLINE_UNIFIED_NAMES 1

// General 
#define USE_UNIFIED_NAMES 1

// --------- End User Configurable Options

// Specific
#ifndef REFER_UNIFIED_NAMES
#define REFER_UNIFIED_NAMES USE_UNIFIED_NAMES
#endif
#ifndef MEDLINE_UNIFIED_NAMES
#define MEDLINE_UNIFIED_NAMES USE_UNIFIED_NAMES
#endif
#ifndef FILMLINE_UNIFIED_NAMES
#define FILMLINE_UNIFIED_NAMES USE_UNIFIED_NAMES
#endif

// General
#ifndef BSN_EXTENSIONS
# define BSN_EXTENSIONS	0 /* 0 ==> CNIDR's Isearch, 1==> BSn's version */
#endif
#if BSN_EXTENSIONS < 1
# define BRIEF_MAGIC "B" /* CNIDR "hardwires" this */
#endif
