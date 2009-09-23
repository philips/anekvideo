/**
 * device - v4l device handling
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev.h>
#include <errno.h>

#include <sys/mman.h>
#include "defines.h"
#include "device.h"
#include "util.h"
#include "frame.h"

static int av_device_setup_buffers(struct av_device *dev);

static int av_device_destroy(struct av_device *dev)
{
	int fd = dev->fd;
	return close(fd);
}

struct av_device *av_device_new(struct av_server *server)
{
	struct av_device *dev = talloc(server, struct av_device);
	
	if (dev < 0) {
		talloc_free(dev);
		return NULL;
	}

	talloc_set_destructor(dev, av_device_destroy);
	av_clear(*dev);
	return dev;
}

int av_device_setup_from_path(struct av_device *dev, char *path)
{
	int ret;
	
	dev->fd = open(path, O_RDWR);
	if (dev->fd < 0) {
		av_log_error("open(%s) failed\n", path);
		return -1;
	}

	ret = av_ioctl(dev->fd, VIDIOC_QUERYCAP, &dev->cap);
	if (ret < 0) {
		av_log_error("VIDIOC_QUERYCAP failed\n");
		return -1;
	}

	if ((dev->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) {
		av_log_error("Device does not have V4L2_CAP_VIDEO_CAPTURE\n");
		return -1;
	}

        if (!(dev->cap.capabilities & V4L2_CAP_STREAMING)) {
		av_log_error("Device does not have V4L2_CAP_STREAMING\n");
		return -1;
        }
	
	/* TODO: do actual negotiation */
	dev->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	dev->fmt.fmt.pix.width = 0;
	dev->fmt.fmt.pix.height = 0;
	dev->fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	dev->fmt.fmt.pix.field = V4L2_FIELD_ANY;

	ret = av_ioctl(dev->fd, VIDIOC_S_FMT, &dev->fmt);
	if (ret < 0) {
		av_log_error("VIDIOC_S_FMT failed: %s.\n", strerror(errno));
		return -1;
	}

	/* VIDIOC_S_PARM for FPS */
	struct v4l2_streamparm* setfps;
	setfps = (struct v4l2_streamparm *)calloc(1, sizeof(struct v4l2_streamparm));
	memset(setfps, 0, sizeof(struct v4l2_streamparm));
	setfps->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	setfps->parm.capture.timeperframe.numerator = 1;
	setfps->parm.capture.timeperframe.denominator = 0;
	ret = av_ioctl(dev->fd, VIDIOC_S_PARM, setfps);

	av_device_setup_buffers(dev);

	return 0;
}

static int av_device_setup_buffers(struct av_device *dev)
{
	int ret;
	unsigned int i;
	struct v4l2_buffer *buf = &dev->buf;
	struct av_frame *f;

	av_assert(dev);

	/* VIDIOC_REQBUFS */
	memset(&dev->rb, 0, sizeof(struct v4l2_requestbuffers));
	dev->rb.count = MAX_DEVICE_FRAMES;
	dev->rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	dev->rb.memory = V4L2_MEMORY_MMAP;
	ret = av_ioctl(dev->fd, VIDIOC_REQBUFS, &dev->rb);
	if (ret < 0) {
		av_log_error("VIDIOC_REQBUFS failed: %s\n", strerror(errno));
		return -1;
	}

	av_log_debug("VIDIOC_REQBUFS returned %i buffers\n", dev->rb.count);
	av_assert(dev->rb.count > 2);

	/* VIDIOC_QUERYBUF and map to av_frame */
	for (i = 0; i < dev->rb.count; i++) {
		dev->frames[i] = av_frame_new_for_device(dev);
		if (!dev->frames[i]) {
			av_log_error("av_device_frame_new failed\n");
			return -1;
		}
		f = dev->frames[i];

		buf->index = i;
		buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf->memory = V4L2_MEMORY_MMAP;
		
		av_frame_copy_meta_from_v4l2_buffer(dev->frames[i], buf);
		f->mem = mmap(NULL, buf->length, PROT_READ | PROT_WRITE,
				MAP_SHARED, dev->fd, buf->m.offset);
		if (f->mem == MAP_FAILED) {
			av_log_error("mmap failed\n");
			return -1;
		}
	}

	return 0;
}

int av_device_copy_frame(struct av_device *dev, struct av_frame *f)
{
	struct v4l2_buffer buf;
	av_clear(buf);
	int err;

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (av_ioctl(dev->fd, VIDIOC_DQBUF, &buf) < 0) {
		err = errno;
		av_log_error("VIDIOC_DQBUF failed: %s\n", strerror(err));
		return -err;
	}

	av_assert(buf.index < dev->rb.count);

	return 0;
}

struct av_device *av_device_new_from_path(struct av_server *server, char *path)
{
	int ret;

	struct av_device *dev;

	av_log_debug("start\n");

	dev = av_device_new(server);
	if (dev == NULL) {
		perror("av_device_new");
		exit(1);
	}

	ret = av_device_setup_from_path(dev, path);
	if (ret) {
		talloc_free(dev);
		return NULL;
	}

	return dev;
}
