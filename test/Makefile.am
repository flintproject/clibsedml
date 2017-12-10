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
	check_reader_l1v3_parameter \
	check_writer \
	check_rw

TESTS = $(check_PROGRAMS)
