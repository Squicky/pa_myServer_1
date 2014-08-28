/* 
 * File:   ListArrayClass.cpp
 * Author: user
 * 
 * Created on 28. August 2014, 11:47
 */

#include "ListArrayClass.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

ListArrayClass::ListArrayClass(int _mess_paket_size) {

    mess_paket_size = _mess_paket_size;
    count_arrays = 1;
    count_paket_headers = 0;

    nextListArrayClass = NULL;
    first_paket_header = NULL;
    last_paket_header = NULL;

    count_paket_header_in_one_array = MAX_UMTS_DATA_RATE / _mess_paket_size;
    last_index_of_paket_header_in_one_array = count_paket_header_in_one_array - 1;
    paket_header_size = sizeof (paket_header);
    int array_paket_header_size = count_paket_header_in_one_array * paket_header_size;

    array_paket_header = NULL;
    array_paket_header = (paket_header*) malloc(array_paket_header_size);

    if (array_paket_header == NULL) {
        printf("ERROR:\n  Kein virtueller RAM mehr verfügbar \n  (%s)\n", strerror(errno));
        printf("  array_paket_header: %p \n", array_paket_header);
        exit(EXIT_FAILURE);
    }
}

ListArrayClass::~ListArrayClass() {
    delete (array_paket_header);
    if (nextListArrayClass != NULL) {
        delete (this->nextListArrayClass);
    }
}

paket_header *ListArrayClass::copy_paket_header(struct paket_header *ph) {

    if (count_paket_headers < count_paket_header_in_one_array) {
        if (0 == count_paket_headers) {
            first_paket_header = &array_paket_header[count_paket_headers];
            last_paket_header = first_paket_header;
            memcpy(first_paket_header, ph, paket_header_size);
            count_paket_headers++;
            return first_paket_header;
        } else {
            last_paket_header = &array_paket_header[count_paket_headers];
            memcpy(last_paket_header, ph, paket_header_size);
            count_paket_headers++;
            return last_paket_header;
        }
    } else {
        if (this->nextListArrayClass == NULL) {
            nextListArrayClass = new ListArrayClass(mess_paket_size);
        }
        count_paket_headers++;
        last_paket_header = nextListArrayClass->copy_paket_header(ph);
        return last_paket_header;
    }

}

void ListArrayClass::save_to_file_and_clear() {
    count_paket_headers = 0;

    first_paket_header = NULL;
    last_paket_header = NULL;
}