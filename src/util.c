/*
 * This file is part of AnekVideo
 *
 * Code from PulseAudio is under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version:
 *
 *  Copyright 2006 Lennart Poettering
 *  Copyright 2006 Pierre Ossman <ossman@cendio.se> for Cendio AB
 *  Copyright 2004 Joe Marcus Clarke
 *
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 *
 * AnekVideo is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version
 * 2.1 as published by the Free Software Foundation.
 *
 * AnekVideo is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with AnekVideo; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 */

#include <sys/ioctl.h>
#include "util.h"

/* Convert the string s to an unsigned integer in *ret_u */
int av_atou(const char *s, uint32_t *ret_u)
{
	char *x = NULL;
	unsigned long l;

	av_assert(s);
	av_assert(ret_u);

	errno = 0;
	l = strtoul(s, &x, 0);

	if (!x || *x || errno) {
		if (!errno)
			errno = EINVAL;
		return -1;
	}

	if ((uint32_t) l != l) {
		errno = ERANGE;
		return -1;
	}

	*ret_u = (uint32_t) l;

	return 0;
}

ssize_t av_read(int fd, void *buf, size_t count, int *type)
{
	for (;;) {
		ssize_t r;

		if ((r = read(fd, buf, count)) < 0)
			if (errno == EINTR)
				continue;

		return r;
	}
}

ssize_t av_write(int fd, const void *buf, size_t count, int *type)
{
	if (!type || *type == 0) {
		ssize_t r;

		for (;;) {
			if ((r = send(fd, buf, count, MSG_NOSIGNAL)) < 0) {

				if (errno == EINTR)
					continue;

				break;
			}

			return r;
		}

		if (errno != ENOTSOCK)
			return r;

		if (type)
			*type = 1;
	}

	for (;;) {
		ssize_t r;

		if ((r = write(fd, buf, count)) < 0)
			if (errno == EINTR)
				continue;

		return r;
	}
}

/** Calls read() in a loop. Makes sure that as much as 'size' bytes,
 * unless EOF is reached or an error occurred */
ssize_t av_loop_read(int fd, void*data, size_t size, int *type)
{
	ssize_t ret = 0;
	int _type;

	av_assert(fd >= 0);
	av_assert(data);
	av_assert(size);

	if (!type) {
		_type = 0;
		type = &_type;
	}

	while (size > 0) {
		ssize_t r;

		if ((r = av_read(fd, data, size, type)) < 0)
			return r;

		if (r == 0)
			break;

		ret += r;
		data = (uint8_t*) data + r;
		size -= (size_t) r;
	}

	return ret;
}

/** Similar to av_loop_read(), but wraps write() */
ssize_t av_loop_write(int fd, const void*data, size_t size, int *type)
{
	ssize_t ret = 0;
	int _type;

	av_assert(fd >= 0);
	av_assert(data);
	av_assert(size);

	if (!type) {
		_type = 0;
		type = &_type;
	}

	while (size > 0) {
		ssize_t r;

		if ((r = av_write(fd, data, size, type)) < 0)
			return r;

		if (r == 0)
			break;

		ret += r;
		data = (const uint8_t*) data + r;
		size -= (size_t) r;
	}

	return ret;
}

int av_close(int fd)
{
	for (;;) {
		int r;

		if ((r = close(fd)) < 0)
			if (errno == EINTR)
				continue;

		return r;
	}
}

int av_ioctl(int fd, int request, void *arg)
{
	for (;;) {
		int r;
		if ((r = ioctl(fd, request, arg)) < 0)
			if (errno == EINTR)
				continue;
	
		return r;
	}
}
