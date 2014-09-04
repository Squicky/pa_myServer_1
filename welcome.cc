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





    // O_WRONLY nur zum Schreiben öffnen
    // O_RDWR zum Lesen und Schreiben öffnen
    // O_RDONLY nur zum Lesen öffnen
    // O_CREAT Falls die Datei nicht existiert, wird sie neu angelegt. Falls die Datei existiert, ist O_CREAT ohne Wirkung.
    // O_APPEND Datei öffnen zum Schreiben am Ende
    // O_EXCL O_EXCL kombiniert mit O_CREAT bedeutet, dass die Datei nicht geöffnet werden kann, wenn sie bereits existiert und open() den Wert –1 zurückliefert (–1 == Fehler).
    // O_TRUNC Eine Datei, die zum Schreiben geöffnet wird, wird geleert. Darauffolgendes Schreiben bewirkt erneutes Beschreiben der Datei von Anfang an. Die Attribute der Datei bleiben erhalten.
    int File_Deskriptor;
    char name[] = "test.txt";
    if ((File_Deskriptor = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG, S_IRWXO)) != -1) {

        printf("Datei \"%s\" geöffnet \n ", name);

        char puffer[] = "2inhalt1";
        int bytezahl = strlen(puffer);

        write(File_Deskriptor, puffer, bytezahl);

        if ((close(File_Deskriptor)) != -1) {
            printf("Datei \"%s\" geschlossen \n ", name);
        } else {
            printf("Fehler beim Schliessen der Datei\n");
        }
    } else {
        printf("Fehler beim Öffnen der Datei \"%s\" \n ", name);
    }


    ServerClass *s = new ServerClass();

    printf("\nmain EXIT_SUCCESS\n");
    return EXIT_SUCCESS;
}
