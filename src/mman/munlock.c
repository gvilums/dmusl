#include <sys/mman.h>
#include "syscall.h"

int munlock(const void *addr, size_t len)
{
	return 0;
	// return syscall(SYS_munlock, addr, len);
}
