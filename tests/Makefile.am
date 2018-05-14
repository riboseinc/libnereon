
test_common_cflags = -I$(top_srcdir)/src -Wall -W -Wno-unused-parameter -Wno-pointer-sign
test_common_ldflags = -L$(top_srcdir)/src/.libs

bin_PROGRAMS = cmdline ucl_test

cmdline_SOURCES = cmdline.c
cmdline_CFLAGS = $(test_common_cflags)
cmdline_LDADD = -lmulticonfig
cmdline_LDFLAGS = $(test_common_ldflags)

ucl_test_SOURCES = ucl_test.c
ucl_test_CFLAGS = $(test_common_cflags)
ucl_test_LDADD = $(LIBUCL_LDADD)
ucl_test_LDFLAGS = $(LIBUCL_LDFLAGS)
