/* 
 * File:   ServerClass.cpp
 * Author: user
 * 
 * Created on 17. August 2014, 13:14
 */

#include "ServerClass.h"
#include "ServerClientInfo.h"

#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <arpa/inet.h>
#include <pthread.h>
#include <list>
#include <unistd.h>
#include <time.h>

ServerClass::ServerClass() {
    /*
        int thread = pthread_create(&CUS_thread, NULL, &threadStart, this);
    
        if (thread != 0) {
            printf("ERROR:\n  Kann pthread nicht erstellen: \n(%s)\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
     */
    threadRun();
}

ServerClass::ServerClass(const ServerClass& orig) {
}

ServerClass::~ServerClass() {
}

void * ServerClass::threadStart(void * data) {
    ((ServerClass*) data)->threadRun();
}

void ServerClass::threadRun() {

    int control_socket;
    struct sockaddr_in meineAddr, clientAddr;
    socklen_t clientAddrSize = sizeof (clientAddr);

    /* socket_erzeugen */
    control_socket = socket(AF_INET, SOCK_DGRAM, 0); //  SOCK_DGRAM <-> UDP
    if (control_socket < 0) {
        printf("ERROR:\n  Kann Socket nicht oeffnen: \n(%s)\n", strerror(errno));
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else {
        printf("Control UDP Socket (CUS) erstellt :-) \n");
    }

    // meineAddr & remote_Server_Addr mit "0" füllen
    memset((char *) &meineAddr, 0, sizeof (meineAddr));
    memset((char *) &clientAddr, 0, sizeof (clientAddr));

    // meineAddr konfigurieren: IPv4, Port, jeder Absender
    meineAddr.sin_family = AF_INET;
    meineAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    meineAddr.sin_port = htons(LOCAL_Control_SERVER_PORT);

    long rc;

    /* Lokalen Server Port bind(en) */
    rc = bind(control_socket, (struct sockaddr *) &meineAddr, sizeof (meineAddr));
    if (rc < 0) {
        printf("ERROR:\n  Port %d kann nicht an Control UDP Socket (CUS) gebunden werden:\n (%s)\n", LOCAL_Control_SERVER_PORT, strerror(errno));
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else {
        printf("Port %d an Control UDP Socket (CUS) gebunden :-) \n", LOCAL_Control_SERVER_PORT);
    }

    struct init_info_client_to_server info_c2s;
    struct init_info_server_to_client info_s2c;

    /* Daten in While Schleife empfangen */
    printf("Control UDP Socket (CUS) (%s:%d) wartet auf Daten ... \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));
    while (1) {

        // rc = Anzahl empfangener Bytes
        rc = recvfrom(control_socket, &info_c2s, sizeof (info_c2s), 0, (struct sockaddr *) &clientAddr, &clientAddrSize);
        //        cus_puffer[rc] = 0;
        printf("CUS (%s:%d) hat %ld Bytes von Client (%s:%d) empfangen\n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        //        printf("  Daten: |%s|\n", cus_puffer);

        if (rc == sizeof (info_c2s)) {

            ServerClientClass *sc = new ServerClientClass(info_c2s.paket_size);
            ServerClientList.push_back(sc);

            if (sc->udp_rec_port != 0) {
                // Gib dem Empfangs-pthread 1/10 Sekunde Zeit
                usleep(100000);
            }

            info_s2c.port = sc->udp_rec_port;

            rc = sendto(control_socket, &info_s2c, sizeof (info_s2c), 0, (struct sockaddr*) &clientAddr, clientAddrSize);
            //            cus_puffer[rc] = 0;
            if (rc < 0) {
                printf("ERROR:\n  %ld Bytes gesendet (%s)\n", rc, strerror(errno));
            }
            printf("CUS (%s:%d) hat %ld Bytes an Client (%s:%d) gesendet\n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            //          printf("  Daten: |%s|\n", cus_puffer);

            if (sc->udp_rec_port == 0) {
                printf("ERROR:\n  Es konnte kein Port an UDP Mess-Socket (UMS) gebunden werden");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }

    }
}

