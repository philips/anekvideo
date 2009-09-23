#ifndef _AV_RANDOM_H
#define _AV_RANDOM_H
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


#include <sys/types.h>

void av_random_seed(void);
void av_random(void *ret_data, size_t length);

#endif
