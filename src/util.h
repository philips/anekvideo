/*
 * This file is part of AnekVideo
 *
 * Code from PulseAudio is under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version:
 *
 *  Copyright 2004-2006 Lennart Poettering
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

#ifndef _AV_UTIL_H
#define _AV_UTIL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/socket.h>


#include "log.h"
#include "defines.h"

#define av_clear(x) memset(&(x), 0, sizeof(x))

int av_atou(const char *s, uint32_t *ret_u);
int av_close(int fd);
int av_ioctl(int fd, int request, void *arg);
ssize_t av_loop_write(int fd, const void*data, size_t size, int *type);
ssize_t av_loop_read(int fd, void*data, size_t size, int *type);
ssize_t av_write(int fd, const void *buf, size_t count, int *type);
ssize_t av_read(int fd, void *buf, size_t count, int *type);


#ifndef AV_LIKELY
#ifdef __GNUC__
#define AV_LIKELY(x) (__builtin_expect(!!(x),1))
#define AV_UNLIKELY(x) (__builtin_expect(!!(x),0))
#else
#define AV_LIKELY(x) (x)
#define AV_UNLIKELY(x) (x)
#endif
#endif

#if defined(PAGE_SIZE)
#define AV_PAGE_SIZE ((size_t) PAGE_SIZE)
#elif defined(PAGESIZE)
#define AV_PAGE_SIZE ((size_t) PAGESIZE)
#elif defined(HAVE_SYSCONF)
#define AV_PAGE_SIZE ((size_t) (sysconf(_SC_PAGE_SIZE)))
#else
/* Let's hope it's like x86. */
#define AV_PAGE_SIZE ((size_t) 4096)
#endif

/* Rounds down */
static inline void* av_align_ptr(const void *p) {
    return (void*) (((size_t) p) & ~(sizeof(void*)-1));
}
#define AV_ALIGN_PTR(x) (av_align_ptr(x))

/* Rounds up */
static inline size_t av_align(size_t l) {
    return (((l + sizeof(void*) - 1) / sizeof(void*)) * sizeof(void*));
}
#define AV_ALIGN(x) (av_align(x))

/* Rounds down */
static inline void* av_page_align_ptr(const void *p) {
    return (void*) (((size_t) p) & ~(AV_PAGE_SIZE-1));
}
#define AV_PAGE_ALIGN_PTR(x) (av_page_align_ptr(x))

/* Rounds up */
static inline size_t av_page_align(size_t l) {
    return ((l + AV_PAGE_SIZE - 1) / AV_PAGE_SIZE) * AV_PAGE_SIZE;
}
#define AV_PAGE_ALIGN(x) (av_page_align(x))


/* av_assert_se() is an assert which guarantees side effects of x,
 * i.e. is never optimized away, regardless of NDEBUG or FASTPATH. */
#define av_assert_se(expr)                                              \
    do {                                                                \
        if (AV_UNLIKELY(!(expr))) {                                     \
            av_log_error("Assertion '%s' failed at %s:%u, function %s(). Aborting.", #expr , __FILE__, __LINE__, __func__); \
            abort();                                                    \
        }                                                               \
    } while (FALSE)

/* Does exactly nothing */
#define av_nop() do {} while (FALSE)

/* av_assert() is an assert that may be optimized away by defining
 * NDEBUG. av_assert_fp() is an assert that may be optimized away by
 * defining FASTPATH. It is supposed to be used in inner loops. It's
 * there for extra paranoia checking and should probably be removed in
 * production builds. */
#ifdef NDEBUG
#define av_assert(expr) av_nop()
#define av_assert_fp(expr) av_nop()
#elif defined (FASTPATH)
#define av_assert(expr) av_assert_se(expr)
#define av_assert_fp(expr) av_nop()
#else
#define av_assert(expr) av_assert_se(expr)
#define av_assert_fp(expr) av_assert_se(expr)
#endif

#endif /* _AV_UTIL_H */
