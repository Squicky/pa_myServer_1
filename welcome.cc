
// save: 21.09.2014 18:45


#include <iostream>

#include <cstdlib>
#include <cstdio>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <time.h>

#include <string.h>

#include "ServerClass.h"
#include "ServerClientInfo.h"

timespec timespec_diff_timespec(timespec *start, timespec *end) {
    timespec temp;

    if (end->tv_nsec < start->tv_nsec) {
        temp.tv_sec = end->tv_sec - start->tv_sec - 1;
        temp.tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
    } else {
        temp.tv_sec = end->tv_sec - start->tv_sec;
        temp.tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return temp;
}



int main(int argc, char**argv) {
    // Prints welcome message...
    //    std::cout << "Welcome ... \n" << std::endl;

    /*
        // Prints arguments...
        if (argc > 1) {
            std::cout << std::endl << "Arguments:" << std::endl;
            for (int i = 1; i < argc; i++) {
                std::cout << i << ": " << argv[i] << std::endl;
            }
        }
     */
/*
    timespec a, b, c, d, e, f, g;
    timespec *pa = &a;
    timespec *pb = &b;
    timespec *pc = &c;
    timespec *pd = &d;
    timespec *pe = &e;
    timespec *pf = &f;
    timespec *pg = &g;

    clock_gettime(CLOCK_REALTIME, pa);
    clock_gettime(CLOCK_REALTIME, pb);
    clock_gettime(CLOCK_REALTIME, pc);
    clock_gettime(CLOCK_REALTIME, pd);

    usleep(1000);
    clock_gettime(CLOCK_REALTIME, pe);
    usleep(1000);
    clock_gettime(CLOCK_REALTIME, pf);
    usleep(1000);
    clock_gettime(CLOCK_REALTIME, pg);
    
    timespec x = timespec_diff_timespec(pa, pb);
    printf(" a - b: %ld,%.9ld  # %.9f\n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);
    x = timespec_diff_timespec(pb, pc);
    printf(" b - c: %ld,%.9ld  # %.9f \n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);
    x = timespec_diff_timespec(pc, pd);
    printf(" c - d: %ld,%.9ld  # %.9f \n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);
    x = timespec_diff_timespec(pd, pe);
    printf(" d - e: %ld,%.9ld  # %.9f \n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);
    x = timespec_diff_timespec(pe, pf);
    printf(" e - f: %ld,%.9ld  # %.9f \n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);
    x = timespec_diff_timespec(pf, pg);
    printf(" f - g: %ld,%.9ld  # %.9f \n", x.tv_sec, x.tv_nsec, (double) x.tv_nsec / 1000000000.0);


    int size_int = sizeof (int);
    int size_double = sizeof (double);
    int size_timespec = sizeof (timespec);

    int size_paket_header = sizeof (paket_header);

    paket_header ph;

    int size_count_pakets_in_train = sizeof (ph.count_pakets_in_train);
    int size_recv_time = sizeof (ph.recv_time);

    long int longint;
    int size_longint = sizeof (longint);

    struct timespec ts;

    int size_tstv_sec = sizeof (ts.tv_sec);
    int size_tstv_nsec = sizeof (ts.tv_nsec);
*/

    ServerClass *s = new ServerClass();

    printf("\n main EXIT_SUCCESS \n");
    return EXIT_SUCCESS;
}
