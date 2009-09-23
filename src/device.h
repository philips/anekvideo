/**
 * device - v4l device handling
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#ifndef _AV_DEVICE_H
#define _AV_DEVICE_H

#include <linux/videodev2.h>
#include <stdint.h>

#define MAX_DEVICE_FRAMES 6

struct av_device {
	int fd;
	struct v4l2_capability cap;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers rb;
	struct v4l2_buffer buf;

	struct av_frame *frames[MAX_DEVICE_FRAMES];
};

struct av_server;

struct av_device *av_device_new(struct av_server *server);
struct av_device *av_device_new_from_path(struct av_server *server, char *path);
int av_device_setup_from_path(struct av_device *dev, char *path);
int av_device_copy_frame(struct av_device *dev, struct av_frame *f);


#endif /* _AV_DEVICE_H */
