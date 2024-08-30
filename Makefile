.PHONY: all build debug clean

SRCDIR=src
BINDIR=bin
CFLAGS=-Wall -fPIC -I$(SRCDIR)

# specify DJB2=1 to enable DJB2 hashing lookup
ifeq ($(DJB2), 1)
    DJB2_FLAG=-DDJB2
else
    DJB2_FLAG=
endif

all: build

build:
	mkdir -p $(BINDIR)
	gcc $(CFLAGS) $(DJB2_FLAG) -s $(SRCDIR)/lexikon.c -o $(BINDIR)/lexikon

debug:
	mkdir -p $(BINDIR)
	gcc $(CFLAGS) $(DJB2_FLAG) -g -DDEBUG $(SRCDIR)/lexikon.c \
		-no-pie -fno-stack-protector -Wl,-z,norelro -z execstack \
		-o $(BINDIR)/lexikon

clean:
	rm -f $(SRCDIR)/*.o $(BINDIR)/lexikon

