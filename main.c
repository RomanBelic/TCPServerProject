/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Romaaan
 *
 * Created on 8 décembre 2016, 17:03
 */

#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/unistd.h>
#include "process_utils.h"
#include "socket_utils.h"
#include "shared.h"
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include "mutils.h"
/*
 * 
 */

int main(int argc, char** argv) {
    struct process root = fork_process(-10);  
    if (!root.isRoot){  
        struct s_socket server = init_server_socket(server_port);
        if(server.sock_fd > 0){
           listen_for_connections(&server);
           shutdown(server.sock_fd, SHUT_RDWR);
           close(server.sock_fd);
        }
        exit(EXIT_SUCCESS);
    }
    else{        
        const char* cmd_exit = "STOP\0";
        char *msg = malloc(16 * sizeof(char));
        while(root.child_id > 0){
            listen_for_intput(msg);
            if (strcmp(cmd_exit, msg) == 0){
                root.child_id = -1;
                printf("server : stopping\n");
                kill(root.child_id, SIGCHLD);
            }
        }
        free(msg);
    }
    exit(EXIT_SUCCESS);;
}

