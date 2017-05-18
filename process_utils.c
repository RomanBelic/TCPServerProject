/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "process_utils.h"
#include <sys/unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <limits.h>
#include "shared.h"

struct process fork_process (int priority)
{
    struct process root;
    memset(&root, 0, sizeof(root));
    pid_t cur_id = getpid();
    signal(SIGCHLD, SIG_IGN);  //ignoring zombies
    pid_t child_id = fork();

    if (child_id == 0) //inside child
    {
        root.id = getpid();
        root.parent_id = cur_id;
        root.isRoot = false;
        root.child_id = 0;
        setpriority(PRIO_PROCESS, root.id, priority);
       
    } 
    else if (child_id > 0)  //inside parent
    {
        root.id = cur_id;
        root.isRoot = true;
        root.parent_id = 0; 
        root.child_id = 1;
    }
    else 
    {
        root.id = -1;
        root.parent_id -1;
        root.child_id = -1;
        root.isRoot = false;
        perror(EMSG);
    } 
    
    return root;
}

int exec_proc(struct process *proc, bool isParent, pid_t id, int (*func)(void *args), void* fargs)
{
    if (proc->isRoot == isParent && proc->id == id){
        return func(fargs);
    }
    return -1;
}

struct pipeline init_pipeline (){
    struct pipeline mypipe = {-1,-1};
    int arr[2];
    if (pipe(arr) == 0){
        mypipe.in_desc = arr[PIPE_IN];
        mypipe.out_desc = arr[PIPE_OUT];
    }
    else{
        mypipe.in_desc = -1;
        mypipe.out_desc = -1;
    } 
    return mypipe;
}


