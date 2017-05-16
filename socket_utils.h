#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "socket_structs.c"

struct s_socket init_server_socket(int port_number);
char *getsockipv4(struct s_socket *client);
int communicate_multiprocess_mode(int cl_fd);
int communicate_multiplex_mode(int cl_fd, struct client_stack *cl_stack, fd_set *socket_set);
int listen_for_connections (struct s_socket *server);
int add_to_clarray(int newcl, struct client_stack *clarray);
struct client_stack init_cl_stack(int nclients);
int listen_multiplex_mode(struct s_socket *server, struct client_stack* clarray, fd_set *socket_set, fd_set *mutator_set);
struct s_socket accept_client_socket (struct s_socket *server);
int listen_multiprocess_mode(struct s_socket *server, int* act_process);
int delete_from_clarray(int cl_fd, struct client_stack *clarray);
int set_socket_mode(int s_fd, int mode);
    
#endif /* SOCKET_UTILS_H */

