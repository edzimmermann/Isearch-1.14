#
# CNIDR Isearch-cgi
#
# Answer a couple of questions and type 'make'
#

# Compiler  
# Must be an ANSI c++ compiler
#
CC=g++

# Compiler Flags
# Nothing fancy
#
#CFLAGS=-g -Wall
CFLAGS=-O2 -W -DUNIX

# Isearch Source
# Where is your CNIDR Isearch code?
#
ISEARCH_DIR=..
ISEARCH_LIB=libIsearch.a
LIB_DIR=$(ISEARCH_DIR)/bin
BIN_DIR=$(ISEARCH_DIR)/bin

# cgi-bin Directory
# What is the path to your httpd server's cgi-bin directory?
#
CGIBIN_PATH=/usr/etc/httpd/cgi-bin
CGIBIN_PATH=/opt/Isearch/httpd/cgi-bin

# Thats all!  Type 'make'
#
LDFLAGS=
INC=-I$(ISEARCH_DIR)/src
LIB=-lIsearch -lm
OBJ=cgi-util.o
H=config.hxx
VER=1.04
DIST=Isearch-cgi-$(VER)

# all: $(LIB_DIR)/$(ISEARCH_LIB) isrch_srch.o isrch_fetch.o \
#     search_form.o cgi-util.o isrch_srch isrch_fetch search_form done

all: isrch_srch isrch_fetch search_form done

cgi-util.o: cgi-util.hxx cgi-util.cxx $(H)
	$(CC) $(CFLAGS) $(INC) -c cgi-util.cxx

isrch_srch.o:isrch_srch.cxx $(H)
	$(CC) $(CFLAGS) $(INC) -c isrch_srch.cxx

isrch_fetch.o:isrch_fetch.cxx $(H)
	$(CC) $(CFLAGS) $(INC) -c isrch_fetch.cxx

search_form.o:search_form.cxx $(H)
	$(CC) $(CFLAGS) $(INC) -c search_form.cxx

isrch_srch:isrch_srch.o $(H) $(OBJ) # $(LIB_DIR)/$(ISEARCH_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o isrch_srch isrch_srch.o $(OBJ) -L$(LIB_DIR) $(LIB)

isrch_fetch:isrch_fetch.o $(H) $(OBJ) # $(LIB_DIR)/$(ISEARCH_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o isrch_fetch isrch_fetch.o $(OBJ) -L$(LIB_DIR) $(LIB)

search_form:search_form.o $(H) $(OBJ) # $(LIB_DIR)/$(ISEARCH_LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o search_form search_form.o $(OBJ) -L$(LIB_DIR) $(LIB)

$(ISEARCH_LIB):$(LIB_DIR)/$(ISEARCH_LIB)
	@echo ""

$(LIB_DIR)/$(ISEARCH_LIB):
	cd $(ISEARCH_DIR);make isearch

done:
	@echo ""
	@echo "Welcome to CNIDR Isearch-cgi version $(VER)!"
	@echo ""
	@echo "Read the README file for configuration and installation instructions"
	@echo ""

srcdist:
	cp README ../$(DIST).README;make clean;cd ..;tar cvf $(DIST).tar $(DIST);gzip $(DIST).tar

clean:
	rm -f *.o *~ isrch_srch isrch_fetch search_form isearch ifetch *html

install:
	test -d $(CGIBIN_PATH) || mkdirhier $(CGIBIN_PATH)
	cp isearch ifetch $(CGIBIN_PATH)

