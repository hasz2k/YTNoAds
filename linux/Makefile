CC         ?= gcc
PKG_CONFIG ?= pkg-config
INSTALL    ?= install

PKGS   := gtk4 webkitgtk-6.0
TARGET := ytnoads
SRC    := main.c
OBJ    := $(SRC:.c=.o)
DEP    := $(OBJ:.o=.d)

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# Zmienne użytkownika (CFLAGS, LDFLAGS) można nadpisać z linii poleceń lub środowiska.
CFLAGS  ?= -O2
CFLAGS  += -std=gnu11 -Wall -Wextra $(shell $(PKG_CONFIG) --cflags $(PKGS))
LDLIBS  := $(shell $(PKG_CONFIG) --libs $(PKGS))

.PHONY: all debug install uninstall clean

all: $(TARGET)

debug: CFLAGS += -O0 -g3
debug: all

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# -MMD -MP generuje pliki .d, dzięki czemu zmiana nagłówka przebuduje zależne pliki
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

install: $(TARGET)
	$(INSTALL) -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ) $(DEP)

-include $(DEP)
