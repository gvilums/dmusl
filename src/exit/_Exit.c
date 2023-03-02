#include <stddef.h>
#include <stdlib.h>
#include "syscall.h"

typedef struct{
  size_t size;
  void* address;
} ioStruct;

static ioStruct* const inputRoot;
static ioStruct* outputRoot;
static const unsigned int inputNumber;
static unsigned int outputNumber = 0;
static const unsigned int maxOutputNumber;

static const void * __capability const returnPair;


_Noreturn void _Exit(int ec)
{
	// __syscall(SYS_exit_group, ec);
	// for (;;) __syscall(SYS_exit, ec);
	__asm__ volatile(
    // "ldr c0, %w[] \n"
		"ldpbr c29, [%w[returnPair]] \n"
		: : [returnPair] "r" (returnPair) : "c0", "c29"
	);
	__builtin_unreachable();
}
