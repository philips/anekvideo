/**
 * buffer - a simple expandable buffer using talloc
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#ifndef _AV_BUFFER_H
#define _AV_BUFFER_H

#define BUF_CHUNK 1024

struct av_buffer {
	void *buf;
	int used;
	int size;
};

struct av_buffer *buffer_init(void);
void av_buffer_free(struct av_buffer *b);
int av_buffer_grow(struct av_buffer *b, size_t size);

#endif /* _AV_DEVICE_H */
