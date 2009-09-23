/**
 * frame - generic anekvideo frame
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include "frame.h"
#include "log.h"
#include "util.h"


static int av_frame_destroy(struct av_frame *f)
{
	av_log_debug("destroying av_frame %p", f);
	return 0;
}

static struct av_frame *av_frame_new(void *cnt)
{
	av_assert(cnt);

	struct av_frame *buf = talloc(cnt, struct av_frame);
	
	if (buf < 0) {
		talloc_free(buf);
		return NULL;
	}

	talloc_set_destructor(buf, av_frame_destroy);
	memset(buf, 0, sizeof(*buf));
	return buf;
}

struct av_frame *av_frame_new_for_server(struct av_server *srv)
{
	return av_frame_new((void *)srv);
}

struct av_frame *av_frame_new_for_client(struct av_server *client)
{
	return av_frame_new((void *)client);
}

struct av_frame *av_frame_new_for_device(struct av_device *dev)
{
	return av_frame_new((void *)dev);
}

int av_frame_copy_meta_from_v4l2_buffer(struct av_frame *dst, struct v4l2_buffer *src)
{
	dst->field = src->field;
	dst->length = src->length;
	dst->sequence = src->sequence;
	
	memcpy(&dst->timestamp, &src->timestamp, sizeof(dst->timestamp));
	memcpy(&dst->timecode, &src->timecode, sizeof(dst->timecode));

	return 0;
}

int av_frame_copy_meta(struct av_frame *dst, struct av_frame *src)
{
	dst->field = src->field;
	dst->length = src->length;
	dst->sequence = src->sequence;
	
	memcpy(&dst->timestamp, &src->timestamp, sizeof(dst->timestamp));
	memcpy(&dst->timecode, &src->timecode, sizeof(dst->timecode));

	return 0;
}

int av_frame_copy_all(struct av_frame *dst, struct av_frame *src)
{
	av_frame_copy_meta(dst, src);
	memcpy(&dst->length, &src->length, sizeof(dst->length));

	return 0;
}
