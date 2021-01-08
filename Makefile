IDIR =./
CC=gcc
CFLAGS=-Wall -I .

ODIR=.

LIBS=-lm

_DEPS_LIB_BANK = myBank.h
DEPS_LIB_BANK = $(patsubst %,$(IDIR)/%,$(_DEPS_LIB_BANK))

_OBJ_LIB_BANK = myBank.o
OBJ_LIB_BANK = $(patsubst %,$(ODIR)/%,$(_OBJ_LIB_BANK))

TARGET_BANK_LIB = libmyBank
TARGET_LOWER_BANK_LIB = mybank
TARGET_MAIN = main

# Generic target for objects
$(ODIR)/%.o: %.c $(DEPS_LIB_BANK)
	$(CC) -fpic -c -o $@ $< $(CFLAGS)

# Generic target for shared libraries
$(ODIR)/%.so: $(OBJ_LIB_BANK)
	$(CC) -shared -o $@ $^ $(CFLAGS)

# Generic target for static libraries
$(ODIR)/%.a: $(OBJ_LIB_BANK)
	ar rcs $@ $^

# Static libmyBank target
$(TARGET_LOWER_BANK_LIB)s: $(TARGET_BANK_LIB).a

# Static mains target
$(TARGET_MAIN)s: $(TARGET_MAIN).o $(TARGET_BANK_LIB).a
	$(CC) -o $@ $^ $(CFLAGS)

all: $(TARGET_MAIN)s

.PHONY: clean

clean:
	rm -f $(ODIR)/maind $(ODIR)/mains $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.a *~ $(INCDIR)/*~