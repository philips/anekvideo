/**
 * defines - global definitions
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#ifndef _AV_DEFINES_H
#define _AV_DEFINES_H

#define AV_SOCK_PATH "/tmp/v4lproxy.sock"
#define AV_VIDEO_PATH "/dev/video0"
#define MAX_MSG_SIZE 8192


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#include <stdio.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <ccan/talloc/talloc.h>

#endif /* _AV_DEFINES_H */
