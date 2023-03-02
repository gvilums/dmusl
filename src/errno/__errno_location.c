#include <errno.h>
#include "pthread_impl.h"

int __errno_value;

int *__errno_location(void)
{
	return &__errno_value;
	// return &__pthread_self()->errno_val;
}

weak_alias(__errno_location, ___errno_location);
