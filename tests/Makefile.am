
test_common_cflags = -I$(top_srcdir)/src -Wall -W -Wno-unused-parameter -Wno-pointer-sign
test_common_ldflags = -L$(top_srcdir)/src/.libs

bin_PROGRAMS = ucl_test noc_test nos_test

ucl_test_SOURCES = ucl_test.c
ucl_test_CFLAGS = $(test_common_cflags)
ucl_test_LDADD = $(LIBUCL_LDADD)
ucl_test_LDFLAGS = $(LIBUCL_LDFLAGS)

noc_test_SOURCES = noc_test.c
noc_test_CFLAGS = $(test_common_cflags)
noc_test_LDADD = -lnereon
noc_test_LDFLAGS = $(test_common_ldflags)

nos_test_SOURCES = nos_test.c
nos_test_CFLAGS = $(test_common_cflags)
nos_test_LDADD = -lnereon
nos_test_LDFLAGS = $(test_common_ldflags)
