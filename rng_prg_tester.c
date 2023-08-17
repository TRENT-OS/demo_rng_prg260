/*
 *  UART test
 *
 *  Copyright (C) 2020-2023, HENSOLDT Cyber GmbH
 */

#include "OS_Error.h"
#include "OS_Dataport.h"
#include "lib_io/FifoDataport.h"
#include "lib_debug/Debug.h"

#include <camkes.h>

static OS_Dataport_t port = OS_DATAPORT_ASSIGN(entropy_port);


void print_bytes(void *ptr, int size) 
{
    unsigned char *p = ptr;
    int i;
    for (i=0; i<size; i++) {
        printf("%02hhX ", p[i]);
    }
    printf("\n");
}

int test_read_bytes(int bytes_to_read, int expected) {
    printf("Requesting %i Bytes:\n", bytes_to_read);

    size_t read = entropy_rpc_read(bytes_to_read);

    if (read - expected) {
        printf("Failure: Expected %i Bytes, received %lu Bytes\n", expected, read);
        return 1;
    }

    printf("Success: Read %lu Bytes:\n", read);
    print_bytes((void *) OS_Dataport_getBuf(port), read);
    return 0;
}


//---------------------------------------------------------------------------
void pre_init(void) {
    Debug_LOG_DEBUG("pre_init");
}


//---------------------------------------------------------------------------
void post_init(void) {
    Debug_LOG_DEBUG("post init");
}



int run(void) {
    //test the rng

    printf("Starting Test\n");
    int fail = 0;
    fail += test_read_bytes(16, 16);
    fail += test_read_bytes(32, 32);
    fail += test_read_bytes(4096, 4096);
    fail += test_read_bytes(31, 16);
    if (fail) {
        printf("Test failed!\n");
        return OS_ERROR_GENERIC;
    }
    printf("Success Test passed!\n");
    return OS_SUCCESS;
}