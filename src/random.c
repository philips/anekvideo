/*
 * This file is part of AnekVideo
 *
 * Code from PulseAudio is under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version:
 *
 *  Copyright 2006 Lennart Poettering 
 *  Copyright 2006 Pierre Ossman <ossman@cendio.se> for Cendio AB
 *
 * Copyright (C) 2009 Brandon Philips <brandon@ifup.org>
 *
 * AnekVideo is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * AnekVideo is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with AnekVideo; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#include "log.h"
#include "random.h"
#include "defines.h"
#include "util.h" 

static int has_whined = TRUE;

static const char * const devices[] = { "/dev/urandom", "/dev/random", NULL };

static int random_proper(void *ret_data, size_t length) {
	int fd, ret = -1;
	ssize_t r = 0;
	const char *const * device;

	av_assert(ret_data);
	av_assert(length > 0);

	device = devices;

	while (*device) {
		ret = 0;

		if ((fd = open(*device, O_RDONLY | O_NOCTTY)) >= 0) {
			if ((r = av_loop_read(fd, ret_data, length, NULL)) < 0 || (size_t) r != length)
				ret = -1;

			av_close(fd);
		} else
			ret = -1;

		if (ret == 0)
			break;

		device++;
	}

	return ret;
}

void av_random_seed(void) {
	unsigned int seed;

	if (random_proper(&seed, sizeof(unsigned int)) < 0) {

		if (!has_whined) {
			av_log_warn("Failed to get proper entropy. Falling back to seeding with current time.");
			has_whined = TRUE;
		}

		seed = (unsigned int) time(NULL);
	}

	srand(seed);
}

void av_random(void *ret_data, size_t length) {
	uint8_t *p;
	size_t l;

	av_assert(ret_data);
	av_assert(length > 0);

	if (random_proper(ret_data, length) >= 0)
		return;

	if (!has_whined) {
		av_log_warn("Failed to get proper entropy. Falling back to unsecure pseudo RNG.");
		has_whined = TRUE;
	}

	for (p = ret_data, l = length; l > 0; p++, l--)
		*p = (uint8_t) rand();
}
