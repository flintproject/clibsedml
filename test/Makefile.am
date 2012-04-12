AM_CPPFLAGS = -I$(top_srcdir)/src $(XML_CPPFLAGS)

AM_LDFLAGS = $(top_builddir)/src/libsedml.la

check_PROGRAMS = check_mathml check_reader check_reader1 check_reader2 check_writer check_rw
check_mathml_SOURCES = check_mathml.c
check_reader_SOURCES = check_reader.c
check_reader1_SOURCES = check_reader1.c
check_reader2_SOURCES = check_reader2.c
check_writer_SOURCES = check_writer.c
check_rw_SOURCES = check_rw.c

TESTS = $(check_PROGRAMS)
