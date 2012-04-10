AM_CPPFLAGS = -I$(top_srcdir)/src $(XML_CPPFLAGS)

AM_LDFLAGS = $(top_builddir)/src/libsedml.la $(XML_LIBS)

check_PROGRAMS = check_mathml check_reader check_writer check_rw
check_mathml_SOURCES = check_mathml.c
check_reader_SOURCES = check_reader.c
check_writer_SOURCES = check_writer.c
check_rw_SOURCES = check_rw.c

TESTS = $(check_PROGRAMS)
