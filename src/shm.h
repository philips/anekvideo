#ifndef _AV_SHM_H
#define _AV_SHM_H

#include <sys/types.h>
#include <sys/mman.h>
#include <dirent.h>
#include "util.h"
#include "log.h"

#define MAX_SHM_SIZE (AV_ALIGN(1024*1024*64))


struct av_shm {
    unsigned id;
    void *ptr;
    size_t size;
    int do_unlink:1;
    int shared:1;
};

int av_shm_create_rw(struct av_shm *m, size_t size, int shared, mode_t mode);
int av_shm_attach_ro(struct av_shm *m, unsigned id);

void av_shm_free(struct av_shm *m);

int av_shm_cleanup(void);

#endif /* _AV_SHM_H */
