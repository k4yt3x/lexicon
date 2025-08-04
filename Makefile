.PHONY: all shellcode build debug clean

CC=clang
SRCDIR=src
INCDIR=include
BINDIR=bin
OBJDIR=obj
CFLAGS=-Wall -fPIC -I$(INCDIR)

# specify DJB2=1 to enable DJB2 hashing lookup
DJB2=1
ifeq ($(DJB2), 1)
	CFLAGS+=-DDJB2
endif

# specify NOSIGNAL=1 to disable signal handler abuse
NOSIGNAL=0
ifeq ($(NOSIGNAL), 1)
	CFLAGS+=-DNOSIGNAL
endif

fast: prepare shellcode
	$(CC) $(CFLAGS) -O3 -ffast-math -s $(SRCDIR)/lexicon.c -o $(BINDIR)/lexicon

build: prepare shellcode
	$(CC) $(CFLAGS) -s $(SRCDIR)/lexicon.c -o $(BINDIR)/lexicon

debug: prepare shellcode
	$(CC) $(CFLAGS) -g -DDEBUG $(SRCDIR)/lexicon.c -o $(BINDIR)/lexicon

custom: prepare
	ld -o $(BINDIR)/shellcode.bin -N -Ttext 0x0 --oformat binary $(OBJDIR)/shellcode.o
	python3 tools/encoder.py $(BINDIR)/shellcode.bin $(INCDIR)/shellcode.h
	$(CC) $(CFLAGS) -O3 -ffast-math -s $(SRCDIR)/lexicon.c -o $(BINDIR)/lexicon

shellcode: prepare
	nasm -f elf64 $(SRCDIR)/shellcode.nasm -o $(OBJDIR)/shellcode.o
	ld -o $(BINDIR)/shellcode.bin -N -Ttext 0x0 --oformat binary $(OBJDIR)/shellcode.o
	python3 tools/encoder.py $(BINDIR)/shellcode.bin $(INCDIR)/shellcode.h

prepare:
	mkdir -p $(BINDIR) $(OBJDIR)

clean:
	rm -f $(SRCDIR)/*.o $(BINDIR)/lexicon
