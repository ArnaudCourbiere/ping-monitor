-include config.mk

PREFIX?=/usr/local
INSTALL_BIN=$(PREFIX)/bin
ODIR=./obj
BDIR=./bin
BFILE=ping-monitor
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
	tools/install.py '$(INSTALL_BIN)'
	update-rc.d ping-monitor defaults

uninstall:
	-(/etc/init.d/ping-monitor stop)
	rm -rf  $(INSTALL_BIN)/$(BFILE) /etc/init.d/ping-monitor /var/run/ping-monitor
	update-rc.d ping-monitor remove
