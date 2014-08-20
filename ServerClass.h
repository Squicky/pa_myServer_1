/* 
 * File:   ServerClass.h
 * Author: user
 *
 * Created on 17. August 2014, 13:14
 */

#ifndef SERVERCLASS_H
#define	SERVERCLASS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <list>

#include "ServerClientClass.h"

#define LOCAL_Control_SERVER_PORT 5000

class ServerClass {
public:
    ServerClass();
    ServerClass(const ServerClass& orig);
    virtual ~ServerClass();
private:
    static void * threadStart(void * vdata);
    void threadRun();
    pthread_t CUS_thread;
    
    std::list<ServerClientClass *> ServerClientList;

};

#endif	/* SERVERCLASS_H */

