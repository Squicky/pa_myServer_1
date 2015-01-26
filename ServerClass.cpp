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
#include <unistd.h>

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
        printf(" Control UDP Socket (CUS) erstellt :-) \n");
    }

    // meineAddr & remote_Server_Addr mit "0" füllen
    memset((char *) &meineAddr, 0, sizeof (meineAddr));
    memset((char *) &clientAddr, 0, sizeof (clientAddr));

    // meineAddr konfigurieren: IPv4, Port, jeder Absender
    meineAddr.sin_family = AF_INET;
    meineAddr.sin_port = htons(LOCAL_Control_SERVER_PORT);

    char SERVER_IP_strongrom[] = "134.99.147.228";
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    if (0 == strcmp(hostname, "strongrom")) {
        meineAddr.sin_addr.s_addr = inet_addr(SERVER_IP_strongrom);
    }
    else {
        meineAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    long rc;

    /* Lokalen Server Port bind(en) */
    rc = bind(control_socket, (struct sockaddr *) &meineAddr, sizeof (meineAddr));
    if (rc < 0) {
        printf("ERROR:\n  IP %s und Port %d kann nicht an Control UDP Socket (CUS) gebunden werden:\n (%s)\n", inet_ntoa(meineAddr.sin_addr), LOCAL_Control_SERVER_PORT, strerror(errno));
        fflush(stdout);
        exit(EXIT_FAILURE);
    } else {
        printf(" IP %s und Port %d an Control UDP Socket (CUS) gebunden :-) \n", inet_ntoa(meineAddr.sin_addr), LOCAL_Control_SERVER_PORT);
    }

    struct init_info_client_to_server info_c2s;
    struct init_info_server_to_client info_s2c;

    /* Daten in While Schleife empfangen */
    printf(" Control UDP Socket (CUS) (%s:%d) wartet auf Daten ... \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));
    while (1) {

        // rc = Anzahl empfangener Bytes
        rc = recvfrom(control_socket, &info_c2s, sizeof (info_c2s), 0, (struct sockaddr *) &clientAddr, &clientAddrSize);
        //        cus_puffer[rc] = 0;
        printf("CUS (%s:%d) hat %ld Bytes ", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc);
        printf("von Client (%s:%d) empfangen\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        //        printf("  Daten: |%s|\n", cus_puffer);

        info_s2c.client_ip_ok = true;

        for (std::list<ServerClientClass *>::iterator it = ServerClientList.begin(); it != ServerClientList.end(); *it++) {
            if ((*it)->otherAddr.sin_addr.s_addr == clientAddr.sin_addr.s_addr) {
                if ((*it)->stop == false) {
                    (*it)->stop = true;

                    for (int i = 0; i < 10; i++) {
                        if ((*it)->in_while_schleife == true) {
                            sleep(1);
                        }
                    }
                }

                if ((*it)->in_while_schleife == false) {
                    info_s2c.client_ip_ok = true;
                } else {
                    info_s2c.client_ip_ok = false;
                }

                struct timespec jetzt;
                clock_gettime(CLOCK_REALTIME, &jetzt);

                double dif = ServerClientClass::timespec_diff_double(&(*it)->end_time, &jetzt);
                if (dif < 5) {
                    sleep(5);
                }
            }
        }

        if (rc == sizeof (info_c2s)) {

            ServerClientClass *sc = NULL;
            if (info_s2c.client_ip_ok == true) {
                sc = new ServerClientClass(info_c2s.paket_size, info_c2s.zeit_dateiname);
                ServerClientList.push_back(sc);

                if (sc->udp_rec_port != 0) {
                    // Gib dem Empfangs-pthread 120 Sekunde Zeit
                    int i = 0;
                    while (sc->in_while_schleife == false && i < 120) {
                        sleep(1);
                        i++;
                    }
                }

                info_s2c.port = sc->udp_rec_port;

                info_s2c.log_files_ok = sc->log_files_ok;
            } else {
                info_s2c.port = 0;
                info_s2c.log_files_ok = false;

                printf("ERROR:\n  Server Error: client_ip_ok: %s:%d \n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
                fflush(stdout);

            }

            rc = sendto(control_socket, &info_s2c, sizeof (info_s2c), 0, (struct sockaddr*) &clientAddr, clientAddrSize);

            if (rc < 0) {
                printf("ERROR:\n  %ld Bytes gesendet (%s)\n", rc, strerror(errno));
            }
            printf("CUS (%s:%d) hat %ld Bytes an ", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc);
            printf("Client (%s:%d) gesendet\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            if (sc != NULL) {
                if (sc->udp_rec_port == 0) {
                    printf("ERROR:\n  Es konnte kein Port an UDP Mess-Socket (UMS) gebunden werden\n");
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                } else if (sc->log_files_ok == false) {
                    printf("ERROR:\n  Log Files sind nicht OK\n");
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }
            }

        } else {
            sleep(1);
        }

    }
}

