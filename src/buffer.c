/**
 * buffer - a simple expandable buffer using talloc
 * 
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 * 
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 */

#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "defines.h"

struct av_buffer *buffer_init(void)
{
	struct buffer *b;

	b = malloc(sizeof(*b));
	assert(b);

	b->buf = NULL;
	b->size = 0;
	b->used = 0;

	return b;
}

void av_buffer_free(struct av_buffer *b)
{
	if (!b)
		return;

	free(b->buf);
	free(b);
}

int av_buffer_grow(struct av_buffer *b, size_t size)
{
        if (!b)
		return -1;

	if (size < b->size)
		return 0;

	b->size += size;
	b->size += BUF_CHUNK - (b->size % BUF_CHUNK);

	if (!b->buf) {
		b->buf = malloc(b->size);
		b->used = 0;
	} else {
		b->buf = realloc(b->buf, b->size);
	}

	debugprintf("grew buffer %p to %d\n", b, b->size);

	assert(b->buf);

        return 0;
}


