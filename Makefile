NAME = lc

# These locations are fairly typical these days,
# but perhaps not universal.
PREFIX	= /usr/local
BINDIR	= ${PREFIX}/bin
MANDIR	= ${PREFIX}/share/man/man1

DIRNAME	= lc-1.1
TARFILE	= /tmp/lc-1.1.tar.gz

INSTALL	= install

all: $(NAME)

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

install: all
	$(INSTALL) -c -m 755 $(NAME) $(BINDIR)/.
	$(INSTALL) -c -m 644 $(NAME).1 $(MANDIR)/.

clean:
	rm -f a.out core $(NAME) *.o
	rm -f $(DIRNAME)

distribution:
	test -d $(DIRNAME) || ln -s . $(DIRNAME)
	tar czf $(TARFILE) \
	    ./$(DIRNAME)/Makefile \
	    ./$(DIRNAME)/README \
	    ./$(DIRNAME)/*.[hc18]
