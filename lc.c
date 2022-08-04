#ifndef lint
static char *copyright =
	"LC - Copyright University of Waterloo, 1978,1985,1987";
#endif /* lint */
/*
 * lc [directory ...]
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
typedef struct dirent DirType;

#ifndef MAXPATHLEN
#	define	MAXPATHLEN	1024			/* For older systems */
#endif

		/* this assumes that S_IFMT is a mask of contiguous 1s */
#define	IFMT_OFFSET	12			/* may be system-dependent */
#define	IFMT		(S_IFMT >> IFMT_OFFSET)

#define	MODEX(m)	(((m)>>IFMT_OFFSET)&IFMT) /* IFMT to table index */

#define	NMALLOC		100		/* how much to malloc in growlist() */
#define COLUMNWIDTH	15		/* number of characters in a column */
#define INDENT		"    "		/* blanks at beginning of a column */

static struct filelist {
	char **names;		/* pointer to array of names */
	int size;		/* how many elements */
	int end;		/* next element to use */
	int flag;
	char *title;
}
	dlist = {(char **) NULL, 0, 0, 0, "Directories"},
	flist = {(char **) NULL, 0, 0, 0, "Files"},
	plist = {(char **) NULL, 0, 0, 0, "Pipes"},
	blist = {(char **) NULL, 0, 0, 0, "Block Spec. Files"},
	clist = {(char **) NULL, 0, 0, 0, "Char. Spec. Files"},
	mpclist = {(char **) NULL, 0, 0, 0, "MPX Char. Files"},
	mpblist = {(char **) NULL, 0, 0, 0, "MPX Block Files"},
	symlist = {(char **) NULL, 0, 0, 0, "Unsatisfied Symbolic Links"},
	soclist = {(char **) NULL, 0, 0, 0, "Sockets"},
	doorlist = {(char **) NULL, 0, 0, 0, "Doors"},
				/* flag is always on for this list */
	goklist = {(char **) NULL, 0, 0, 1, "**GOK"};

static struct filelist *printlist[] = {
	&dlist, &flist, &plist, &blist, &clist, &mpclist, &mpblist, &symlist,
	&soclist, &doorlist, &goklist, 0
};

static struct filelist *listtype[IFMT+1];

static struct xinit {
	int	x_flag;
	struct	filelist	*x_list;
} xinit[] ={
#ifdef S_IPIPE
	S_IPIPE,	&plist,
#endif
#ifdef S_IFPIP
	S_IFPIP,	&plist,
#endif
#ifdef S_IFIFO
	S_IFIFO,	&plist,
#endif
	S_IFREG,	&flist,
	S_IFDIR,	&dlist,
	S_IFCHR,	&clist,
	S_IFBLK,	&blist,
#ifdef S_IFMPC
	S_IFMPC,	&mpclist,
#endif
#ifdef S_IFMPB
	S_IFMPB,	&mpblist,
#endif
#ifdef S_IFLNK
	S_IFLNK,	&symlist,
#endif
#ifdef S_IFSOCK
	S_IFSOCK,	&soclist,
#endif
#ifdef S_IFDOOR
	S_IFDOOR,	&doorlist,
#endif
	-1,
};

static int nfl;		/* flag; this is not the first line of output */
static int errcall;	/* flag; last line of output was error message */
static int notfirst;	/* flag; means not the first thing in this directory */
static int nflg;	/* flag; no output - just want exit status */
static int ncols = 5;	/* number of columns */
static int linewidth;
static int manyflg;
static int anyfound;
static int all  = 1;
static char pathbuf[MAXPATHLEN];
static char *fnptr = pathbuf;
static int aflg;


	static void
warn(name)
	char *name;
{
	if(nfl && !errcall)
		putchar('\n');
	fprintf(stdout, "%s:  %s\n", name, strerror(errno));
	fflush(stdout);
	errcall=nfl=1;
	errno=0;
}

	static void
growlist( list )
	struct filelist *list;
{
	register int oldsize, newsize;
	register char **newlist;

	oldsize = list->size * sizeof (char *);
	newsize = oldsize + (NMALLOC * sizeof (char *));

	newlist = (char **) malloc( newsize );

	if (oldsize != 0) {
		memcpy( newlist, list->names, oldsize );
		free( (char *)list->names );
	}

	list->names = newlist;
	list->size += NMALLOC;
}

	static void
build(list, name)
	struct filelist *list;
	char *name;
{
	register int n;

	if(list->flag == 0)
		return;
	anyfound++;
	if(nflg)
		return;
	if (list->end >= list->size)
		growlist(list);
	n = strlen( name ) + 1;
	
	(list->names)[list->end++] = strncpy( malloc( n ), name, n );
}

	static void
release( list )
	struct filelist *list;
{
	register char **p;
	register int i;

	for (p = list->names, i = 0; i < list->end; i++)
		free( p[i] );

	list->end = 0;
}

	static void
print(list)
	struct filelist *list;
{
	register int cursor, i;
	register char **p;

	if(notfirst++)
		putchar('\n');
	if(ncols!=1) {
		if (manyflg)
			fputs(INDENT, stdout);
		printf("%s:\n", list->title);
	}
	cursor = 0;
	for(p = list->names, i = 0; i < list->end; i++) {
		int len;
		int posused;	/* positions used by blanks and filename */
		register char *fname;

		fname = p[i];
		len = strlen(fname);
		posused = len + ((cursor == 0) ? 0 : COLUMNWIDTH - cursor%COLUMNWIDTH);
		if ((cursor + posused) > linewidth) {
			printf("\n");
			cursor = 0;
			posused = len;
		}
		if (manyflg && (cursor == 0)) fputs(INDENT, stdout); 
		printf("%*s", posused, fname);
		cursor += posused;
	}
	if(cursor != 0)
		 printf("\n");
	errcall = 0;
	if (manyflg)
		release(list);
}

	static int
pstrcmp( pa, pb )
	char *pa, *pb;
{
	auto char *a = *(char**)pa;
	auto char *b = *(char**)pb;

	return (strcmp( a, b ));
}

	static void
listdir(path)
	char *path;
{
	register char *p;
	DirType *d;
	struct filelist **ppr, *pr;
	DIR *u;
	char *dirname;
	struct stat ibuf;

	if (!manyflg) {
		if (chdir(path) < 0) {
			warn(path);
			return;
		}
		dirname = ".";
	} else
		dirname = path;
	if (stat(dirname, &ibuf) < 0) {
		warn(path);
		return;
	}
	if ((ibuf.st_mode&S_IFMT) != S_IFDIR) {
		if(!manyflg){
			errno=ENOTDIR;
			warn(path);
		}
		return;
	}
	if ((u = opendir(dirname)) == NULL) {
		warn(path);
		return;
	}
	if(manyflg) {
		if(nfl++)
			putchar('\n');
		printf("%s:\n", path);
	}
	while((d = readdir(u)) != NULL) {
		if (!aflg) {	/* skip "." and ".." */
			p = d->d_name;
			if(*p++ == '.' && (*p == '\0'
			    || *p++ == '.' && *p == '\0'))
				continue;
		}
		if ((fnptr + strlen(d->d_name)) >= &pathbuf[MAXPATHLEN]) {
			errno=ENAMETOOLONG;
			warn(pathbuf);
/*
			err("%s: Component `%s' makes pathname > %d bytes.\n",
				pathbuf, d->d_name, MAXPATHLEN);
*/
			continue;
		}
		(void)strcpy(fnptr, d->d_name);
		if(stat(pathbuf, &ibuf) < 0 && lstat(pathbuf, &ibuf) < 0) {
			warn(pathbuf);
			continue;
		}
		build(listtype[MODEX(ibuf.st_mode)], d->d_name);
	}

	closedir(u);
	notfirst = 0;
	for (ppr = printlist; 0 != (pr = *ppr); ppr++)
		if(pr->names && pr->end != 0) {
			qsort( (char *)pr->names, pr->end,
					sizeof(char *), pstrcmp );
			print(pr);
		}
	fflush(stdout);
}


	int /*GOTO*/
main(argc, argv)
	char **argv;
{
	register char *path, *p, *q;
	struct xinit *xp;
	int i;

	for (i = 0 ; i < IFMT ; i++)
		listtype[i] = &goklist;
	for (xp = xinit; xp->x_flag != -1; xp++)
		listtype[MODEX(xp->x_flag)] = xp->x_list;
#ifdef TIOCGWINSZ
	if (isatty(1)) {
		struct winsize win;
		if (ioctl(1, TIOCGWINSZ, &win) != -1) {
			ncols = (win.ws_col == 0 ? 5 : (win.ws_col / COLUMNWIDTH));
			ncols = ncols ? ncols : 1;	/* can't have 0 columns */
		}
	}
#endif /* TIOGCWINSZ */
	if(argc >= 2 && argv[1][0] == '-') {
		argv++;
		argc--;
		for(p = &argv[0][1]; *p; p++)
			switch(*p) {
			case 'a':
				aflg++;
				break;

			case 'n':
				nflg++;
				break;

			case '1':
				ncols=1;
				break;

			default:
				switch(*p) {
				case 'f':
					flist.flag++;
					break;

				case 'd':
					dlist.flag++;
					break;

				case 'b':
					blist.flag++;
					break;

				case 'c':
					clist.flag++;
					break;

				case 'B':
					mpblist.flag++;
					break;

				case 'C':
					mpclist.flag++;
					break;

				case 's':
					blist.flag++;
					clist.flag++;
					mpblist.flag++;
					mpclist.flag++;
					break;

				case 'p':
					plist.flag++;
					break;

				default:
					fprintf(stderr, "Unknown flag: %c\n",
						*p);
					continue;
				}
				all = 0;
			}
	}

	if (all)
		flist.flag = dlist.flag = blist.flag = clist.flag
		    = mpblist.flag = mpclist.flag
		    = symlist.flag = soclist.flag
		    = doorlist.flag = plist.flag = 1;

	linewidth = ncols * COLUMNWIDTH;

	if(argc < 3) {
		path = argc == 1 ? "." : argv[1];
		listdir(path);
	} else {
		manyflg++;
		while(--argc) {
			path = p = *++argv;
			if(strlen(path) >= MAXPATHLEN) {/* = for '/' */
				errno=ENAMETOOLONG;
				warn(path);
				continue;
			}
			q = pathbuf;
			while (0 != (*q++ = *p++));
			q[-1] = '/';
			fnptr = q;
			listdir(path);
		}
	}
	exit(anyfound==0);
}
