libgit2 = dependency('libgit2')
libmicrohttpd = dependency('libmicrohttpd')
openssl = dependency('openssl')

configure_file(output: 'config.h',
               configuration: conf)

sources = [
    'main.c', 
    'server.h', 'server.c',
    'crypto.h', 'crypto.c',
    'gitmgr.h', 'gitmgr.c',
    'logging.h', 'logging.c',
    'confmgr.h', 'confmgr.c',
    'utils.h', 'utils.c',
    'url_parser.h', 'url_parser.c',
    'memutils.h', 'memutils.c'
]

if thread.found() 
    sources += ['thread_pool.h', 'thread_pool.c']
endif

executable('mirror-remote', sources, 
           dependencies: [libgit2, libmicrohttpd, openssl, thread],
           install: true
           )
