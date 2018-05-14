
test_common_cflags = -I$(top_srcdir)/src -Wall -W -Wno-unused-parameter -Wno-pointer-sign
test_common_ldflags = -L$(top_srcdir)/src/.libs

bin_PROGRAMS = cli_test ucl_test

cli_test_SOURCES = cli_test.c
cli_test_CFLAGS = $(test_common_cflags)
cli_test_LDADD = -lmulticonfig
cli_test_LDFLAGS = $(test_common_ldflags)

ucl_test_SOURCES = ucl_test.c
ucl_test_CFLAGS = $(test_common_cflags)
ucl_test_LDADD = $(LIBUCL_LDADD)
ucl_test_LDFLAGS = $(LIBUCL_LDFLAGS)
