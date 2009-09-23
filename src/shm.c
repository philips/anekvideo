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

#include <signal.h>
#include "atomic.h"
#include "util.h"
#include "shm.h"
#include "random.h"

#define SHM_PATH "/dev/shm/"

/* TODO: figure out atomic stuff in PA. Not needed now though */
/* Magic marker to discover owning PID */
struct shm_marker {
	struct av_atomic marker; /* 0xc0ffee11 */
	struct av_atomic pid;
};

#define SHM_MARKER ((int) 0xc0ffee11)
#define SHM_MARKER_SIZE AV_ALIGN(sizeof(struct shm_marker))
#define SHM_NAME_PREFIX "anek-shm-"

static char *segment_name(char *fn, size_t l, unsigned id)
{
	snprintf(fn, l, "/" SHM_NAME_PREFIX "%u", id);
	return fn;
}

int av_shm_create_rw(struct av_shm *m, size_t size, int shared, mode_t mode)
{
	char fn[32];
	int fd = -1;

	av_assert(m);
	av_assert(size > 0);
	av_assert(size <= MAX_SHM_SIZE);
	av_assert(mode >= 0600);

	/* Each time we create a new SHM area, let's first drop all stale
	 * ones */
	av_shm_cleanup();

	/* Round up to make it page aligned */
	size = AV_PAGE_ALIGN(size);

	if (!shared) {
		m->id = 0;
		m->size = size;

		if ((m->ptr = mmap(NULL, m->size, PROT_READ|PROT_WRITE, 
					MAP_ANONYMOUS|MAP_PRIVATE, 
					-1, (off_t) 0)) == MAP_FAILED) {
			av_log_error("mmap() failed: %s", strerror(errno));
			goto fail;
		}

		m->do_unlink = FALSE;

	} else {
		struct shm_marker *marker;

		av_random(&m->id, sizeof(m->id));
		segment_name(fn, sizeof(fn), m->id);

		if ((fd = shm_open(fn, O_RDWR|O_CREAT|O_EXCL, mode & 0444)) < 0) {
			av_log_error("shm_open() failed: %s", strerror(errno));
			goto fail;
		}

		m->size = size + SHM_MARKER_SIZE;

		if (ftruncate(fd, (off_t) m->size) < 0) {
			av_log_error("ftruncate() failed: %s", strerror(errno));
			goto fail;
		}

		if ((m->ptr = mmap(NULL, AV_PAGE_ALIGN(m->size), PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t) 0)) == MAP_FAILED) {
			av_log_error("mmap() failed: %s", strerror(errno));
			goto fail;
		}

		/* We store our PID at the end of the shm block, so that we
		 * can check for dead shm segments later */
		marker = (struct shm_marker*) ((uint8_t*) m->ptr + m->size - SHM_MARKER_SIZE);
		av_atomic_store(&marker->pid, (int) getpid());
		av_atomic_store(&marker->marker, SHM_MARKER);

		av_assert_se(av_close(fd) == 0);
		m->do_unlink = TRUE;
	}

	m->shared = shared;

	return 0;

fail:
	if (fd >= 0) {
		shm_unlink(fn);
		av_close(fd);
	}

	return -1;
}

int av_shm_attach_ro(struct av_shm *m, unsigned id) {
	char fn[32];
	int fd = -1;
	struct stat st;

	av_assert(m);

	segment_name(fn, sizeof(fn), m->id = id);

	if ((fd = shm_open(fn, O_RDONLY, 0)) < 0) {
		if (errno != EACCES) {
			av_log_error("shm_open() failed: %s", strerror(errno));
		}
		goto fail;
	}

	if (fstat(fd, &st) < 0) {
		av_log_error("fstat() failed: %s", strerror(errno));
		goto fail;
	}

	if (st.st_size <= 0 ||
		st.st_size > (off_t) (MAX_SHM_SIZE+SHM_MARKER_SIZE) ||
		AV_ALIGN((size_t) st.st_size) != (size_t) st.st_size) {
		av_log_error("Invalid shared memory segment size");
		goto fail;
	}

	m->size = (size_t) st.st_size;

	if ((m->ptr = mmap(NULL, AV_PAGE_ALIGN(m->size), 
				PROT_READ, MAP_SHARED, fd, 
				(off_t) 0)) == MAP_FAILED) {
		av_log_error("mmap() failed: %s", strerror(errno));
		goto fail;
	}

	m->do_unlink = FALSE;
	m->shared = TRUE;

	av_assert_se(av_close(fd) == 0);

	return 0;

fail:
	if (fd >= 0)
		av_close(fd);

	return -1;
}

void av_shm_free(struct av_shm *m) {
	av_assert(m);
	av_assert(m->ptr);
	av_assert(m->size > 0);

	av_assert(m->ptr != MAP_FAILED);

	if (!m->shared) {
		if (munmap(m->ptr, m->size) < 0)
			av_log_error("munmap() failed: %s", strerror(errno));
			
	} else {
		if (munmap(m->ptr, AV_PAGE_ALIGN(m->size)) < 0)
			av_log_error("munmap() failed: %s", strerror(errno));

		if (m->do_unlink) {
			char fn[32];

			segment_name(fn, sizeof(fn), m->id);

			if (shm_unlink(fn) < 0)
				av_log_error(" shm_unlink(%s) failed: %s", fn, strerror(errno));
		}
	}

	memset(m, 0, sizeof(*m));
}

int av_shm_cleanup(void) {
	DIR *d;
	struct dirent *de;

	if (!(d = opendir(SHM_PATH))) {
		av_log_warn("Failed to read "SHM_PATH": %s", strerror(errno));
		return -1;
	}

	while ((de = readdir(d))) {
		struct av_shm seg;
		unsigned id;
		pid_t pid;
		char fn[128];
		struct shm_marker *m;

		if (strncmp(de->d_name, SHM_NAME_PREFIX, strlen(SHM_NAME_PREFIX)))
			continue;

		if (av_atou(de->d_name + strlen(SHM_NAME_PREFIX), &id) < 0)
			continue;

		if (av_shm_attach_ro(&seg, id) < 0)
			continue;

		if (seg.size < SHM_MARKER_SIZE) {
			av_shm_free(&seg);
			continue;
		}

		m = (struct shm_marker*) ((uint8_t*) seg.ptr + seg.size - SHM_MARKER_SIZE);

		if (av_atomic_load(&m->marker) != SHM_MARKER) {
			av_shm_free(&seg);
			continue;
		}

		if (!(pid = (pid_t) av_atomic_load(&m->pid))) {
			av_shm_free(&seg);
			continue;
		}

		if (kill(pid, 0) == 0 || errno != ESRCH) {
			av_shm_free(&seg);
			continue;
		}

		av_shm_free(&seg);

		/* Ok, the owner of this shms segment is dead, so, let's remove the segment */
		segment_name(fn, sizeof(fn), id);

		if (shm_unlink(fn) < 0 && errno != EACCES && errno != ENOENT)
			av_log_warn("Failed to remove SHM segment %s: %s\n",
					 fn, strerror(errno));
	}

	closedir(d);

	return 0;
}
