#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include "syscall.h"

static void dummy(void) { }
weak_alias(dummy, __vm_wait);

#define UNIT SYSCALL_MMAP2_UNIT
#define OFF_MASK ((-0x2000ULL << (8*sizeof(syscall_arg_t)-1)) | (UNIT-1))

void *__mmap_real(void *start, size_t len, int prot, int flags, int fd, off_t off)
{
	long ret;
	if (off & OFF_MASK) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (len >= PTRDIFF_MAX) {
		errno = ENOMEM;
		return MAP_FAILED;
	}
	if (flags & MAP_FIXED) {
		__vm_wait();
	}
#ifdef SYS_mmap2
	ret = __syscall(SYS_mmap2, start, len, prot, flags, fd, off/UNIT);
#else
	ret = __syscall(SYS_mmap, start, len, prot, flags, fd, off);
#endif
	/* Fixup incorrect EPERM from kernel. */
	if (ret == -EPERM && !start && (flags&MAP_ANON) && !(flags&MAP_FIXED))
		ret = -ENOMEM;
	return (void *)__syscall_ret(ret);
}

uintptr_t __mmap_base_addr = 0;
uintptr_t __mmap_end_addr = 0;

void *__mmap(void *start, size_t len, int prot, int flags, int fd, off_t off) {
	if (__mmap_base_addr == 0) {
		const size_t map_size = 1ull << 30;
		__mmap_base_addr = (uintptr_t)__mmap_real(NULL, map_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		__mmap_end_addr = __mmap_base_addr + map_size;
	}

	size_t aligned_len = ((len - 1) | (PAGE_SIZE - 1)) + 1;
	if (fd != -1) {
		errno = EBADF;
		return MAP_FAILED;
	}
	if (__mmap_base_addr + aligned_len < __mmap_base_addr || off != 0 || (prot & PROT_EXEC)) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	if (__mmap_base_addr + aligned_len > __mmap_end_addr) {
		errno = ENOMEM;
		return MAP_FAILED;
	}
	__mmap_base_addr += aligned_len;
	return (void *)(__mmap_base_addr - aligned_len);
}

weak_alias(__mmap, mmap);
