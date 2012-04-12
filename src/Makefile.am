lib_LTLIBRARIES = libsedml.la
libsedml_la_SOURCES = document.c mathml.c reader.c writer.c xhtml.c
libsedml_la_CPPFLAGS = $(XML_CPPFLAGS)
libsedml_la_LIBADD = $(XML_LIBS)
libsedml_la_LDFLAGS = -no-undefined

nobase_include_HEADERS = \
	sedml/common.h \
	sedml/document.h \
	sedml/mathml.h \
	sedml/reader.h \
	sedml/version.h \
	sedml/writer.h \
	sedml/xhtml.h
