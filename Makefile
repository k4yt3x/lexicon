.PHONY: static dynamic debug clean

SRCDIR=src
BINDIR=bin

dynamic:
	mkdir -p $(BINDIR)
	gcc -Wall -fPIC -I$(SRCDIR) -s $(SRCDIR)/lexikon.c -o $(BINDIR)/lexikon

static:
	mkdir -p $(BINDIR)
	gcc -Wall -fPIC -I$(SRCDIR) -static -s $(SRCDIR)/lexikon.c -o $(BINDIR)/lexikon

debug:
	mkdir -p $(BINDIR)
	gcc -Wall -fPIC -I$(SRCDIR) -g -DDEBUG $(SRCDIR)/lexikon.c \
		-no-pie -fno-stack-protector -Wl,-z,norelro -z execstack \
		-o $(BINDIR)/lexikon

clean:
	rm -f $(SRCDIR)/*.o $(BINDIR)/lexikon

