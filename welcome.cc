/*
 * Copyright (c) 2009-2010, Oracle and/or its affiliates. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Oracle nor the names of its contributors
 *   may be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

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

    // Prints arguments...
    /*    if (argc > 1) {
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
*/
    
    ServerClass *s = new ServerClass();

    printf("\n main EXIT_SUCCESS \n");
    return EXIT_SUCCESS;
}
