#include <elf.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "syscall.h"
#include "atomic.h"
#include "libc.h"

static void dummy(void) {}
weak_alias(dummy, _init);

extern weak hidden void (*const __init_array_start)(void), (*const __init_array_end)(void);

static void dummy1(void *p) {}
weak_alias(dummy1, __init_ssp);

#define AUX_CNT 38

void __init_tls(size_t *auxv) {}
size_t __dummy_auxval = 0;
char* __dummy_argv = NULL;
char* __dummy_envp = NULL;

#ifdef __GNUC__
__attribute__((__noinline__))
#endif
void __init_libc(char **envp, char *pn)
{
	__environ = &__dummy_envp;
	libc.auxv = &__dummy_auxval;
	__hwcap = 0; // = aux[AT_HWCAP]
	// if (aux[AT_SYSINFO]) __sysinfo = aux[AT_SYSINFO];
	libc.page_size = 4096; // TODO: real page size

	if (!pn) pn = "";
	__progname = __progname_full = pn;

	for (size_t i=0; pn[i]; i++) if (pn[i]=='/') __progname = pn+i+1;

	__init_tls(NULL);
	__init_ssp(NULL); // TODO: entropy source

	return;
}

static void libc_start_init(void)
{
	_init();
	uintptr_t a = (uintptr_t)&__init_array_start;
	for (; a<(uintptr_t)&__init_array_end; a+=sizeof(void(*)()))
		(*(void (**)(void))a)();
}

weak_alias(libc_start_init, __libc_start_init);

typedef int lsm2_fn(int (*)(int,char **,char **), int, char **);
static lsm2_fn libc_start_main_stage2;

int __libc_start_main(int (*main)(int,char **,char **), int argc, char **argv,
	void (*init_dummy)(), void(*fini_dummy)(), void(*ldso_dummy)())
{
	// char **envp = argv+argc+1;

	/* External linkage, and explicit noinline attribute if available,
	 * are used to prevent the stack frame used during init from
	 * persisting for the entire process lifetime. */
	// __init_libc(envp, argv[0]);
	__init_libc(NULL, NULL);

	/* Barrier against hoisting application code or anything using ssp
	 * or thread pointer prior to its initialization above. */
	lsm2_fn *stage2 = libc_start_main_stage2;
	__asm__ ( "" : "+r"(stage2) : : "memory" );
	return stage2(main, argc, argv);
}

static int libc_start_main_stage2(int (*main)(int,char **,char **), int argc, char **argv)
{
	// char **envp = argv+argc+1;
	__libc_start_init();

	/* Pass control to the application */
	// exit(main(argc, argv, envp));
	exit(main(0, &__dummy_argv, &__dummy_envp));
	return 0;
}
