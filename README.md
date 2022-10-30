# lc - better than ls

lc - "list catalog" - an alternative to the venerable ls command from
the University of Waterloo's MFCF.

lc is an alternative to ls, that has been in wide use at the
University of Waterloo (and elsewhere) since at least the
mid 1970's.

I believe it originated on the Math Faculty Computing Facility's
Honeywell system ("watbun") running GCOS, and was ported from
there to the MFCF UNIX systems.

Unlike ls, lc displays objects by type, showing "Directories:",
"Files:", "Pipes:", "Unsatisfied Symbolic Links:", among other
file system object types.

## Alternative

If C is inconvenient for you, you may enjoy RJ White's
"lc - a ls alternative" reimplementation in Perl at
https://github.com/rjwhite/lc

## Distributions

I don't think lc is available in any Linux distributions,
but it is (currently) available as a FreeBSD pkg/port,
in misc/lc.

## History

At the time I added these files to this git repository in
mid-2022, the files in my lc-1.0.tar.gz tarball had the dates:

```
-rw-r--r-- 1 jsellens jsellens  430 Nov 13  1999 Makefile
-rw-r--r-- 1 jsellens jsellens  563 Aug  3 23:15 README.md
-rw-r--r-- 1 jsellens jsellens 2387 Nov 13  1999 lc.1
-rw-r--r-- 1 jsellens jsellens 7885 Oct 23  1996 lc.c
```

and the RCS log showed an initial revision of 1987/05/14
and the code mentioned 1978.

All that to say, this code has been around for a while.

Once I got used to lc (before I even used UNIX), I knew that
it was the "one true way", and have dragged it around with
me ever since.

Enjoy

John Sellens
