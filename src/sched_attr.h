#ifndef __SCHED_ATTR_H__
#define __SCHED_ATTR_H__

#include <sys/syscall.h>

struct sched_attr {
    __u32 size;              /* Size of this structure */
    __u32 sched_policy;      /* Policy (SCHED_*) */
    __u64 sched_flags;       /* Flags */
    __s32 sched_nice;        /* Nice value (SCHED_OTHER,
                              SCHED_BATCH) */
    __u32 sched_priority;    /* Static priority (SCHED_FIFO,
                              SCHED_RR) */
    /* Remaining fields are for SCHED_DEADLINE */
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

int sched_setattr(pid_t pid,
                  const struct sched_attr *attr,
                  unsigned int flags)
{
    return syscall(SYS_sched_setattr, pid, attr, flags);
}

#endif /* __SCHED_ATTR_H__ */
