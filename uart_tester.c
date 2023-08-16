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

#include <string.h>

//---------------------------------------------------------------------------

typedef struct {
    FifoDataport*   uart_input_fifo; // FIFO in dataport shared with the UART driver
    uint8_t*        uart_output_fifo;
} uart_ctx_t;

uart_ctx_t uart_ctx = { 0 };


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

/*
OS_Error_t rng_get_bytes(void * buffer, size_t bytes) {
    int bytes_to_request = bytes / 16;
    bytes_to_request += (bytes % 16 ? 1: 0)
}*/


void uart_rng_write(uart_ctx_t *ctx, char *bytes, size_t amount) {
    // Copy data into uart ouput buffer
    memcpy(ctx->uart_output_fifo, bytes, amount);

    // Notfiy the uart driver about the data
    uart_rpc_write(amount);
}


void uart_rng_read(uart_ctx_t *ctx, void *buffer, size_t *amount) {
    FifoDataport* fifo = ctx->uart_input_fifo;

    // Get buffer with the available data
    *amount = FifoDataport_getContiguous(fifo, &buffer);

    print_bytes((void *) buffer, *amount);
}


void clean_fifo(uart_ctx_t *ctx, size_t amount) {
    FifoDataport* fifo = ctx->uart_input_fifo;
    FifoDataport_remove(fifo, amount);
}

static void data_available_callback(void *ctx_ptr) {
    Debug_LOG_DEBUG("Uart event callback triggered");
    uart_ctx_t *ctx = ctx_ptr;

    
    int err;
    void *buffer = NULL;
    size_t available = 0;
    
    uart_rng_read(ctx, buffer, &available);
    
    err = uart_event_reg_callback((void *) &data_available_callback, (void *) ctx);
    if (err)  {
        Debug_LOG_ERROR("uart_event_reg_callback failed, code: %d", err);
    }
}

//---------------------------------------------------------------------------
void pre_init(void)
{
    Debug_LOG_DEBUG("pre_init");
}


//---------------------------------------------------------------------------
void post_init(void)
{
    uart_ctx_t *ctx = &uart_ctx;

    // init function to register callback
    OS_Dataport_t input_port  = OS_DATAPORT_ASSIGN(uart_input_port);
    OS_Dataport_t output_port = OS_DATAPORT_ASSIGN(uart_output_port);

    ctx->uart_input_fifo  = (FifoDataport *) OS_Dataport_getBuf(input_port);
    ctx->uart_output_fifo = (uint8_t *) OS_Dataport_getBuf(output_port);

    int err = uart_event_reg_callback((void *) &data_available_callback, (void *) ctx);
    if (err) {
        Debug_LOG_ERROR("uart_event_reg_callback failed, code: %d", err);
    }
    Debug_LOG_DEBUG("uart callback registered!");
}

int run(void) {
    //test the rng

    printf("Starting Test\n");
    uart_ctx_t *ctx = &uart_ctx;
    char msg[] =  {0x73, 0x01};
    uart_rng_write(ctx, msg, 2);
    
    return OS_SUCCESS;
}