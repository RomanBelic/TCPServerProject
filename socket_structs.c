/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <netinet/in.h>

#ifndef SOCKET_STRUCT_H
#define SOCKET_STRUCT_H

struct s_socket {
    struct sockaddr_in addr;
    int sock_fd;
    int s_size;
};

struct client_stack {
    int max_fd;
    int cl_limit;
    int actcl_count;
    int *cl_array;
};

#endif