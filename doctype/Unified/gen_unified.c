#include <stdio.h>
#include <string.h>

#define COMMENT_CHAR '#'

char prefix[] = "_S";
char misc[] = "other_fields";
char message[] = "/* Don't edit this file, it is produced automaticaly from %s */\n";

/* Note: if START_COUNT is negative it counts down */
#ifndef START_COUNT
# define START_COUNT -9000 /* Start "private" numbers here */
#endif

FILE *myopen(const char *ext)
{
  char buf[256];
  char rootname[] = "unified";

  sprintf(buf, "%s.%s", rootname, ext);
  return fopen(buf, "w");
}

main(int argc, char **argv)
{
 char buf[256];
 char temp[256];
 int i, value;
 int count=START_COUNT;
 int num = 0;

 FILE *fp1 = myopen("h");
 FILE *fp2 = myopen("c");
 FILE *fp3 = myopen("inc");

 if (argc != 1)
  freopen(argv[1], "r", stdin);

 if (fp1 == NULL || fp2 == NULL) {
   printf("ERROR\n");
   exit(-1);
 }

 fprintf(fp1, message, argc != 1 ? argv[1] : "<stdin>");
 fprintf(fp2, message, argc != 1 ? argv[1] : "<stdin>");
 fprintf(fp3, message, argc != 1 ? argv[1] : "<stdin>");
 fprintf(fp1, "#ifndef _UNIFIED_H\n#define _UNIFIED_H\n");
 fprintf(fp2, "const char *_UnifiedName(int id)\n{\n  switch (id) {\n");
 fprintf(fp3, "#ifndef _UNIFIED_INC\n#define _UNIFIED_INC\n");
 while (fgets(buf, 255, stdin) != NULL) {
  char *tcp;

  if ((tcp = strchr(buf, COMMENT_CHAR)) != NULL)
    *tcp = '\0';

  value = 0;
  for (i=0; buf[i]; i++)
    if (buf[i] == '-') temp[i] = '_';
    else if (isspace(buf[i])) {
	temp[i] = buf[i] = 0;
	if (isdigit(buf[i+1]))
	  value = atoi(&buf[i+1]); /* Have a Bib-1 number */
    } else temp[i] = tolower(buf[i]);
  temp[i]=0;
  if (temp[0] == 0) continue;
  if (value == 0)
    value=(START_COUNT > 0 ? count++ : count--); /* Private numbers */
  fprintf(fp1, "# define %s%s _UnifiedName(%d)\n", prefix, temp, value);
  fprintf(fp2, "  case %5d: return \"%s\";\n", value, buf);
  fprintf(fp3, "# define %s%s \"%s\"\n", prefix, temp, buf);
  num++;
 }
 fprintf (fp1, "# define %s%s _UnifiedName(%d)\n", prefix, misc, count);
 fprintf (fp2, "  default:\n\treturn \"%s\";\n  }\n}", misc);
 fprintf (fp3, "# define %s%s \"%s\"\n", prefix, misc, misc);
 fprintf(fp1, "\
\n\n\nextern \n\
#ifdef __cplusplus\n\
  \"C\" {\n\
#endif\n\
const char *_UnifiedName(int);\n\
#ifdef __cplusplus\n\
};\n\
#endif\n"); 
 fprintf(fp1, "#endif\n");
 fprintf(fp3, "#endif\n");
 printf("Processed %d entries.\n", num);
 return (0);
}

