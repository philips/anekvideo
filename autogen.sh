#!/bin/sh -e

# autogen.sh - anekvideo autogen
# 
# Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
# 
# AnekVideo is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version
# 2.1 as published by the Free Software Foundation.

autoreconf --install --symlink

MYCFLAGS="-g -Wall \
-Wmissing-declarations -Wmissing-prototypes \
-Wnested-externs -Wpointer-arith \
-Wpointer-arith -Wsign-compare -Wchar-subscripts \
-Wstrict-prototypes -Wshadow \
-Wformat=2 -Wtype-limits"

case "$CFLAGS" in
	*-O[0-9]*)
		;;
	*)
		MYCFLAGS="$MYCFLAGS -O2"
		;;
esac

libdir() {
        echo $(cd $1/$(gcc -print-multi-os-directory); pwd)
}

args="--prefix=/usr \
--sbindir=/sbin \
--libdir=$(libdir /usr/lib)"

export CFLAGS="$CFLAGS $MYCFLAGS"
./configure $args $@
