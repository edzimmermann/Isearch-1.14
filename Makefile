#
# Compiling and Installing Isearch
#
# 1)  Type `make'
#
# 2)  Type `make install' (as root), to copy executables to /usr/local/bin/.
#       or `make install INSTALL=~/bin' to copy them to ~/bin/.
#       etc.
#
# 3)  Typing `make clean' will clean up .o files.
#
# The executables are
#
#	Iindex	- command line indexing utility
#	Isearch	- command line searching utility
#	Iutil - command line utilities for Isearch databases
#
SHELL=/bin/sh

#
# Compiler
#
CC=g++
#CC=/sw/CenterLine/bin/CC

#
# Compiler Flags
#
# Uncomment the appropriate entry
#
#CFLAGS=-g -DUNIX -Wall -Dfar=

CFLAGS=-O2 -DUNIX
#CFLAGS=-g -DUNIX -W -Wno-unused
#CFLAGS=-g -DUNIX	# CenterLine

#
# Install Directory
#
# Where should I install executables (make install)?
#
INSTALL=/usr/local/bin

#
# Executables Directory
#
# Where should I place executables?
#
BIN_DIR=bin

#
# Document Type Directory
#
# Where are document type sources located?
#
DOCTYPE_DIR=doctype

#
# Doctype dependent libraries (-lgdbm, etc)
DOCLIB=

#
# Source Directory
#
# Where are search engine sources located?
#
SRC_DIR=src

#
# Isearch-cgi Directory
#
# Where is the CGI gateway code?
#
CGI_DIR=Isearch-cgi

# 
# That should be all you need to configure
#

RM = rm -f
LDFLAGS=
VER=1.14
DIST=Isearch-$(VER)
BINDIST=$(DIST)-bin
freezename=`echo $(VER) | sed 's/\./-/g'`
OSNAME=`uname -s`
OSVER=`uname -r`
OS=$(OSNAME)_$(OSVER)

all: isearch isearch-cgi done
all: isearch done

isearch:
	`if [ -f src/conf.h ] ; \
		then true ; \
		else echo ./configure ; \
	fi`
	cd $(DOCTYPE_DIR); make "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	cd $(SRC_DIR); make "BIN_DIR=../$(BIN_DIR)" \
			"DOCTYPE_DIR=../$(DOCTYPE_DIR)" \
			"CFLAGS=$(CFLAGS)" "CC=$(CC)" \
			"DOCLIB=$(DOCLIB)" "LDFLAGS=$(LDFLAGS)"

isearch-cgi: isearch
	cd $(CGI_DIR); make "BIN_DIR=../$(BIN_DIR)" \
			"LIB_DIR=../$(BIN_DIR)" "ISEARCH_DIR=.." \
			"CFLAGS=$(CFLAGS)" "CC=$(CC)" \
			"DOCLIB=$(DOCLIB)" "LDFLAGS=$(LDFLAGS)"

done:
	@echo ""
	@echo "Welcome to CNIDR Isearch version $(VER)!"
	@echo ""
	@echo "Read the README file for configuration and installation instructions"
	@echo ""

clean:
	$(RM) *~ $(BIN_DIR)/Iindex $(BIN_DIR)/Isearch $(BIN_DIR)/Iutil \
		$(BIN_DIR)/libIsearch.a $(BIN_DIR)/core
	cd $(SRC_DIR); make -i clean
	cd $(DOCTYPE_DIR); make -i clean
	cd $(CGI_DIR); make -i clean

realclean:
	$(RM) *~ $(BIN_DIR)/Iindex $(BIN_DIR)/Isearch $(BIN_DIR)/Iutil \
		$(BIN_DIR)/libIsearch.a $(BIN_DIR)/core config.* \
		Makefile.all
	cd $(SRC_DIR); make -i realclean
	cd $(DOCTYPE_DIR); make -i clean
	cd $(CGI_DIR); make -i clean

distclean:
	$(RM) *~ $(BIN_DIR)/Iindex $(BIN_DIR)/Isearch $(BIN_DIR)/Iutil \
		$(BIN_DIR)/libIsearch.a $(BIN_DIR)/core config.* \
		Makefile.all
	cd $(SRC_DIR); make -i distclean
	cd $(DOCTYPE_DIR); make -i clean
	cd $(CGI_DIR); make -i clean

binclean:
	$(RM) *~ $(BIN_DIR)/Iindex $(BIN_DIR)/Isearch $(BIN_DIR)/Iutil \
		$(BIN_DIR)/libIsearch.a $(BIN_DIR)/core
	cd $(CGI_DIR);$(RM) isrch_fetch isrch_srch search_form

build:
	`if [ -e src/conf.h ] ; \
		then make -i realclean; \
		else true; \
	fi`
	make all

oldinstall:
	@echo "*** Copying Isearch executables to $(INSTALL)/. ***"
	cp $(BIN_DIR)/Iindex $(INSTALL)/.
	cp $(BIN_DIR)/Isearch $(INSTALL)/.
	cp $(BIN_DIR)/Iutil $(INSTALL)/.
	cd $(CGI_DIR); make install

bindir=/opt/Isearch/bin
install:
	test -d $(bindir) || mkdirhier $(bindir)
	-cp -f -p $(BIN_DIR)/Iindex $(bindir)
	-cp -f -p $(BIN_DIR)/Isearch $(bindir)
	-cp -f -p $(BIN_DIR)/Iutil $(bindir)
	-chmod -R a+r  $(bindir)
#	cd $(CGI_DIR); make install

srcdist:
	make -i distclean;cd ..;tar cvf $(DIST).tar $(DIST);gzip $(DIST).tar

bindist:
	cd ..; \
	tar cvf $(DIST)_$(OS)$(LDFLAGS).tar \
		$(DIST)/bin/Iindex \
		$(DIST)/bin/Isearch \
		$(DIST)/bin/Iutil \
		$(DIST)/Isearch-cgi/Configure \
		$(DIST)/Isearch-cgi/README \
		$(DIST)/Isearch-cgi/isrch_fetch \
		$(DIST)/Isearch-cgi/isrch_srch \
		$(DIST)/Isearch-cgi/search_form \
		$(DIST)/doc \
		$(DIST)/README \
		$(DIST)/CHANGES \
		$(DIST)/TUTORIAL \
		$(DIST)/html  \
		$(DIST)/COPYRIGHT; \
		gzip $(DIST)_$(OS)$(LDFLAGS).tar

