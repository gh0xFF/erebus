#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int lock_process_memory(void);

#if defined(__linux__)
    #include <sys/mman.h>
    #include <sys/resource.h>
    #include <linux/seccomp>
#elif defined(__APPLE__)
#endif


int lock_process_memory(void) {
    #if defined(__linux__)
        scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mmap), 0);
        seccomp_load(ctx);

        struct rlimit memlock_rlimit = {
            .rlim_cur = RLIM_INFINITY,
            .rlim_max = RLIM_INFINITY
        };
        if (setrlimit(RLIMIT_MEMLOCK, &memlock_rlimit) == -1) {
            fprintf(stderr, "setrlimit(RLIMIT_MEMLOCK) failed\n");
            return -1;
        }

        if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
            fprintf(stderr, "mlockall failed\n");
            return -1;
        }
    #elif defined(__APPLE__)
        #pragma message "must be runned on sandbox from side of OS since MacOS 10"

        /*
            since mac os 10 sandbox must me created from os side(not app)
            todo generate .sb file and run like
            system("sandbox-exec -f my_profile.sb ./my_program");
        */
    #else
        #pragma message "I fucked this fucking Windows in the mouth\ninstall a normal OS"
    #endif

    return 0;
}
