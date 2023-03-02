#include <stddef.h>
#include <stdlib.h>
#include "syscall.h"

typedef struct{
  size_t size;
  void* address;
} ioStruct;

ioStruct* const __inputRoot;
ioStruct* __outputRoot;
const unsigned int __inputNumber;
unsigned int __outputNumber = 0;
const unsigned int __maxOutputNumber;

const void * __capability const __returnPair;


_Noreturn void _Exit(int ec)
{
	// __syscall(SYS_exit_group, ec);
	// for (;;) __syscall(SYS_exit, ec);
	// __asm__ volatile(
	// 	"ldr c0, %w[returnPair] \n"
	// 	"ldpbr c29, [c0] \n"
	// 	: : [returnPair] "r" (&__returnPair) : "c0", "c29"
	// );
	__asm__ volatile(
		// "ldr c0, %w[] \n"
		"ldpbr c29, [%w[returnPair]] \n"
		: : [returnPair] "m" (__returnPair) : "c0", "c29"
	);
	__builtin_unreachable();
}
