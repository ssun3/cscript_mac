PREFIX ?= /usr/local
DESTDIR ?=
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/share/man

CFLAGS ?= -O3
CFLAGS += -std=gnu99 -Wall

all: cscript

cscript:

install: cscript
	@install -v -d "$(DESTDIR)$(BINDIR)" && install -v -m 0755 cscript "$(DESTDIR)$(BINDIR)/cscript"

clean:
	rm -f cscript

.PHONY: all clean install
