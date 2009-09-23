#ifndef _AV_ATOMIC_H
#define _AV_ATOMIC_H

/*
 * This file is part of AnekVideo
 *
 * Code from PulseAudio is under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version:
 *
 *  Copyright 2006 Lennart Poettering
 *  Copyright 2008 Nokia Corporation
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

#include "util.h"
#include "defines.h"

/*
 * atomic_ops guarantees us that sizeof(AO_t) == sizeof(void*).  It is
 * not guaranteed however, that sizeof(AO_t) == sizeof(size_t).
 * however very likely.
 *
 * For now we do only full memory barriers. Eventually we might want
 * to support more elaborate memory barriers, in which case we will add
 * suffixes to the function names.
 *
 * On gcc >= 4.1 we use the builtin atomic functions. otherwise we use
 * libatomic_ops
 */

#ifndef PACKAGE
#error "Please include config.h before including this file!"
#endif

/* __sync based implementation */

struct av_atomic {
    volatile int value;
};

#define AV_ATOMIC_INIT(v) { .value = (v) }

static inline int av_atomic_load(const struct av_atomic *a) {
    __sync_synchronize();
    return a->value;
}

static inline void av_atomic_store(struct av_atomic *a, int i) {
    a->value = i;
    __sync_synchronize();
}

/* Returns the previously set value */
static inline int av_atomic_add(struct av_atomic *a, int i) {
    return __sync_fetch_and_add(&a->value, i);
}

/* Returns the previously set value */
static inline int av_atomic_sub(struct av_atomic *a, int i) {
    return __sync_fetch_and_sub(&a->value, i);
}

/* Returns the previously set value */
static inline int av_atomic_inc(struct av_atomic *a) {
    return av_atomic_add(a, 1);
}

/* Returns the previously set value */
static inline int av_atomic_dec(struct av_atomic *a) {
    return av_atomic_sub(a, 1);
}

/* Returns TRUE when the operation was successful. */
static inline int av_atomic_cmpxchg(struct av_atomic *a, int old_i, int new_i) {
    return __sync_bool_compare_and_swap(&a->value, old_i, new_i);
}

struct av_atomic_ptr {
    volatile unsigned long value;
};

#define AV_ATOMIC_PTR_INIT(v) { .value = (long) (v) }

static inline void* av_atomic_ptr_load(const struct av_atomic_ptr *a) {
    __sync_synchronize();
    return (void*) a->value;
}

static inline void av_atomic_ptr_store(struct av_atomic_ptr *a, void *p) {
    a->value = (unsigned long) p;
    __sync_synchronize();
}

static inline int av_atomic_ptr_cmpxchg(struct av_atomic_ptr *a, void *old_p, void* new_p) {
    return __sync_bool_compare_and_swap(&a->value, (long) old_p, (long) new_p);
}

#endif
