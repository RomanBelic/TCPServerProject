/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <sys/types.h>

typedef enum {true = 1, false =! true} bool;

struct process 
{
    pid_t id;
    pid_t parent_id;
    pid_t child_id;
    bool isRoot;
};

struct pipeline {
    int in_desc;
    int out_desc;
};




