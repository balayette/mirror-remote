AM_CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g \
	@libgit2_CFLAGS@ \
	@libmicrohttpd_CFLAGS@ \
	@openssl_CFLAGS@ \
	$(PTHREAD_CFLAGS)

AM_LDFLAGS = @libgit2_LIBS@ \
	@libmicrohttpd_LIBS@ \
	@openssl_LIBS@ \
	$(PTHREAD_LIBS)

bin_PROGRAMS = mirror
mirror_SOURCES = main.c \
	server.h server.c \
	crypto.h crypto.c \
	gitmgr.h gitmgr.c \
	logging.h logging.c \
	confmgr.h confmgr.c \
	utils.h utils.c \
	url_parser.h url_parser.c \
	memutils.h memutils.c

if BUILD_PTHREAD
mirror_SOURCES += thread_pool.h thread_pool.c
endif
