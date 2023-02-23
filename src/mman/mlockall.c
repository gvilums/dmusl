#include <sys/mman.h>
#include "syscall.h"

int mlockall(int flags)
{
	return 0;
	// return syscall(SYS_mlockall, flags);
}
