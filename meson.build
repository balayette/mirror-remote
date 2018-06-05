project('mirror-remote', 'c', version: '0.1', default_options: ['c_std=c11'])
conf = configuration_data()

conf.set('PACKAGE_NAME', 'mirror-remote')
conf.set('PACKAGE_VERSION', '0.1')

thread = dependency('threads', required: false)

doxygen = find_program('doxygen', required: false)

pdflatex = find_program('pdflatex', required: false)
if pdflatex.found() 
    conf.set('GEN_HTML', 'NO')
    conf.set('GEN_LATEX', 'YES')
else
    conf.set('GEN_HTML', 'YES')
    conf.set('GEN_LATEX', 'NO')
endif

dot = find_program('dot', required: false)
if dot.found()
    conf.set('USE_DOT', 'YES')
else
    conf.set('USE_DOT', 'NO')
endif

conf.set('top_srcdir', '@0@/../src'.format(meson.current_source_dir()))

if thread.found()
    conf.set('HAVE_PTHREAD', true)
else
    conf.set('HAVE_PTHREAD', false)
endif

subdir('doc')
subdir('src')
subdir('man')



