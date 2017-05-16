#ifndef UTILS_H
#define UTILS_H

#include "process_structs.c"

#define PIPE_IN  0
#define PIPE_OUT 1

struct process fork_process (int priority);
int exec_proc(struct process *proc, bool isParent, pid_t id, int (*func)(void *args), void* fargs);
struct pipeline init_pipeline ();

#endif /* UTILS_H */

