#include <sys/mman.h>
#include "syscall.h"

static void dummy(void) { }
weak_alias(dummy, __vm_wait);

int __munmap_real(void *start, size_t len)
{
	__vm_wait();
	return syscall(SYS_munmap, start, len);
}

int __munmap(void *start, size_t len) {
	return 0;
}

weak_alias(__munmap, munmap);
