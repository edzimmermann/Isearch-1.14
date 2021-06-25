SHELL=/bin/sh
#
# Compiler
#
CC=g++

#
# Compiler Flags
#
# Uncomment the appropriate entry
#
#CFLAGS=-g -DUNIX -Wall -Dfar=

#CFLAGS=-DUNIX -O2
#CFLAGS=-g -DUNIX -Wall -O2
CFLAGS=-g -DUNIX -Wall -D__CSTRING__

#
# Source Files Directory
#
# Where are search engine sources located?
#
SRC_DIR=../src

# 
# That should be all you need to configure
#

OBJ= dtconf.o

H=

INC= 

RM= rm -f

all: $(OBJ) dtconf ../src/dtreg.hxx

../src/dtreg.hxx: dtconf.inf
	./dtconf

dtconf.o:$(H) dtconf.cxx
	$(CC) $(CFLAGS) $(INC) -c dtconf.cxx

dtconf:$(OBJ) dtconf.cxx
	$(CC) $(CFLAGS) -o dtconf $(OBJ)
	./dtconf

clean:
	$(RM) *~ *.o core dtconf ../src/dtreg.hxx

build:
	make -i clean
	make all
