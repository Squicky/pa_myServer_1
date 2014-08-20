/* 
 * File:   ServerClientClass.cpp
 * Author: user
 * 
 * Created on 17. August 2014, 19:36
 */

#include "ServerClientClass.h"
#include "ServerClientInfo.h"

//#include <sys/socket.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

ServerClientClass::ServerClientClass(int _paket_size) {
    mess_paket_size = _paket_size;

    udp_rec_port = LOCAL_Mess_PORT;

    client_mess_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_mess_socket < 0) {
        printf("ERROR:\n  Kann UDP Mess-Socket (UMS) für Client nicht oeffnen: \n(%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        printf("UDP Mess-Socket (UMS) für Client  erstellt :-) \n");
    }

    // meineAddr & clientAddr mit "0" füllen
    memset((char *) &clientAddr, 0, sizeof (clientAddr));
    memset((char *) &meineAddr, 0, sizeof (meineAddr));

    // meineAddr konfigurieren: IPv4, Port, jeder Absender
    meineAddr.sin_family = AF_INET;
    meineAddr.sin_port = htons(udp_rec_port);
    meineAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    long rc;

    int i;
    for (i = 0; i < 1000; i++) {
        rc = bind(client_mess_socket, (struct sockaddr*) &meineAddr, sizeof (meineAddr));
        if (rc < 0) {
            printf("ERROR:\n  Port %d kann nicht an UDP Mess-Socket (UMS) gebunden werden:\n (%s)\n", udp_rec_port, strerror(errno));

            udp_rec_port++;
            meineAddr.sin_port = htons(udp_rec_port);
        } else {
            printf("Port %d an UDP Mess-Socket (UMS %d) gebunden :-) \n", udp_rec_port, udp_rec_port);

            i = udp_rec_port;
            break;
        }
    }

    // Wenn Port an Socket "bind" erfolgreich, dann Empfang (rec) Socket starten
    if (i == udp_rec_port) {
        int thread = pthread_create(&rec_thread, NULL, &rec_threadStart, this);

        if (thread != 0) {
            printf("ERROR:\n  Kann pthread nicht erstellen: \n(%s)\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    } else {
        printf("ERROR:\n  Es konnte kein Port an UDP Mess-Socket (UMS) gebunden werden \n");
        //        exit(EXIT_FAILURE);
    }

    clientAddrSize = sizeof (clientAddr);
}

ServerClientClass::ServerClientClass(const ServerClientClass& orig) {
}

ServerClientClass::~ServerClientClass() {
}

void * ServerClientClass::rec_threadStart(void * data) {

    ((ServerClientClass*) data)->rec_threadRun();
}

void ServerClientClass::rec_threadRun() {
    printf("Empfangs-pthread für UDP Mess-Socket (UMS) (%s:%d) gestartet \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));


    // Startwerte und init

    // Berechne Speichergröße für Array für Header eines ganzen Trains
    // bei max. UMTS Geschwindigkeit 
    // Da wir nur 500 ms lange senden wollen sollte das Array groß genug sein (fast doppelte Größe))
    int paket_header_size = sizeof (paket_header);
    int array_paket_header_size = max_umts_data_rate / mess_paket_size;
    array_paket_header_size = array_paket_header_size * paket_header_size;

    struct paket_header *array_paket_header_recv = (paket_header*) malloc(array_paket_header_size);
    struct paket_header *array_paket_header_send = (paket_header*) malloc(array_paket_header_size);

    int paket_size = sizeof (paket);
    struct paket *arbeits_paket = (struct paket *) malloc(paket_size);
    struct paket_header *arbeits_paket_header = &(arbeits_paket->header);

    if (array_paket_header_recv == NULL || array_paket_header_send == NULL || arbeits_paket == NULL) {
        printf("ERROR:\n  Kein virtueller RAM mehr verfügbar \n");
        printf("  array_paket_header_recv: %p \n", array_paket_header_recv);
        printf("  array_paket_header_send: %p \n", array_paket_header_send);
        printf("  mein_paket: %p \n", arbeits_paket);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // mein_paket->puffer mit dezimal "83" (binär "01010011") füllen :-)
    memset((char *) &(arbeits_paket->puffer), 83, sizeof (arbeits_paket->puffer));

    //    struct paket_header *mein_paket_header = (struct paket_header *) mein_paket;

    //    int paket_header_size = sizeof (paket_header);

    /*
        struct paket_header lokalesPaket;
        lokalesPaket.token = 0;
        lokalesPaket.train_id = 0;
        lokalesPaket.paket_id = 0;
        lokalesPaket.count_pakets_in_train = 0;
        lokalesPaket.recv_data_rate = 64000 / 8; // 64 kBit/Sek.

        lokalesPaket.count_pakets_in_train = lokalesPaket.recv_data_rate / mess_paket_size;
     */
    // Es soll nur 1/2 Sek gesendet werden
    int mess_paket_size_doppelt = 2 * mess_paket_size;
    //    lokalesPaket.count_pakets_in_train = lokalesPaket.recv_data_rate / mess_paket_size_doppelt;

    long countBytes;
    int i;

    int index_paket = 0;
    /* Daten in While Schleife empfangen */
    printf("UDP Mess-Socket (UMS) (%s:%d) wartet auf Daten ... \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));
    int last_paket_id = -1;
    while (1) {

        /*
        // rc = Anzahl empfangener Bytes
        rc = recvfrom(client_mess_socket, puffer, puffer_size, 0, (struct sockaddr *) &clientAddr, &clientAddrSize);
        puffer[rc] = 0;
        printf("UMS (%s:%d) hat %ld Bytes von Client (%s:%d) empfangen\n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("  Daten: |%s|\n", puffer);
         */

        countBytes = recvfrom(client_mess_socket, arbeits_paket, paket_size, 0, (struct sockaddr *) &clientAddr, &clientAddrSize);
        clock_gettime(CLOCK_REALTIME, &(arbeits_paket->header.recv_time));
        //        usleep(5000);
        if ((last_paket_id + 1) != arbeits_paket->header.paket_id) {
            printf("paket empfangen id: %d # ", arbeits_paket->header.paket_id);
            printf("index_paket: %d # ", index_paket);
            printf("count_pakets_in_train: %d # ", arbeits_paket->header.count_pakets_in_train);
            printf("countBytes: %ld # ", countBytes);
            printf("token: %d ", arbeits_paket->header.token);
            printf(" ############# \n");
        } else {
//            if ((index_paket % 100) == 0 || arbeits_paket_header->paket_id == (arbeits_paket_header->count_pakets_in_train - 1)) {
            if (arbeits_paket_header->paket_id == (arbeits_paket_header->count_pakets_in_train - 1)) {
                printf("paket empfangen id: %d # ", arbeits_paket->header.paket_id);
                printf("index_paket: %d # ", index_paket);
                printf("count_pakets_in_train: %d # ", arbeits_paket->header.count_pakets_in_train);
                printf("countBytes: %ld # ", countBytes);
                printf("token: %d ", arbeits_paket->header.token);
                printf(" \n");
            }
        }
        last_paket_id = arbeits_paket->header.paket_id;

        if (countBytes != mess_paket_size) {
            printf("ERROR:\n  %ld Bytes empfangen (%s)\n", countBytes, strerror(errno));
        }

        // Header des empfangenen Pakets in das Array sichern 
        memcpy(&(array_paket_header_recv[index_paket]), arbeits_paket_header, paket_header_size);

        // wenn leztes Paket vom Paket Train empfangen, dann Antwort Train senden
        if (arbeits_paket_header->paket_id == (arbeits_paket_header->count_pakets_in_train - 1)) {

            arbeits_paket_header->count_pakets_in_train = arbeits_paket_header->recv_data_rate / mess_paket_size_doppelt;

            if (52000 < arbeits_paket_header->count_pakets_in_train) {
                arbeits_paket_header->count_pakets_in_train = 52000;
            }

            // berechne neue Empfangsrate
            double time_diff = timespec_diff_double(array_paket_header_recv[0].recv_time, array_paket_header_recv[index_paket].recv_time);
            double count_all_bytes = index_paket * mess_paket_size;
            double bytes_per_sek = count_all_bytes / time_diff;
            arbeits_paket_header->recv_data_rate = bytes_per_sek;

            printf("Last index_paket: %d # ", index_paket);
            printf("count_all_bytes: %f # ", count_all_bytes);
            printf("time_diff: %f # ", time_diff);
            printf("my new data_rate: %f \n", bytes_per_sek);

            printf("sende %d Pakete # token: %d\n", arbeits_paket_header->count_pakets_in_train, arbeits_paket_header->token);
            for (i = 0; i < arbeits_paket_header->count_pakets_in_train; i++) {
                arbeits_paket_header->paket_id = i;
                clock_gettime(CLOCK_REALTIME, &(arbeits_paket_header->send_time));

                countBytes = sendto(client_mess_socket, arbeits_paket, mess_paket_size, 0, (struct sockaddr*) &clientAddr, clientAddrSize);

                usleep(10000);

                if (countBytes != mess_paket_size) {
                    printf("ERROR:\n  %ld Bytes gesendet (%s)\n", countBytes, strerror(errno));
                }
            }

            index_paket = -1;
            last_paket_id = -1;
        }

        /*
        rc = sendto(client_mess_socket, puffer, strlen(puffer), 0, (struct sockaddr*) &clientAddr, clientAddrSize);
        if (rc < 0) {
            printf("ERROR:\n  %ld Bytes gesendet (%s)\n", rc, strerror(errno));
        }
        printf("UMS (%s:%d) hat %ld Bytes an Client (%s:%d) gesendet\n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port), rc, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        printf("  Daten: |%s|\n", puffer);
         */

        index_paket++;
    }
}

/*
 * Berechnet aus zwei timespec de Zeitdifferenz
 * 
 * 1 Sek =         1.000 Millisekunden
 * 1 Sek =     1.000.000 Mikrosekunden 
 * 1 Sek = 1.000.000.000 Nanosekunden 
 */
timespec ServerClientClass::timespec_diff_timespec(timespec start, timespec end) {
    timespec temp;

    if (end.tv_nsec < start.tv_nsec) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

double ServerClientClass::timespec_diff_double(timespec start, timespec end) {
    timespec temp = timespec_diff_timespec(start, end);

    double temp2 = temp.tv_nsec;
    double temp3 = 1000000000;
    temp2 = temp2 / temp3;
    temp3 = temp.tv_sec;

    return (temp2 + temp3);
}