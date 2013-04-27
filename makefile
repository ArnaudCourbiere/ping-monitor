-include config.mk

PREFIX?=/usr/local
INSTALL_BIN=$(PREFIX)/bin
ODIR=./obj
BDIR=./bin
BFILE=ping_monitor
BFILE_PATH=$(BDIR)/$(BFILE)

INSTALL=install
CC=gcc
CFLAGS=-Wall

_OBJ=main.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

all: $(ODIR) $(BDIR) $(BFILE_PATH)
    
$(BFILE_PATH): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR):
	mkdir -p $@

$(BDIR):
	mkdir -p $@

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean distclean intall uninstall

clean:
	rm -rf $(ODIR) $(BDIR)

distclean:
	rm -f config.mk

install: all
	mkdir -p $(INSTALL_BIN)
	$(INSTALL) $(BFILE_PATH) $(INSTALL_BIN)

uninstall:
	rm -f  $(INSTALL_BIN)/$(BFILE)