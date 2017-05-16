/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   shared.h
 * Author: Romaaan
 *
 * Created on 29 janvier 2017, 18:30
 */

#ifndef SHARED_H
#define SHARED_H


#define CountElems(x) (sizeof(x) / sizeof((x)[0]))

#define emsg ("Error")

#define server_port (1000)

#define newline ("\r\n")

#define FIN ((char) 255)         //EOT end of transmission packet

#define ACK_FIN ((char) 254)     //acknowledged EOT packet

#define FIN_SHD ((char)253)      //instant EOT

#define PRGS ((char)252)         //in progress

#define maxConnections 32 //max clients in multiplex mode

#define maxMultiConnections 2 //max clients in multiprocess mode


#endif /* SHARED_H */

