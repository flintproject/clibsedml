AM_CPPFLAGS = -I$(top_srcdir)/src $(XML_CPPFLAGS)

AM_LDFLAGS = $(top_builddir)/src/libsedml.la

check_PROGRAMS = \
	check_document \
	check_mathml \
	check_reader \
	check_reader1 \
	check_reader2 \
	check_reader_l1v3_ikappab \
	check_reader_l1v3_leloup \
	check_reader_l1v3_lorenz_cellml \
	check_reader_l1v3_lorenz_sbml \
	check_reader_l1v3_oscli \
	check_writer \
	check_rw

check_document_SOURCES = check_document.c
check_mathml_SOURCES = check_mathml.c
check_reader_SOURCES = check_reader.c
check_reader1_SOURCES = check_reader1.c
check_reader2_SOURCES = check_reader2.c
check_reader_l1v3_ikappab_SOURCES = check_reader_l1v3_ikappab.c
check_reader_l1v3_leloup_SOURCES = check_reader_l1v3_leloup.c
check_reader_l1v3_lorenz_cellml_SOURCES = check_reader_l1v3_lorenz_cellml.c
check_reader_l1v3_lorenz_sbml_SOURCES = check_reader_l1v3_lorenz_sbml.c
check_reader_l1v3_oscli_SOURCES = check_reader_l1v3_oscli.c
check_writer_SOURCES = check_writer.c
check_rw_SOURCES = check_rw.c

TESTS = $(check_PROGRAMS)
