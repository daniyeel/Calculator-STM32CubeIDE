//////////////////////////////////////////////////////////////////////////////
// prog: mailbox.c
//       shows mailbox handling and along the way handles UART-kbd input
// note: for OSMboxCreate, see main.c!
// auth: MSC
//////////////////////////////////////////////////////////////////////////////

#include <includes.h>
#include <ctype.h>      // toupper
#include "main.h"

// MboxPostTaskTask; task that:
// 1. reads incoming chars from uart to string
// 2. posts command-string to mailbox
void MboxPostTask (void *pdata)
{
    char          c;

	UART_puts((char *)__func__); UART_puts("started, enter key on your keyboard\n\r");

	while(TRUE)
    {
		OSTimeDly(LOOP_DELAY); // add some delay
		if ((c=UART_get()) == 255) // int value == -1
			continue;

		OSMboxPost(MboxHandle, &c);         // post address of buf
		if (Uart_debug_out & MAILBOX_DEBUG_OUT)
		{
			UART_puts("MboxPostTask, posts: "); UART_putchar(c); UART_puts("\n\r"); // to UART
		}
	}
}

// MboxPendTask;
// waits/pends for a posted string and writes it to outputs
void MboxPendTask (void *pdata)
{
	INT8U    error;
	char    *s; // pointer to buffer, which is one char!

	UART_puts((char *)__func__); UART_puts("started\n\r");

	while (TRUE)
	{
		s = OSMboxPend(MboxHandle, WAIT_FOREVER, &error);

		switch(s[0])
		{
		case '1': Uart_debug_out ^= MUTEX_DEBUG_OUT; // toggle output on/off
				  UART_puts("\n\rmutex output = "); UART_puts(Uart_debug_out & MUTEX_DEBUG_OUT ? "ON\n\r" : "OFF\n\r");
				  break;
		case '2': Uart_debug_out ^= MAILBOX_DEBUG_OUT; // toggle output on/off
		  	      UART_puts("\n\rmailbox output = "); UART_puts(Uart_debug_out & MAILBOX_DEBUG_OUT ? "ON\n\r" : "OFF\n\r");
				  break;
		case '3': Uart_debug_out ^= FLAG_DEBUG_OUT; // toggle output on/off
		  	      UART_puts("\n\rflag output = "); UART_puts(Uart_debug_out & FLAG_DEBUG_OUT ? "ON\n\r" : "OFF\n\r");
				  break;
		case '4': Uart_debug_out ^= QUEUE_DEBUG_OUT; // toggle output on/off
		  	  	  UART_puts("\n\rqueue output = "); UART_puts(Uart_debug_out & QUEUE_DEBUG_OUT ? "ON\n\r" : "OFF\n\r");
				  break;

		// following keys to hold or resume a specific task or taskgroup
		case '5': hold_resume(MUTEX_DEBUG_OUT); break;
		case '6': UART_puts("Cannot pause keyboard handler (mailbox-task)\n\r"); break;
		case '7': hold_resume(FLAG_DEBUG_OUT); break;
		case '8': hold_resume(QUEUE_DEBUG_OUT); break;
		case '0': hold_resume(INTERRUPT_DEBUG_OUT); break;

		// display stackdata of all tasks
		case 's':
		case 'S': displayAllStackData(); break;
		}
		
		if (Uart_debug_out & MAILBOX_DEBUG_OUT)
		{
			UART_puts("Received from MboxPostTask: "); UART_putchar(s[0]); UART_puts("\n\r");
		}
	}
}
