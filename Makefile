NAME = lc

PREFIX	= /usr/local

DIRNAME	= lc-1.1
TARFILE	= /tmp/lc-1.1.tar.gz

INSTALL	= cp

all: $(NAME)

$(NAME): $(NAME).c
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).c

install: all
	cp $(NAME) ${PREFIX}/bin/$(NAME)

clean:
	rm -f a.out core $(NAME) *.o
	rm -f $(DIRNAME)

distribution:
	test -d $(DIRNAME) || ln -s . $(DIRNAME)
	tar czf $(TARFILE) \
	    ./$(DIRNAME)/Makefile \
	    ./$(DIRNAME)/README \
	    ./$(DIRNAME)/*.[hc18]
