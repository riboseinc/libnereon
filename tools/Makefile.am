
tools_common_cflags = -I$(top_srcdir)/src -Wall -W -Wno-unused-parameter -Wno-pointer-sign
tools_common_ldflags = -L$(top_srcdir)/src/.libs

bin_PROGRAMS = nos_to_cc

nos_to_cc_SOURCES = nos_to_cc.c
nos_to_cc_CFLAGS = $(tools_common_cflags)
nos_to_cc_LDADD = -lnereon
nos_to_cc_LDFLAGS = $(tools_common_ldflags)
