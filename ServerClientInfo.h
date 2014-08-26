/* 
 * File:   ServerClientInfo.h
 * Author: user
 *
 * Created on 19. August 2014, 10:40
 */


/*
 * eth0: 192.168.120.233
 * eth1: 192.168.220.235
 * 
 * iptables -t nat -A POSTROUTING -o eth1 -d 192.168.220.236 -s 192.168.220.235 -j SNAT --to-source 192.168.120.234

iptables -t nat -A PREROUTING -i eth0 -s 192.168.120.234 -d 192.168.220.236 -j DNAT --to-destination 192.168.120.233

iptables -t nat -A POSTROUTING -o eth0 -d 192.168.120.234 -s 192.168.120.233 -j SNAT --to-source 192.168.220.236

iptables -t nat -A PREROUTING -i eth1 -s 192.168.220.236 -d 192.168.120.234 -j DNAT --to-destination 192.168.220.235 
 */

#ifndef SERVERCLIENTINFO_H
#define	SERVERCLIENTINFO_H

#include <time.h>

#define SERVER_IP "192.168.120.233"


struct init_info_client_to_server {
    int paket_size;
};

struct init_info_server_to_client {
    int port;
};

struct paket_header {
//    int token;
    int train_id;
    int train_send_countid;
    int paket_id;
    int count_pakets_in_train;
    struct timespec recv_time;
    struct timespec send_time;
    int recv_data_rate; // Bytes per Sek
};

struct paket {
    struct paket_header header;
    char *puffer;
};

    /*
    * max. UMTS Datenrate: HSPA+ 42 MBits/Sek
    * 42 MBits/Sek = 42000000 Bits/Sek = 5250000 Bytes/Sek
    */
    static int max_umts_data_rate = 115250000; 

#endif	/* SERVERCLIENTINFO_H */

