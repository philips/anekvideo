/**
 * frame - an anekvideo frame
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include "server.h"

struct av_frame {
	uint32_t		refcount;
	enum v4l2_field		field;
	struct timeval		timestamp;
	struct v4l2_timecode	timecode;
	uint32_t		sequence;
	uint32_t		length;

	uint32_t		reserved0;
	uint32_t		reserved1;
	uint32_t		reserved2;
	uint32_t		reserved3;

	void *mem;		/* shared memory where the frame is copied */
};

struct av_frame *av_frame_new_for_server(struct av_server *srv);
struct av_frame *av_frame_new_for_client(struct av_server *client);
struct av_frame *av_frame_new_for_device(struct av_device *dev);
int av_frame_copy_meta_from_v4l2_buffer(struct av_frame *dst, struct v4l2_buffer *src);
int av_frame_copy_meta(struct av_frame *dst, struct av_frame *src);
int av_frame_copy_all(struct av_frame *dst, struct av_frame *src);
