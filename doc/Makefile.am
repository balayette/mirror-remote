if HAVE_DOXYGEN
if HAVE_PDFLATEX
doc.pdf:
	$(DOXYGEN) doxyconfig
	+$(MAKE) -C doc/latex
	mv doc/latex/refman.pdf ./doc.pdf
	rm -rf ./doc

all-local: doc.pdf
clean-local:
	rm -f doc.pdf
else
doc:
	$(DOXYGEN) doxyconfig
all-local: doc
clean-local:
	rm -rf ./doc
endif
endif
