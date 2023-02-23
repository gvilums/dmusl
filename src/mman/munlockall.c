#include <sys/mman.h>
#include "syscall.h"

int munlockall(void)
{
	return 0;
	// return syscall(SYS_munlockall);
}
