# Generated automatically from Makefile.in by configure.
#
# $Id: Makefile.in,v 1.7 2000/10/26 15:16:42 withnail Exp $
#

CC=gcc
YACC=byacc
LEX=flex

INSTALL=./install-sh
DICTPATH=/usr/local/etc/raddb
DICT=$(DICTPATH)/dictionary
BINDEST=/usr/local/bin
MANDEST=/usr/local/man/man1

VERSION=0.66
INCLUDES=-I. -I/usr/include
CFLAGS=-g -O2  -DSTDC_HEADERS=1 -DHAVE_IF_ETHER_H=1 -DRETSIGTYPE=void -DHAVE_ALLOCA=1 -D_BSD_SOURCE=1 -D__FAVOR_BSD=1 -DBSD=1 -DHAVE_LIBPCAP=1 -DPCAP_RESTART=pcap_restart  -DVERSIONSTR=\"$(VERSION)\" -DDICTFILE=\"$(DICT)\"
LIBS=-lpcap 

SRC=radstock.c process.c init.c 
OBJS=radstock.o process.o init.o y.tab.o lex.yy.o
TARGET=radstock
MANPAGE=radstock.1
DICTLOCAL=dictionary
PKGINFO=pkginfo.c

all: $(TARGET)

pkg: install
	$(CC) -E $(CFLAGS) $(PKGINFO) > pkginfo
	mkdir -p pkg
	pkgmk -o -r /usr/local -f radstock.proto -d pkg
	pkgtrans -s pkg radstock-$(VERSION).inst radstock
	gzip pkg/radstock-$(VERSION).inst

tgz: 
	mkdir -p ../radstock-$(VERSION)
	cp -pr * ../radstock-$(VERSION)
	(cd ../radstock-$(VERSION) ; make distclean)
	rm -rf ../radstock-$(VERSION)/CVS*
	rm -rf ../radstock-$(VERSION)/radfilters/CVS*
	(cd ..; tar cf radstock-$(VERSION).tar radstock-$(VERSION) )
	gzip ../radstock-$(VERSION).tar	

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

debug: $(OBJS)
	$(CC) $(CFLAGS) -g -o $(TARGET) $(OBJS) $(LIBS) 

static: $(OBJS)
	$(CC) $(CFLAGS) -s -static -o $(TARGET).static $(OBJS) $(LIBS) 

install: $(TARGET)
	$(INSTALL) -c -m 0755 $(TARGET) $(BINDEST)
	$(INSTALL) -c -m 0644 $(MANPAGE) $(MANDEST)
	$(INSTALL) -d -m 0755 $(DICTPATH)
	$(INSTALL) -c -m 0644 $(DICTLOCAL) $(DICT)

.c.o:	
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $<

y.tab.h: grammar.y
	$(YACC) -d grammar.y

y.tab.c: grammar.y
	$(YACC) -d grammar.y

lex.yy.c: scanner.l grammar.y
	$(LEX) scanner.l

clean:
	rm -f *~ $(OBJS) $(TARGET) y.tab.h y.tab.c lex.yy.c

distclean: clean
	rm -f config.status config.cache config.log Makefile .depends

radstock.o: radstock.c radstock.h y.tab.h version.h
init.o: init.c radstock.h y.tab.h
process.o: process.c radstock.h y.tab.h

tags: $(SRC)
	$(TAGS) $(SRC)


