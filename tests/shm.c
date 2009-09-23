#include "shm.h"

#define VAL 0xDEADBEEF

int main(int argc, char **argv)
{
	struct av_shm shm;
	struct av_shm shm_ro;
	unsigned int *p, *rp;
	
	av_shm_create_rw(&shm, MAX_SHM_SIZE - 1, 1, 0600);
	av_shm_free(&shm);
	p = shm.ptr;

	p[MAX_SHM_SIZE/2] = VAL;

	av_shm_attach_ro(&shm_ro, shm.id);
	rp = shm.ptr;

	if (rp[MAX_SHM_SIZE/2] != VAL)
		av_log_error("shm_ro reading failed\n");

	if (rp[0])
		av_log_error("shm_ro reading failed\n");

	return 0;

}
