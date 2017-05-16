/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/_default_fcntl.h>
#include "process_utils.h"
#include "socket_utils.h"
#include "mutils.h"
#include "shared.h"

struct s_socket init_server_socket(int port_number){
    
    struct s_socket servsocket;
    memset(&servsocket, 0, sizeof(servsocket));
    int error = 0;
    
    if ((servsocket.sock_fd = socket(AF_INET, SOCK_STREAM, 0)) > 0){
        servsocket.addr.sin_addr.s_addr = INADDR_ANY;
        servsocket.addr.sin_family = AF_INET;
        servsocket.addr.sin_port = htons(port_number);
        servsocket.s_size = sizeof(servsocket.addr);
        int sopt = 1;
        setsockopt(servsocket.sock_fd, SOL_SOCKET,SO_REUSEADDR, &sopt, sizeof(sopt));
        if (bind(servsocket.sock_fd, (struct sockaddr *)&servsocket.addr, sizeof(servsocket.addr)) == 0) {  
            if (listen(servsocket.sock_fd, 8) != 0){
                error = 1;
            }
        }  
        else {
            error = 1;
        }
    }
    else {
        error = 1;
    }
    if (error > 0) {
        shutdown(servsocket.sock_fd, SHUT_RDWR);
        close(servsocket.sock_fd);
        servsocket.sock_fd = -1;
        perror(emsg);
    }
    return servsocket;
}

char *getsockipv4(struct s_socket *client)
{
    char *sockip = calloc (sizeof(client->addr), sizeof(char));
    inet_ntop(client->addr.sin_family, &(client->addr.sin_addr),sockip, sizeof(client->addr));
    return sockip;
}

int communicate_multiprocess_mode(int cl_fd){
    char buff[256];
    char cmd_pck = PRGS;
    int bytesRead;
    while ((bytesRead = recv(cl_fd, &buff, sizeof(buff), 0)) > 0) {   
        memccpy(&cmd_pck, &buff, 1, sizeof(char));  
        if (cmd_pck == FIN){
            cmd_pck = ACK_FIN;
            send(cl_fd, &cmd_pck, 1, 0); //send echo
        }    
        else if (cmd_pck == ACK_FIN){
            shutdown(cl_fd, SHUT_WR);
            break;
        } 
        else if (cmd_pck == FIN_SHD){
            shutdown(cl_fd, SHUT_RDWR);
            break;
        }
        else {
            FILE *f = fopen("log.txt", "a"); 
            cmd_pck = PRGS;
            fwrite(&buff, sizeof(char), bytesRead, f);        
            fflush(f);
            fclose(f);
            send(cl_fd, &buff, bytesRead, 0); //send echo
        }
    }
    return (int)cmd_pck;
}

int communicate_multiplex_mode(int cl_fd, struct client_stack *cl_stack, fd_set *socket_set){
    char buff[256];
    char cmd_pck = PRGS;
    int bytesRead;
    if ((bytesRead = recv(cl_fd, &buff, sizeof(buff), 0)) > 0) {   
        memccpy(&cmd_pck, &buff, 1, sizeof(char));  
        if (cmd_pck == FIN){
            cmd_pck = ACK_FIN;
            send(cl_fd, &cmd_pck, 1, 0); //send echo
        }    
        else if (cmd_pck == ACK_FIN){
            shutdown(cl_fd, SHUT_WR);
            close(cl_fd);
            FD_CLR(cl_fd, socket_set);
            delete_from_clarray(cl_fd, cl_stack);
        } 
        else if (cmd_pck == FIN_SHD){
            shutdown(cl_fd, SHUT_RDWR);
            close(cl_fd);
            FD_CLR(cl_fd, socket_set);
            delete_from_clarray(cl_fd, cl_stack);
        }
        else {
            FILE *f = fopen("log.txt", "a"); 
            cmd_pck = PRGS;
            fwrite(&buff, sizeof(char), bytesRead, f);        
            fflush(f);
            fclose(f);
            send(cl_fd, &buff, bytesRead, 0); //send echo
        }
    }
    return (int)cmd_pck;
}

int listen_for_connections (struct s_socket *server){

    volatile int isListening = 1;
    fd_set socket_set;
    fd_set mutator_set;
    FD_ZERO(&socket_set);
    FD_ZERO(&mutator_set);
    FD_SET(server->sock_fd, &socket_set);

    struct client_stack cl_stack = init_cl_stack(maxConnections);   
    add_to_clarray(server->sock_fd, &cl_stack);
    
    int *act_process = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);  //mapping this memblock to kernel to share between processes
    *act_process = 0;
        
    while (isListening){    
        if (*act_process < maxMultiConnections){
            listen_multiprocess_mode(server, act_process);
        }else {
            memcpy(&mutator_set, &socket_set, sizeof(socket_set)); 
            listen_multiplex_mode(server, &cl_stack, &socket_set, &mutator_set);
        }
    }
    munmap(act_process, sizeof(int));
    return 0; 
}

int listen_multiplex_mode(struct s_socket *server, struct client_stack* cl_stack, fd_set *socket_set, fd_set *mutator_set){
    if (select(cl_stack->max_fd + 1, mutator_set, NULL, NULL, NULL) > 0){
        if (FD_ISSET(server->sock_fd, mutator_set)){
            struct s_socket client = accept_client_socket(server);
            set_socket_mode(client.sock_fd, O_NONBLOCK);
            add_to_clarray(client.sock_fd, cl_stack);
            FD_SET(client.sock_fd, socket_set);
            communicate_multiplex_mode(client.sock_fd, cl_stack, socket_set);
        }
        else {
            for (int i = 1; i < cl_stack->cl_limit; i++){ 
                int clfd;
                if ((clfd = cl_stack->cl_array[i]) > 0){
                    if (FD_ISSET(clfd, mutator_set)){
                        communicate_multiplex_mode(clfd, cl_stack, socket_set); //pas bon, car il faut un array des structures client *s_socket
                    }
                }
            }
        }
    }
    return 0;
}

int delete_from_clarray(int cl_fd, struct client_stack *clarray){
    int result = 0;
    for (int i = 0; i < clarray->cl_limit; i++){   
        if (clarray->cl_array[i] == cl_fd){   
            clarray->cl_array[i] = -1;
            if (clarray->max_fd == cl_fd){
                clarray->max_fd = find_maxnb(clarray->cl_array);
            }
            clarray->actcl_count --;
            result = 1;
            break;
        }
    }
    return result;
}

int add_to_clarray(int newcl, struct client_stack *clarray){
    int result = 0;
    for (int i = 0; i < clarray->cl_limit; i++){   
        if (clarray->cl_array[i] == -1){   
            clarray->cl_array[i] = newcl;
            clarray->max_fd = newcl;
            clarray->actcl_count ++;
            result = 1;
            break;
        }
    }
    return result;  
}

struct client_stack init_cl_stack(int nclients){
    struct client_stack cl_stack;
    int *cl_array = malloc(sizeof(int) * nclients);
    memset(cl_array, -1, sizeof(int) * nclients);
    cl_stack.cl_array = cl_array;
    cl_stack.cl_limit = nclients;
    cl_stack.max_fd = -1;
    cl_stack.actcl_count = 0;
    return cl_stack;
}

struct s_socket accept_client_socket(struct s_socket *server){
    struct s_socket client;
    memset(&client, 0, sizeof(client));
    int s_size = server->s_size;
    int s_fd = accept(server->sock_fd, (struct sockaddr *)&client.addr, &s_size);
    client.sock_fd = s_fd;
    client.s_size = s_size;
    return client;
}

int listen_multiprocess_mode(struct s_socket *server, int* act_process){
    
    struct s_socket client = accept_client_socket(server);
    if (client.sock_fd > 0){
        struct process comproc = fork_process(-10);
        if (!comproc.isRoot){
            (*act_process)++;
            communicate_multiprocess_mode(client.sock_fd);
            shutdown(client.sock_fd, SHUT_RDWR);
            close(client.sock_fd);
            (*act_process)--;
            exit(EXIT_SUCCESS);
        }
        else {
            close(client.sock_fd); //Closing fd on parent immediately
        }
    }
    else {
        perror(emsg);
    }
    return 0;
}

int set_socket_mode(int s_fd, int mode){
    int flags = fcntl(s_fd, F_GETFL, 0);
    return fcntl(s_fd, F_SETFL, flags | mode);
}