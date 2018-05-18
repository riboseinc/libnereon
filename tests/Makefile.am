
test_common_cflags = -I$(top_srcdir)/src -Wall -W -Wno-unused-parameter -Wno-pointer-sign
test_common_ldflags = -L$(top_srcdir)/src/.libs

bin_PROGRAMS = cli_test ucl_test cfg_test cfg_test1

cli_test_SOURCES = cli_test.c test_util.c
cli_test_CFLAGS = $(test_common_cflags)
cli_test_LDADD = -lmulticonfig
cli_test_LDFLAGS = $(test_common_ldflags)

ucl_test_SOURCES = ucl_test.c test_util.c
ucl_test_CFLAGS = $(test_common_cflags)
ucl_test_LDADD = $(LIBUCL_LDADD)
ucl_test_LDFLAGS = $(LIBUCL_LDFLAGS)

cfg_test_SOURCES = cfg_test.c test_util.c
cfg_test_CFLAGS = $(test_common_cflags)
cfg_test_LDADD = $(LIBUCL_LDADD)
cfg_test_LDFLAGS = $(LIBUCL_LDFLAGS)

cfg_test1_SOURCES = cfg_test1.c
cfg_test1_CFLAGS = $(test_common_cflags)
cfg_test1_LDADD = -lmulticonfig
cfg_test1_LDFLAGS = $(test_common_ldflags)
