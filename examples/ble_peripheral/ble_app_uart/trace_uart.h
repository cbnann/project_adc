#ifndef TRACE_UART_H
#define TRACE_UART_H
#include <stdint.h>
#include <stdio.h>


#ifdef TRACE_UART
	#define TRACE_LOG
	#define TRACE_LOG_FORMAT
#endif 

#define RX_PIN_NUMBER  			11     // UART RX pin number.
#define TX_PIN_NUMBER  			9     // UART TX pin number.

#define CTS_PIN_NUMBER 			10    // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER 			8    // Not used if HWFC is set to false
#define HWFC_           			false 	   // UART hardware flow control
    

void _trace_init(void);
void _trace_dump(uint8_t * p_buffer, uint32_t len);
void _trace_dump_f(uint8_t * p_buffer, uint32_t len);

#ifdef TRACE_UART
    #define trace_init          _trace_init
    #ifdef TRACE_LOG
        #define trace_dump      _trace_dump      
        #ifdef  TRACE_LOG_FORMAT
            #define trace_log(msg,...)      {printf("%-10s\t\t%d\t",__MODULE__,__LINE__);printf(msg,##__VA_ARGS__);}
        #else
            #define trace_log               printf
        #endif
     #else
        #define trace_log(...)
        #define trace_dump(...)            
     #endif
            
     #ifdef TRACE_INFO
        #ifdef  TRACE_INFO_FORMAT
            #define trace_info(msg,...)      {printf("%-10s\t\t%d\t",__MODULE__,__LINE__);printf(msg,##__VA_ARGS__);}
        #else
            #define trace_info               printf
        #endif
     #else
        #define trace_info(...)
     #endif
#else
    #define trace_init(...)  
    #define trace_dump(...)   
    #define trace_log(...)
    #define trace_info(...)
#endif
        
     

        
#endif
