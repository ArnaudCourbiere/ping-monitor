#IDIR=../include
CC=gcc
CFLAGS=-Wall

ODIR=./obj
BDIR=./bin
#LIBS=-lcurl

#_DEPS=utility.h
#DEPS=$(patsubst %, $(IDIR)/%, $(_DEPS))

_OBJ=main.o
OBJ=$(patsubst %, $(ODIR)/%, $(_OBJ))

all: $(ODIR) $(BDIR) $(BDIR)/ping_monitor
    
$(BDIR)/ping_monitor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR):
	mkdir -p $@

$(BDIR):
	mkdir -p $@

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -r $(ODIR) $(BDIR)

