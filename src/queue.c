#include <string.h>
#include <stdlib.h> // atoi
#include <ctype.h>  // toupper

#include "includes.h"
#include "taskcreate.h" // for task function prototypes
#include "main.h"

// variables for queue (externally used in calcul.c)
int firNum  = 0;
int oper  = 0;
int secNum = 0;
int res = 0;

void outQ_member(PQ, char *);

// print all data from data_queue (pq)
void outQ_member(PQ pq, char *func)
{
    UART_puts("\n\r==============================\n\r");
    UART_puts(func);                UART_puts("\n\r");
    UART_puts("q.firstNumber: ");   UART_putint(pq->firstNumber);  UART_puts("\n\r");
    UART_puts("q.round: ");         UART_putint(pq->value);        UART_puts(" (13 >> + & 14 >> - & 15 >> *; & 16 >> /)\n\r");
    UART_puts("q.secondNumber: ");  UART_putint(pq->secondNumber); UART_puts("\n\r");
    UART_puts("q.result: ");        UART_putint(pq->result);       UART_puts("\n\r");
    UART_puts("==============================");
}

// QueuePostTask:
// 1. fills every sec the next struct out of array
// 2. posts address of that struct to os-queue
// 3. increments to next array-member
void QueuePostTask (void *pdata)
{
	PQ            pq; // pointer to Q structure
	int 	      i = 0;

	UART_puts((char *)__func__); UART_puts("started\n\r");

    while(TRUE) 
    {
       	OSTimeDly(400);

    	// time has past, fill next data structure member and post its address to os-queue
    	pq = &data_queue[i];       // data_queue declared in main.c

    	// access 'x' of data_queue struct (pq->x) and set equal to value calculated in calcul.c
    	pq->firstNumber  = firNum;
    	pq->value        = oper;
    	pq->secondNumber = secNum;
    	pq->result       = res;
    	strcpy(pq->text, "Values above give a summary of the calculation!");

		OSQPost(QueueHandle, pq);
	}
}


// InputQueuePendTask:
// 1. waits until queue is full
// 2. scans thru queue, accepting every member one by one
// 3. outputs data of every q-member
// 4. cleans up queue
void InputQueuePendTask(void *pdata)
{
	INT8U         error;
	PQ            pq; // pointer to Q structure
	OS_Q_DATA     qd; // queue data structure

    UART_puts((char *)__func__); UART_puts("started\n\r");

    while(TRUE)
	{
		OSTimeDly(LOOP_DELAY);

		OSQQuery(QueueHandle, &qd); // get number of members
		if (qd.OSNMsgs < QSIZE) // wait until q is full
			continue;

		// this code doesn't work correctly, values get ruined
		if (KEYS_read() == key_en)
		{
			if (Uart_debug_out & QUEUE_DEBUG_OUT)
				outQ_member(pq, (char *)__func__); // display data to uart
		}
	}
}
