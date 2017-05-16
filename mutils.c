/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include "shared.h"
#include <sys/socket.h>
#include <string.h>

int getlocaltime(char *tpointer){
    
    struct tm *timeinfo = calloc(1, sizeof(struct tm));
    time_t rawtime;
    time (&rawtime); 
    timeinfo = localtime (&rawtime);
    tpointer = asctime (timeinfo);
    free(timeinfo);
    return 1;
}

int find_maxnb(int* array){
    int maxnb = array[0];
    for (int i = 0; i < CountElems(array); i++){
        if (maxnb < array[i]){
            maxnb = array[i];
        } 
    }
    return maxnb;
}

int close_fdst(int * fds_arr){
    int res = 0;
    for(int i = 0; i  < CountElems(fds_arr); i++){
        int fd;
        if ((fd = fds_arr[i])> 0){
            shutdown(fd, SHUT_RDWR);
            if (close(fd) == 0)
                res ++;
        }
    } 
    return res;
}

int listen_for_intput(char *output){
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(STDIN_FILENO, &fd);
    char buffer[32];
    int nbRead;
    if (select(STDIN_FILENO + 1, &fd, NULL, NULL, NULL) > 0){
        if(FD_ISSET(STDIN_FILENO, &fd)){
            if((nbRead = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0){
                output = realloc(output, sizeof(char) * (nbRead-1));
                memset(output, '\0', sizeof(output));
                memcpy(output, buffer, nbRead -1);
            }
        }
    }
    return nbRead;
}