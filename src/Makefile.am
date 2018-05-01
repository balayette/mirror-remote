AM_CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g @libgit2_CFLAGS@ @libmicrohttpd_CFLAGS@
AM_LDFLAGS = @libgit2_LIBS@ @libmicrohttpd_LIBS@

bin_PROGRAMS = mirror
mirror_SOURCES = main.c \
	mirror.h mirror.c
