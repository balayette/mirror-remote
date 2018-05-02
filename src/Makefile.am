AM_CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g \
	@libgit2_CFLAGS@ \
	@libmicrohttpd_CFLAGS@ \
	@openssl_CFLAGS@

AM_LDFLAGS = @libgit2_LIBS@ @libmicrohttpd_LIBS@ @openssl_LIBS@

bin_PROGRAMS = mirror
mirror_SOURCES = main.c \
	mirror.h mirror.c \
	server.h server.c \
	crypto.h crypto.c \
	gitmgr.h gitmgr.c \
	logging.h logging.c \
	confmgr.h confmgr.c \
	utils.h utils.c \
	joylent/http_parser.h joylent/http_parser.c
