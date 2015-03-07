/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_psoc4ble
 * @{
 *
 * @file
 * @brief       Implementation of the low-level UART driver for the PSoC 5LP
 *
 * @author      Murat Cakmak <mail@muratcakmak.net>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

#include <project.h>

/* guard the file in case no UART is defined */
#if UART_0_EN

/************************** MACRO DEFINITIONS ********************************/
#define ENABLE_TX_INTERRUPT 	UART0_SetTxInterruptMode(UART0_INTR_TX_UART_DONE)

#define DISABLE_TX_INTERRUPT	UART0_SetTxInterruptMode(0)
/*************************** TYPE DEFINITIONS ********************************/
/**
 * @brief Struct holding the configuration data for a UART device
 */
typedef struct {
    uart_rx_cb_t rx_cb;         /**< receive callback */
    uart_tx_cb_t tx_cb;         /**< transmit callback */
    void *arg;                  /**< callback argument */
} uart_conf_t;

/******************************* VARIABLES ***********************************/
/**
 * @brief UART device configurations
 */
static uart_conf_t config[UART_NUMOF];

/**************************** PRIVATE FUNCTIONS ******************************/
/**
 * @brief Handler function for UART Interrupts
 *
 */
extern int delay;
CY_ISR(UART0_IRQHandler)
{
//    if (UART0_GetTxInterruptSource() & UART0_INTR_TX_UART_DONE)
//    {
//    	/* Clear interrupt flag */
//        UART0_ClearTxInterruptSource(UART0_INTR_TX_UART_DONE);
//    }
//    
//    if (UART0_GetRxInterruptSource() & UART0_INTR_RX_NOT_EMPTY)
//    {
//    	/* Clear interrupt flag */
//        UART0_ClearRxInterruptSource(UART0_INTR_RX_NOT_EMPTY);
//
//        char data = UART0_UartGetChar();
//        
//        delay = 200;
//    }
    
//    if (UART0_GetTxInterruptSource() & UART0_INTR_TX_UART_DONE)
//    {
//    	/* Clear interrupt flag */
//        UART0_ClearTxInterruptSource(UART0_INTR_TX_UART_DONE);
//
//        /* Disable TX Interrupt If client request */
////        if (config[UART_0].tx_cb(config[UART_0].arg) == 0) {
////        	DISABLE_TX_INTERRUPT;
////        }
//    }
//    
//    if (UART0_GetRxInterruptSource() & UART0_INTR_RX_NOT_EMPTY)
//    {
//    	/* Clear interrupt flag */
//        UART0_ClearRxInterruptSource(UART0_INTR_RX_NOT_EMPTY);
//
//        char data = UART0_UartGetChar();
////        config[UART_0].rx_cb(config[UART_0].arg, data);
//    }
//
    if (sched_context_switch_request) {
        thread_yield();
    }
}

/**************************** PUBLIC FUNCTIONS *******************************/

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{    
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }
 
    /* save callback */
    config[uart].rx_cb = rx_cb;
    config[uart].tx_cb = tx_cb;
    config[uart].arg = arg;

    switch (uart) {

        case UART_0:
            /* configure and enable global device interrupts */
            UART0_IRQn_StartEx(UART0_IRQHandler);

            if (rx_cb != NULL)
            {
                UART0_SetRxInterruptMode(UART0_INTR_RX_NOT_EMPTY);
                UART0_ClearRxInterruptSource(UART0_INTR_RX_ALL);
            }
            
            if (tx_cb != NULL)
            {
                UART0_SetTxInterruptMode(UART0_INTR_TX_UART_DONE);
                UART0_ClearTxInterruptSource(UART0_INTR_TX_ALL);
            }
            
            UART0_Start();
            
            break;
        default:
            return -1;
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    switch (uart) {
        case UART_0:
            /* this implementation only supports 115200 baud */
            if (baudrate != 115200) {
                return -2;
            }
            break;
        default:
            return -1;
    }

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    switch (uart) {
        case UART_0:
            /* enable TX interrupt */
        	ENABLE_TX_INTERRUPT;
            break;
    }
}

int uart_write(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
        	UART0_UartPutChar(data);
            break;
        default:
            return -1;
    }

    return 1;
}

int uart_read_blocking(uart_t uart, char *data)
{
    switch (uart) {
        case UART_0:
            /* Wait until a byte received */
            while (!(UART0_GetRxInterruptSource() & UART0_INTR_RX_NOT_EMPTY));
            
            *data = UART0_UartGetChar();
            break;
        default:
            return -1;
    }

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    switch (uart) {
        case UART_0:
            /* Wait until all previous bytes are sent */
            while(!(UART0_GetTxInterruptSource() & UART0_INTR_TX_EMPTY));

            UART0_UartPutChar(data);
            break;
        default:
            return -1;
    }

    return 1;
}

void uart_poweron(uart_t uart)
{
    switch (uart) {
        case UART_0:
            UART0_Start();
            break;
    }
}

void uart_poweroff(uart_t uart)
{
    switch (uart) {
        case UART_0:
            UART0_Stop();
            break;
    }
}

#endif /* UART_0_EN */
