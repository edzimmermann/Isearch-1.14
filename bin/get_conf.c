/*
   get_conf.c 
*/

#include <stdio.h>
#include <sys/utsname.h>

#define CONFFILE "conf.h"
#define MAKEFILE "makefile.cnf"

char *include_paths[] =     { "/usr/include","/usr/local/include","/usr/include/X11R6","/opt/xpm/include","" };
char *x11_include_paths[] = { "/usr/include","/usr/local/include","/usr/X11/include","/usr/local/X11/include",
			      "/usr/include/X11R6","/usr/contrib/X11R6/include","" };
char *x11_lib_paths[]=      { "/usr/lib/X11R6","/usr/local/lib","/usr/X11/lib","/usr/contrib/X11R6/lib","" };

int 
search_path(paths, name, file, dname)
char **paths, *name;
FILE *file;
char *dname;
{
  int  n=0,found=0;
  FILE *test;
  char fname[128];

  while((paths[n][0]!=0)&&(!found)) {
    strcpy(fname,paths[n++]);
    strcat(fname,"/");
    strcat(fname,name);

    test=fopen(fname,"r");
    if(test!=NULL) {
      found=1;
      if(file!=NULL)
	fprintf(file,"%s = %s\n",dname,paths[n-1]);
      fclose(test);
    }
  }
  return found;
}

int 
search_file(paths, name, file, define, def_name)
char **paths, *name;
FILE *file;
char *define, *def_name;
{
  int  n=0,t=0;
  FILE *test;
  char fname[128];
  while(paths[n][0]!=0) {
    strcpy(fname,paths[n++]);
    strcat(fname,"/");
    strcat(fname,name);

    test=fopen(fname,"r");
    if(test!=NULL) {
      t=1;
      fclose(test);
    }
  }

  if(!t) {
    printf("%s disabled\n",def_name);
    fprintf(file,"#define no%s\t/* %s disabled */\n",define,def_name);
  } else {
    printf("%s enabled\n",def_name);
    fprintf(file,"#define %s\t/* %s enabled */\n",define,def_name);
  }
  return t;
}

int 
main()
{
  int test,xpm=0,pid,status;
  FILE *file,*mfile;
  struct utsname name;

  if( (( file=fopen(CONFFILE,"w"))!=NULL)&&
      ((mfile=fopen(MAKEFILE,"a"))!=NULL)) {
    test = 0x001122ff;
    if(*(char*)&test) {
      printf("little endian detected\n");
      fprintf(file,"#ifndef LITTLE_ENDIAN\n#define LITTLE_ENDIAN\n#endif\n");
    } else {
      printf("big endian detected\n");
      fprintf(file,"#ifndef BIG_ENDIAN\n#define BIG_ENDIAN\n#endif\n");
    }

    /* search for Xpm headers */
    if(search_file(include_paths,"X11/xpm.h",file,
		"XPM","XPM library")) {
      fprintf(mfile,"XPM     =1\nXPMLIB  =-L/opt/xpm/lib/X11 -lxpm -lm\nXPMDEF  = -DXPM -I/opt/xpm/include\n");
      xpm=1;
    }




    /* search for X11 */
    if(!search_path(x11_include_paths,"X11/X.h",mfile,"X11INC"))
      printf("Error: unable to find X11 header files\n");
    if((!search_path(x11_lib_paths,"libX11.a",mfile,"X11LIB"))&&
       (!search_path(x11_lib_paths,"libX11.sl",mfile,"X11LIB")))
      printf("Error: unable to find X11 library files\n");

    /* search for x11 shared memory extension */
    if(!search_file(x11_include_paths,"X11/extensions/XShm.h",file,
		"XSHM","X11 shared memory extension"))
      printf("Use of shared memory extension is STRONGLY recommended\n");

    /* was no audio driver enabled?? */
    if(!xpm) fprintf(mfile,"AUDIO     = noaudio\nXPM=0\n");

    /* see if socket lib is present */
    if(search_path(x11_lib_paths, "libsocket.a", NULL, "SOCKET")) {
      puts("adding nsl and socket to library list (solaris, eh?)");
      fprintf(mfile,"XTRA_LIBS = -lnsl -lsocket\n");
    }

    fclose(file);
    fclose(mfile);
  } else {
    printf("Sorry, unable to open 'conf.h'!\n");
    return(1);
  }

  return(0);
}
