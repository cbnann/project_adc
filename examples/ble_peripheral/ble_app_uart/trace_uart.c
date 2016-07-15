#include <stdio.h>
#include "simple_uart.h"
#include "trace_uart.h"

//struct __FILE { int handle; /* Add whatever you need here */ };
//FILE __stdout;
//FILE __stdin;

void _trace_init(void)
{
	simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC_);	
}


int fputc(int ch, FILE * p_file) 
{
    simple_uart_put(ch);
    return ch;
}

int fgetc(FILE *fp)
{
    char ch;
    ch = simple_uart_get();
    return ch;
}

void _trace_dump(uint8_t * p_buffer, uint32_t len)
{
    for (uint32_t index = 0; index <  len; index++)
    {
        printf("%02X ",p_buffer[index]);
    }
    printf("\r\n");
}

void _trace_dump_f(uint8_t * p_buffer, uint32_t len)
{
    for (uint32_t index = 0; index <  len; index++)
    {
        printf("0x%02X, ",p_buffer[index]);
    }
    printf("\r\n");
}

