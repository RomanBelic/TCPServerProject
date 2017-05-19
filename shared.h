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

#define EMSG ("Error")

#define SERVER_PORT (1000)

#define NEWLINE ("\r\n")

#define MaxSCons 32 //max clients in multiplex mode

#define MaxMCons 2 //max clients in multiprocess mode

#define INCOMING ((char) 251)

#define ACK ((char) 250)

#define FIN ((char) 252)

#define FIN_SHD ((char)253)      //instant EOT

#define ACK_INCOMING ((char) 254)

#define ACK_FIN ((char) 255)

#define PING ((char) 249)

#endif /* SHARED_H */

