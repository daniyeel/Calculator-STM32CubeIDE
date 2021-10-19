/*
 * calcul.c
 *
 *  Created on: 16 okt. 2021
 *      Author: Daniel
 */

#include <includes.h>
#include "main.h"

//////////////////////////////////////////////////////////////////////////////
// func: InputFlagPost
// args: void *pdata, needed by os
// comm: read key and stores it in key. also passes the key further using a flag
//////////////////////////////////////////////////////////////////////////////
void InputFlagPost (void *pdata)
{
    unsigned int key;
    INT8U        error;

    UART_puts("\n\rINPUTFLAGPOST!\n\r");

    UART_puts((char *)__func__); UART_puts("started, press a key on the ARM-board...\n\r");

    while(TRUE)
    {
       	OSTimeDly(3);             // don't block other tasks
        if ((key=KEYS_read())==0) // read keys...
        {
        	OSTimeDly(50);        // IMPORTANT DELAY, DON'T REMOVE
			continue;
        }

		OSFlagPost(FlagHandle, key, OS_FLAG_SET, &error); // OSFlagPost for passing the value of key
		OSTimeDly(LOOP_DELAY);
	}
}

//////////////////////////////////////////////////////////////////////////////
// func: InputFirst
// args: void *pdata, needed by os
// comm: reads input of the first number and handles it further
//       if a math operator is pressed, go to InputFlagPend
//////////////////////////////////////////////////////////////////////////////
void InputFirst(void *pdata)
{
	INT8U    error;
	OS_FLAGS value = 0;  // OS_FLAGS == INT16U (os_cfg.h)
	INT32U 	 firstNumber = 0;

    while (TRUE)
    {
    	OSTimeDly(3);
    	value = OSFlagPend(FlagHandle,
    			           0xffff, // 4 nibbles = 2 bytes
    					   OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME,
    					   WAIT_FOREVER,
    					   &error);

    	UART_puts("\n\rInputFirst function reached."); UART_puts("\n\r");

        // store KEYS_read() in a variable ->
        // if a math operator is pressed ->
        // go to InputFlagPend(firstNumber, value, pdata);
        if (value == key_pl || value == key_mi || value == key_mu || value == key_bs)
        {
        	UART_puts("\n\rOperator found: "); UART_putint(value); UART_puts("\n\r");
        	oper = value;
        	InputFlagPend(firstNumber, value);
        }

        else // if a key is pressed...
        {
        	// store in 'firstNumber' and '-2', so that e.g. 2 becomes 0 instead of 2 (since comma has value 0, see keys.c)
            firstNumber = 10 * firstNumber + value - 2;
            firNum = firstNumber;
            OSTimeDly(50);
            LCD_putint(value - 2); // print 'firstNumber' on LCD

            UART_puts("\n\rfirstNumber:"); UART_putint(firstNumber); UART_puts("\n\r");
            UART_puts("\n\rPress on an operator (key 13, 14, 15, 16) to continue the calculation!"); UART_puts("\n\r");
        }

        OSTimeDly(100);
    }
}

//////////////////////////////////////////////////////////////////////////////
// func: InputFlagPend
// args: INT16U firstNumber, needed for calling InputSecond
//       OS_FLAGS value, needed to determine what operator is pressed
// comm: contains switch-case for math operator (value)
//////////////////////////////////////////////////////////////////////////////
void InputFlagPend(INT16U firstNumber, OS_FLAGS value)
{
	UART_puts((char *)__func__); UART_puts("started\n\r");

	// switch-case to determine which operator is requested
	switch(value)
	{
		case key_pl:
			LCD_putchar('+');
			UART_puts("\n\r"); UART_puts("PLUS");
			break;
		case key_mi:
			LCD_putchar('-');
			UART_puts("\n\r"); UART_puts("MINUS");
			break;
		case key_mu:
			LCD_putchar('*');
			UART_puts("\n\r"); UART_puts("MULTIPLICATION");
			break;
		case key_bs:
			LCD_putchar('/');
			UART_puts("\n\r"); UART_puts("DIVISION");
			break;
		default:
			// empty...
			break;
	}
	OSTimeDly(100);
	InputSecond(firstNumber, value);
}


void InputSecond(INT16U firstNumber, OS_FLAGS value)
{
	INT16U secondNumber = 0;
	INT16U valueTwo = 0;
	INT16U result = 0;

	UART_puts("\n\rInputSecond function reached."); UART_puts("\n\r");
	UART_puts("operator:"); UART_putint(value);

	while(TRUE)
	{
		valueTwo = KEYS_read();

		if (valueTwo == key_en)
		{
			result = firstNumber + secondNumber;
			res = result;
			UART_puts("\n\r"); UART_puts("Result: "); UART_putint(result);

			OSTimeDly(50); LCD_clear(); OSTimeDly(50); LCD_putint(result);
		}

		else if (valueTwo) // if a key is pressed...
		{
			secondNumber = 10 * secondNumber + valueTwo - 2; // store it in 'secondNumber'
			secNum = secondNumber;
			OSTimeDly(50);
			LCD_putint(valueTwo - 2);

			UART_puts("\n\rsecondNumber:"); UART_putint(secondNumber); UART_puts("\n\r");
		}
	}
}

