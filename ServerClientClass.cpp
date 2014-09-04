/* 
 * File:   ServerClientClass.cpp
 * Author: user
 * 
 * Created on 17. August 2014, 19:36
 */

#include "ServerClientClass.h"
#include "ServerClientInfo.h"
#include "ListArrayClass.h"

//#include <sys/socket.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

ServerClientClass::ServerClientClass(int _paket_size, char _zeit_dateiname[]) {
    stop = false;
    mess_paket_size = _paket_size;

    memcpy(zeit_dateiname, _zeit_dateiname, sizeof (zeit_dateiname));

    udp_rec_port = LOCAL_Mess_PORT;

    client_mess_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_mess_socket < 0) {
        printf("ERROR:\n  Kann UDP Mess-Socket (UMS) für Client nicht oeffnen: \n(%s)\n", strerror(errno));
        fflush(stdout);
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

    //    meineAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (6 < strlen(SERVER_IP) && strlen(SERVER_IP) < 16) {
        meineAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    } else {
        meineAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    long rc;

    int i;
    for (i = 0; i < 1000; i++) {
        rc = bind(client_mess_socket, (struct sockaddr*) &meineAddr, sizeof (meineAddr));
        if (rc < 0) {
            printf("ERROR:\n  IP %s und Port %d kann nicht an UDP Mess-Socket (UMS) gebunden werden:\n (%s)\n", inet_ntoa(meineAddr.sin_addr), udp_rec_port, strerror(errno));

            udp_rec_port++;
            meineAddr.sin_port = htons(udp_rec_port);
        } else {
            printf("IP %s und Port %d an UDP Mess-Socket (UMS %d) gebunden :-) \n", inet_ntoa(meineAddr.sin_addr), udp_rec_port, udp_rec_port);

            i = udp_rec_port;
            break;
        }
    }

    if (rc < 0) {
        printf("ERROR:\n  Es konnte keine IP und Port an UDP Mess-Socket (UMS) gebunden werden \n");
        udp_rec_port = 0;
        fflush(stdout);
    } else {
        int thread = pthread_create(&rec_thread, NULL, &rec_threadStart, this);

        if (thread != 0) {
            printf("ERROR:\n  Kann pthread nicht erstellen: \n(%s)\n", strerror(errno));
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
    }

    clientAddrSize = sizeof (clientAddr);
}

/*
ServerClientClass::ServerClientClass(const ServerClientClass& orig) {
}

ServerClientClass::~ServerClientClass() {
}
 */

void * ServerClientClass::rec_threadStart(void * data) {
    ((ServerClientClass*) data)->rec_threadRun();
}

void ServerClientClass::rec_threadRun() {
    printf("Empfangs-pthread für UDP Mess-Socket (UMS) (%s:%d) gestartet \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));


    // Startwerte und init

    // Berechne Speichergröße für Array für Header eines ganzen Trains
    // bei max. UMTS Geschwindigkeit 
    // Da wir nur 500 ms lange senden wollen sollte das Array groß genug sein (fast doppelte Größe))
    //    uint count_array_paket_header = MAX_UMTS_DATA_RATE / mess_paket_size;
    //    uint last_index_in_array_paket_header = count_array_paket_header - 1;
    //    uint paket_header_size = sizeof (paket_header);
    //    uint array_paket_header_size = count_array_paket_header * paket_header_size;

    //    struct paket_header *array_paket_header_recv = (paket_header*) malloc(array_paket_header_size);
    //    struct paket_header *array_paket_header_send = (paket_header*) malloc(array_paket_header_size);

    char puffer[256];
    sprintf(puffer, "%s_sv_recv", this->zeit_dateiname);
    ListArrayClass *lac_recv = new ListArrayClass(mess_paket_size, puffer);

    sprintf(puffer, "%s_sv_send", this->zeit_dateiname);

    ListArrayClass *lac_send1 = new ListArrayClass(mess_paket_size, puffer);
    ListArrayClass *lac_send2 = new ListArrayClass(mess_paket_size);
    lac_send2->File_Deskriptor = lac_send1->File_Deskriptor;
    ListArrayClass *lac_send3 = lac_send1;

    //    uint array_paket_header_recv_start = (uint) array_paket_header_recv;
    //    uint array_paket_header_recv_ende = array_paket_header_recv_start + array_paket_header_size - 1;

    int paket_header_size = sizeof (paket_header);
    int paket_size = paket_header_size + mess_paket_size;
    struct paket *arbeits_paket_recv = (struct paket *) malloc(paket_size);
    struct paket_header *arbeits_paket_header_recv = &(arbeits_paket_recv->header);
    struct paket *arbeits_paket_send = (struct paket *) malloc(paket_size);
    struct paket_header *arbeits_paket_header_send = &(arbeits_paket_send->header);

    if (arbeits_paket_recv == NULL || arbeits_paket_send == NULL) {
        printf("ERROR:\n  Kein virtueller RAM mehr verfügbar \n");
        printf("  arbeits_paket_recv: %p \n", arbeits_paket_recv);
        printf("  arbeits_paket_send: %p \n", arbeits_paket_send);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // puffer mit dezimal "83" (binär "01010011") füllen :-)
    memset((char *) &(arbeits_paket_send->puffer), 83, mess_paket_size);

    // Es soll nur 1/2 Sek gesendet werden
    int mess_paket_size_doppelt = 2 * mess_paket_size;
    //    lokalesPaket.count_pakets_in_train = lokalesPaket.recv_data_rate / mess_paket_size_doppelt;


    // Timeout fuer recvfrom auf 1 Sek setzen     
    struct timeval timeout_time;
    timeout_time.tv_sec = 0; // Anzahl Sekunden
    timeout_time.tv_usec = 100000; // Anzahl Mikrosekunden : 1 Sek. = 1.000.000 Mikrosekunden

    long countBytes;
    int i;

    int my_last_send_train_id = -1;
    int my_recv_train_send_countid = -1;
    int my_send_train_send_countid = -1;
    int my_max_recv_train_id = -1;
    int my_max_send_train_id = -3;
    int my_max_train_id = -2;
    int my_bytes_per_sek = 64;

    int set_timeout = 0;

    /* Daten in While Schleife empfangen */
    printf("UDP Mess-Socket (UMS) (%s:%d) wartet auf Daten ... \n", inet_ntoa(meineAddr.sin_addr), ntohs(meineAddr.sin_port));
    while (stop == false) {

        timespec a, b;

        clock_gettime(CLOCK_REALTIME, &a);

        countBytes = recvfrom(client_mess_socket, arbeits_paket_recv, paket_size, 0, (struct sockaddr *) &clientAddr, &clientAddrSize);

        clock_gettime(CLOCK_REALTIME, &(arbeits_paket_header_recv->recv_time));

        if (countBytes == -1) {
            b = timespec_diff_timespec(a, arbeits_paket_header_recv->recv_time);

            if (b.tv_nsec < (timeout_time.tv_usec * 1000)) {
                b.tv_sec++;
            }
        }


        if (set_timeout == 0) {
            set_timeout = 1;
            if (setsockopt(client_mess_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout_time, sizeof (timeout_time))) {
                printf("ERROR:\n  Kann Timeout fuer UDP Mess-Socket (UMS) nicht setzen: \n(%s)\n", strerror(errno));
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        }

        if (countBytes == -1) {
            printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
            printf("\033[11;0H# ");
            printf("Timeout recvfrom:  %ld Bytes empfangen (%s)       ", countBytes, strerror(errno));
            printf("\033[19;0H");
            fflush(stdout);
        } else if (countBytes != mess_paket_size - 8 - 12) {
            printf("ERROR:\n  %ld Bytes empfangen (%s)\n", countBytes, strerror(errno));
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        if (countBytes == -1) {
            //            sleep(1);
        } else {

            if (arbeits_paket_header_recv->train_id < my_max_send_train_id) {
                lac_recv->copy_paket_header(arbeits_paket_header_recv);
                continue;
            }

            if (my_max_recv_train_id < arbeits_paket_header_recv->train_id) {
                my_recv_train_send_countid = arbeits_paket_header_recv->train_send_countid;

                printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
                printf("\033[12;0H# ");
                printf("RECV neu train # train_id %d  # countid: %d #  count pakete: %d       ", arbeits_paket_header_recv->train_id, arbeits_paket_header_recv->train_send_countid, arbeits_paket_header_recv->count_pakets_in_train);
                printf("\033[19;0H");
                fflush(stdout);

            } else if (my_max_recv_train_id == arbeits_paket_header_recv->train_id) {
                if (my_recv_train_send_countid < arbeits_paket_header_recv->train_send_countid) {
                    my_recv_train_send_countid = arbeits_paket_header_recv->train_send_countid;

                    printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
                    printf("\033[13;0H# ");
                    printf("RECV alt train # train_id %d  # countid: %d # count pakete: %d        ", arbeits_paket_header_recv->train_id, arbeits_paket_header_recv->train_send_countid, arbeits_paket_header_recv->count_pakets_in_train);
                    printf("\033[19;0H");
                    fflush(stdout);

                }
            } else {

                printf("ERROR:\n  arbeits_paket_header_recv->train_id: %d \n", arbeits_paket_header_recv->train_id);
                fflush(stdout);

                exit(EXIT_FAILURE);

            }

            if (my_max_recv_train_id < arbeits_paket_header_recv->train_id) {
                my_max_recv_train_id = arbeits_paket_header_recv->train_id;

                if (my_max_train_id < my_max_recv_train_id) {
                    my_max_train_id = my_max_recv_train_id;
                    printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
                    printf("\033[14;0H# ");
                    printf("my_max_train_id recv: %d        ", my_max_train_id);
                    printf("\033[19;0H");
                    fflush(stdout);
                }

            }

            // Header des empfangenen Pakets in das Array sichern 

            lac_recv->copy_paket_header(arbeits_paket_header_recv);

        }


        // wenn leztes Paket vom Paket Train empfangen, dann Antwort Train senden
        if (
                (countBytes == -1)
                ||
                (arbeits_paket_header_recv->paket_id == (arbeits_paket_header_recv->count_pakets_in_train - 1))
                ) {

            arbeits_paket_header_send->count_pakets_in_train = arbeits_paket_header_recv->recv_data_rate / mess_paket_size_doppelt;

            if (lac_recv->last_index_of_paket_header_in_one_array < arbeits_paket_header_send->count_pakets_in_train) {
                arbeits_paket_header_send->count_pakets_in_train = lac_recv->last_index_of_paket_header_in_one_array;
            } else if (arbeits_paket_header_send->count_pakets_in_train < 2) {
                arbeits_paket_header_send->count_pakets_in_train = 2;
            }

            // berechne neue Empfangsrate
            double time_diff;
            double count_all_bytes;
            double bytes_per_sek;

            if (1 < lac_recv->count_paket_headers) {

                time_diff = timespec_diff_double(lac_recv->first_paket_header->recv_time, lac_recv->last_paket_header->recv_time);

                if (time_diff <= 0) {
                    printf("ERROR:\n  time_diff <= 0 \n");
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }

                count_all_bytes = lac_recv->count_paket_headers * mess_paket_size;
                bytes_per_sek = count_all_bytes / time_diff;
                my_bytes_per_sek = bytes_per_sek;
            } else {
                my_bytes_per_sek = mess_paket_size * 6;
            }

            int my_bits_per_sek = 8 * my_bytes_per_sek;
            if (30000000 < my_bits_per_sek) {
                i = 2;
            }

            arbeits_paket_header_send->recv_data_rate = my_bytes_per_sek;

            printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
            printf("\033[15;0H# ");
            printf("Last: count: %d # ", lac_recv->count_paket_headers);
            printf("train id: %d # ", arbeits_paket_header_recv->train_id);
            printf("train send countid : %d # ", arbeits_paket_header_recv->train_send_countid);
            printf("paket id: %d # ", arbeits_paket_header_recv->paket_id);
            printf("count in t: %d # ", arbeits_paket_header_recv->count_pakets_in_train);
            printf("recv %.2f %% # ", (double) ((double) lac_recv->count_paket_headers / (double) arbeits_paket_header_send->count_pakets_in_train) * 100.0);
            printf("time_diff: %.2f # ", time_diff);
            if (bytes_per_sek >= 1024 * 1024) {
                printf("data_rate: %.2f MB / Sek        ", bytes_per_sek / (1024 * 1024));
            } else if (bytes_per_sek >= 1024) {
                printf("data_rate: %.2f KB / Sek        ", bytes_per_sek / (1024));
            } else {
                printf("data_rate: %.2f B / Sek        ", bytes_per_sek);
            }
            printf("\033[19;0H");
            fflush(stdout);

            arbeits_paket_header_send->train_id = my_max_recv_train_id + 1;

            if (my_last_send_train_id < arbeits_paket_header_send->train_id) {
                my_send_train_send_countid = 0;
            } else {
                my_send_train_send_countid++;
            }

            arbeits_paket_header_send->train_send_countid = my_send_train_send_countid;

            my_last_send_train_id = arbeits_paket_header_send->train_id;

            if (0 < lac_recv->count_paket_headers) {
                arbeits_paket_header_send->last_recv_train_id = lac_recv->last_paket_header->train_id;
                arbeits_paket_header_send->last_recv_train_send_countid = lac_recv->last_paket_header->train_send_countid;
                arbeits_paket_header_send->last_recv_paket_id = lac_recv->last_paket_header->paket_id;
            }

            if (20000 < arbeits_paket_header_send->count_pakets_in_train) {
                i = 2;
            }

            printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
            printf("\033[16;0H# ");
            printf("sende %d Pakete # train_id: %d # send_countid: %d       ", arbeits_paket_header_send->count_pakets_in_train, arbeits_paket_header_send->train_id, arbeits_paket_header_send->train_send_countid);
            printf("\033[19;0H");
            fflush(stdout);
            for (i = 0; i < arbeits_paket_header_send->count_pakets_in_train; i++) {
                arbeits_paket_header_send->paket_id = i;
                clock_gettime(CLOCK_REALTIME, &(arbeits_paket_header_send->send_time));

                countBytes = sendto(client_mess_socket, arbeits_paket_send, mess_paket_size - 8 - 12, 0, (struct sockaddr*) &clientAddr, clientAddrSize);

                if (countBytes != mess_paket_size - 8 - 12) {
                    printf("ERROR:\n  %ld Bytes gesendet (%s)\n", countBytes, strerror(errno));
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                } else {
                    lac_send3->copy_paket_header(arbeits_paket_header_send);
                }
            }


            if (0 < lac_recv->count_paket_headers) {
                struct paket_header *x;

                if (lac_send1 == lac_send3) {
                    x = lac_send2->give_paket_header(lac_recv->first_paket_header->last_recv_train_id, lac_recv->first_paket_header->last_recv_train_send_countid, lac_recv->first_paket_header->last_recv_paket_id);
                    lac_send2->save_to_file_and_clear();
                    lac_send3 = lac_send2;
                } else {
                    x = lac_send1->give_paket_header(lac_recv->first_paket_header->last_recv_train_id, lac_recv->first_paket_header->last_recv_train_send_countid, lac_recv->first_paket_header->last_recv_paket_id);
                    lac_send1->save_to_file_and_clear();
                    lac_send3 = lac_send1;
                }

                if (x != NULL) {

                    double t = timespec_diff_double(x->send_time, lac_recv->first_paket_header->recv_time);

                    printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
                    printf("\033[18;0H# ");
                    printf("rtt ist %f    ", t);
                    printf("\033[19;0H");
                    fflush(stdout);
                }
            }


            if (my_max_send_train_id < my_last_send_train_id) {
                my_max_send_train_id = my_last_send_train_id;

                if (my_max_train_id < my_max_send_train_id) {
                    my_max_train_id = my_max_send_train_id;
                    printf("\033[11;0H  \033[12;0H  \033[13;0H  \033[14;0H  \033[15;0H  \033[16;0H  \033[17;0H  \033[18;0H  \033[19;0H  ");
                    printf("\033[17;0H# ");
                    printf("my_max_train_id send: %d        ", my_max_train_id);
                    printf("\033[19;0H");
                    fflush(stdout);
                }
            }

            lac_recv->save_to_file_and_clear();

        }


    }

    delete (lac_recv);
    delete (arbeits_paket_recv);
    delete (arbeits_paket_header_recv);
    delete (arbeits_paket_send);
    delete (arbeits_paket_header_send);
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