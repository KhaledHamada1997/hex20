/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "STD_Types.h"
#include "util.h"
#include "RCC_int.h"
#include "DIO_int.h"
#include "NVIC_int.h"
#include "FLASH_int.h"
#include "USART_int.h"
#include "HexDataProcessor_int.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void vidUSARTInterrupt(u16 u16Received);


u8 u8CharCount = 0;
u8 u8Received = 0;
u8 flag = 0;

int
main(int argc, char* argv[])
{
	// At this stage the system clock should have already been configured
	// at high speed.

//	RCC_vidInit();
//	void (*pfunc)(void) = 0;
//	pfunc = *(u32*)(0x08010004);
//	pfunc();


	HexData strHexData[55];
	u8 HexArrayLine[261] = {0};
	u8 u8Counter  = 0;
	u16 u16Counter = 0;
	u32 address = 0;

	void (*pfunc)(void) = 0;

	RCC_vidInit();
	RCC_vidEnablePeripheral(RCC_u8GPIOACLK);
	RCC_vidEnablePeripheral(RCC_u8USART1CLK);
	DIO_vidInit();
	USART_enumInit(USART_CHANNEL_1);
	NVIC_vidEnableInterrupt(NVIC_u8USART1);
	USART_enumSetCallback(USART_CHANNEL_1, vidUSARTInterrupt);
	//	u16 i = 0;
	//
	//	volatile u32 address = 0;
	for ( u8Counter = 64; u8Counter < 128; u8Counter++)
	{
		FLASH_vidErasePage(u8Counter);
	}
	//	for (i = 0 ; i < 5; i++)
	//	{
	//		HexDataProcessor_vidGetHexData(apu8HexArrays[i], &(strHexData[i]));
	//	}
	//	address = HexDataProcessor_u32StoreHexInFlash(strHexData,5);


	//	FLASH_vidErasePage(127);
//	u8 arr2[] = ":020000040801F1\n";
//	 u8 arr1[] = ":100000000050002021020108270201082B020108EC\n";
//
//		HexDataProcessor_vidGetHexData(arr2, &(strHexData[0]));
//		HexDataProcessor_vidGetHexData(arr1, &(strHexData[1]));
//
//		HexDataProcessor_u32StoreHexInFlash(strHexData,2);
	// Infinite loop
	while (1)
	{
		do
		{
			u8Counter = 0;
			do
			{
				do
				{
					while (flag == 0);
					flag = 0;
					HexArrayLine[u8CharCount] = u8Received;
					u8CharCount++;
				}while (u8Received != '\n');
				u8CharCount  = 0;
				HexDataProcessor_vidGetHexData(HexArrayLine, &(strHexData[u8Counter]));
				u8Counter++;
				if (u8Counter != 55)
				{
					USART_voidSendChar(USART_CHANNEL_1,'a');
				}
			}while ( (u8Counter != 55) && (strHexData[u8Counter - 1].enuDataRecord != EndOfLine) );

			address = HexDataProcessor_u32StoreHexInFlash(strHexData,55);
			USART_voidSendChar(USART_CHANNEL_1,'a');
		} while (address == 0);
//		address--;
//		address = *(u32*)(0x08010004);
//		pfunc = (u32*)address;
		pfunc = *(u32*)(0x08010004);
		pfunc();
		//	  arr0[0] = arr0[1];
		// Add your code here.
	}
}

void vidUSARTInterrupt(u16 u16Received)
{
	u8Received = (u8)USART_u16Receive(USART_CHANNEL_1);
	if (u8Received != 0)
	{
		flag = 1;
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
