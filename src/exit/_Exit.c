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
	__asm__ volatile(
		"ldr c0, [%0] \n"
		"ldpbr c29, [c0] \n"
		: : "r" (&__returnPair) : "c0", "c29"
	);

	// The below leads to miscompilations (or other weird behavior)
	/*
	__asm__ volatile(
		"ldpbr c29, %0 \n"
		: : "m" (__returnPair) : "c29"
	);
	*/
	__builtin_unreachable();

}
