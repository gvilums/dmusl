#define _GNU_SOURCE
#include <sys/mman.h>
#include "syscall.h"

int mincore (void *addr, size_t len, unsigned char *vec)
{
	// TODO: write dummy value to all elements in vec
	return 0;
	// return syscall(SYS_mincore, addr, len, vec);
}
