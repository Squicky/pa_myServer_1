/* 
 * File:   ServerClientClass.h
 * Author: user
 *
 * Created on 17. August 2014, 19:36
 */

#ifndef SERVERCLIENTCLASS_H
#define	SERVERCLIENTCLASS_H

#include <arpa/inet.h>

#define LOCAL_Mess_PORT 7000

class ServerClientClass {
public:
    ServerClientClass(int _paket_size, char _zeit_dateiname[]);
    //ServerClientClass(const ServerClientClass& orig);
    //virtual ~ServerClientClass();


    int udp_rec_port;

    static timespec timespec_diff_timespec(timespec *start, timespec *end);
    static double timespec_diff_double(timespec *start, timespec *end);

    bool log_files_ok;
    bool in_while_schleife;
    bool stop;
    timespec end_time;
    struct sockaddr_in otherAddr;

private:
    struct sockaddr_in meineAddr;
    socklen_t otherAddrSize;
    int other_mess_socket;

    int server_rec_port;

    pthread_t rec_thread;
    static void * rec_threadStart(void * vdata);
    void rec_threadRun();

    int mess_paket_size;

    char zeit_dateiname[255];
};

#endif	/* SERVERCLIENTCLASS_H */

