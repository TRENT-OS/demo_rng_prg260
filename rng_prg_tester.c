/*
 *  UART test
 *
 *  Copyright (C) 2020-2023, HENSOLDT Cyber GmbH
 */

#include "OS_Error.h"
#include "OS_Dataport.h"
#include "lib_io/FifoDataport.h"
#include "lib_debug/Debug.h"
#include "string.h"
#include "../../components/Rng_Prg260/interface/if_OS_PRG260_Keystore.h"
#include <camkes.h>

static OS_Dataport_t port = OS_DATAPORT_ASSIGN(entropy_port);

typedef struct {
    Prg260_Key_t key1;
    Prg260_Key_t key2;
    Prg260_Pin_t user_pin;
    Prg260_Pin_t master_pin;
} keystore_ctx_t;

//---------------------------------------------------------------------------

void print_bytes(void *ptr, int size) 
{
    unsigned char *p = ptr;
    int i;
    for (i=0; i<size; i++) {
        printf("%02hhX ", p[i]);
    }
    printf("\n");
}

bool test_read_bytes(int bytes_to_read) {
   Debug_LOG_DEBUG("Requesting %i Bytes:\n", bytes_to_read);

    size_t read = entropy_rpc_read(bytes_to_read);

    if (read - bytes_to_read) {
        Debug_LOG_ERROR("Failure: Expected %i Bytes, received %zu Bytes\n", bytes_to_read, read);
        return true;
    }

    Debug_LOG_DEBUG("Success: Read %zu Bytes:\n", read);
    print_bytes((void *) OS_Dataport_getBuf(port), read);
    return false;
}


//---------------------------------------------------------------------------

OS_Error_t rnd_key(Prg260_Key_t *key) {
    if (entropy_rpc_read(sizeof(Prg260_Key_t)) != sizeof(Prg260_Key_t)) {
        return OS_ERROR_GENERIC;
    }
    memcpy(key, OS_Dataport_getBuf(port), sizeof(Prg260_Key_t));
    return OS_SUCCESS;
}

OS_Error_t test_keystore(void) {
    keystore_ctx_t k_ctx = { 0 };

    k_ctx.master_pin = 5555;
    k_ctx.user_pin = 1111;

    if (rnd_key(&k_ctx.key1) || rnd_key(&k_ctx.key2) ) {
        return OS_ERROR_GENERIC;
    }

    if (prg260_keystore_rpc_state()) {
        Debug_LOG_ERROR("Keyserver EEPROM is already written.");
        if (prg260_keystore_rpc_reset(k_ctx.master_pin)) {
            Debug_LOG_ERROR("Reset of the keystore failed.");
            return OS_ERROR_GENERIC;
        }
        Debug_LOG_DEBUG("Keyserver resetted");
    }

    prg260_keystore_rpc_init(&k_ctx.key1, &k_ctx.key2, k_ctx.user_pin, k_ctx.master_pin);


    Prg260_Key_t tmp_key[] = { 0 };
    if (prg260_keystore_rpc_get_key(k_ctx.user_pin, tmp_key)) {
        if (memcmp(tmp_key, k_ctx.key1, sizeof(Prg260_Key_t))) {
            Debug_LOG_ERROR("Key retrieval failed.");
            return OS_ERROR_GENERIC;
        }
    }
    Debug_LOG_DEBUG("Retrieved Key is equal to stored key");

    if (prg260_keystore_rpc_verify_key(k_ctx.user_pin, (Prg260_Key_t *) k_ctx.key2)) {
        Debug_LOG_ERROR("Key comparison failed!");
        return OS_ERROR_GENERIC;
    } 
    Debug_LOG_DEBUG("Key2 is equal to the stored key.");

    if (prg260_keystore_rpc_change_user_pin(k_ctx.master_pin, 1112)) {
        Debug_LOG_ERROR("Pin change failed.");
        return OS_ERROR_GENERIC;
    }
    Debug_LOG_DEBUG("User Pin changed successfully");

    if (prg260_keystore_rpc_reset(k_ctx.master_pin)) {
        Debug_LOG_ERROR("Reset failed");
        return OS_ERROR_GENERIC;
    }
    Debug_LOG_DEBUG("Keyserver resetted");

    return OS_SUCCESS;
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

    Debug_LOG_DEBUG("Starting Test");
    bool fail = false;

// The PRG260 can provide up to 4096 Bytes per read, but the default uart 
// Dataport size is 4096 and some ~70ish bytes are reserved for the fifo
// tracking. Therefore if 4096 Bytes are required, the Dataport size in 
// system_config.h in this repo must be set to 8096 as well as the UART
// UART dataport size for the Platform in question in `components/UART/plat/`
// This is tested and works but due to compatibility reasons with all platforms
// not set as default.
    fail =  test_read_bytes(16)   || 
            test_read_bytes(32)   ||
            test_read_bytes(2048) || 
            test_read_bytes(31)   ||
            test_keystore();

    if (fail) {
        Debug_LOG_ERROR("Test failed!");
        return OS_ERROR_GENERIC;
    }

    printf("Success Test passed!\n");
    return OS_SUCCESS;
}