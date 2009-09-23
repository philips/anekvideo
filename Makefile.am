# Makefile.am - anekvideo automake
# 
# Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
# 
# AnekVideo is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version
# 2.1 as published by the Free Software Foundation.

SUBDIRS = .

EXTRA_DIST = autogen.sh

ACLOCAL_AMFLAGS = -I m4

AM_MAKEFLAGS = --no-print-directory

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I src -I ccan

AM_LDFLAGS = \
	-Wl,--as-needed

CLEANFILES =

# ------------------------------------------------------------------------------
# libanekcommon
# ------------------------------------------------------------------------------

lib_LTLIBRARIES = \
	libanekcommon.la 

ccan_sources = \
	ccan/talloc/talloc.h ccan/talloc/talloc.c \
	ccan/typesafe_cb/typesafe_cb.h \
	ccan/list/list.h ccan/list/list.c \
	ccan/container_of/container_of.h \
	ccan/check_type/check_type.h \
	ccan/build_assert/build_assert.h

libanekcommon_la_SOURCES = \
	$(ccan_sources) \
	src/device.h src/device.c \
	src/protocol.h src/protocol.c \
	src/server.h src/server.c \
	src/shm.h src/shm.c \
	src/util.c src/util.h \
	src/random.c src/random.h \
	src/frame.c src/frame.h \
	src/defines.h
#	client.h client.c 


libanekcommon_la_LDFLAGS =  -avoid-version

# ------------------------------------------------------------------------------
# daemon/server
# ------------------------------------------------------------------------------

#anekvideo_SOURCES = \
#	libanekcommon.la \
#	src/anekvideo.c

#anekclient_SOURCES = \
#	libanekcommon.la
#	src/anekclient.c

#bin_PROGRAMS = \
#	anekvideo \
#	anekclient

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

TESTS_BINARIES = \
	tests/log \
	tests/device \
	tests/shm

TESTS = \
	tests/log.test \
	tests/device.test \
	tests/shm.test

tests_log_SOURCES = tests/log.c
tests_log_CPPFLAGS = $(AM_CPPFLAGS)
tests_log_LDADD = libanekcommon.la

tests_device_SOURCES = tests/device.c $(server_common)
tests_device_CPPFLAGS = $(AM_CPPFLAGS)
tests_device_LDADD = libanekcommon.la

tests_shm_SOURCES = tests/shm.c
tests_shm_CPPFLAGS = $(AM_CPPFLAGS)
tests_shm_LDADD = libanekcommon.la

noinst_PROGRAMS = $(TESTS_BINARIES)

$(TESTS): $(TESTS_BINARIES)
	@echo "*** $@ ***"; perl tests/run $@

tests: $(TESTS)

.PHONY: $(TESTS) tests

# install, uninstall, clean hooks
# ------------------------------------------------------------------------------
distclean-local:
	rm -rf autom4te.cache

EXTRA_DIST += $(TESTS)
