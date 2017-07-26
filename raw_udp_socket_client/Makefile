IDIR = include
LDIR = lib
SDIR = src
LFLAGS = -lpthread -lpcap
CFLAGS = -g -O2 -Wall -pedantic -ansi
CC = gcc

SUBDIRS = $(LDIR) $(SDIR)
OBJMAIN = $(SDIR)/client.o 

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: client

client: $(OBJMAIN)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
		mv client ~/sudo/snif

$(OBJSERV): prepare

.PHONY: prepare $(SUBDIRS)
prepare: $(SUBDIRS)

$(SUBDIRS):
		make -C $@

$(SDIR): $(LDIR)

clean:
		clear
		cd src	&& $(MAKE) clean
		cd lib	&& $(MAKE) clean
		rm -rf client *.o
		rm -rf ~/sudo/snif
