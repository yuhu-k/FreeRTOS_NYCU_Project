/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timer_a.h"
#include "inc/hw_memmap.h"

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the MSP430X port.
 *----------------------------------------------------------*/

/* Constants required for hardware setup.  The tick ISR runs off the ACLK,
not the MCLK. */
#define portACLK_FREQUENCY_HZ			( ( TickType_t ) 32768 )
#define portINITIAL_CRITICAL_NESTING	( ( uint16_t ) 10 )
#define portFLAGS_INT_ENABLED			( ( StackType_t ) 0x08 )

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

/* Each task maintains a count of the critical section nesting depth.  Each
time a critical section is entered the count is incremented.  Each time a
critical section is exited the count is decremented - with interrupts only
being re-enabled if the count is zero.

usCriticalNesting will get set to zero when the scheduler starts, but must
not be initialised to zero as this will cause problems during the startup
sequence. */
volatile uint16_t usCriticalNesting = portINITIAL_CRITICAL_NESTING;
/*-----------------------------------------------------------*/


/*
 * Sets up the periodic ISR used for the RTOS tick.  This uses timer 0, but
 * could have alternatively used the watchdog timer or timer 1.
 */
void vPortSetupTimerInterrupt( void );
/*-----------------------------------------------------------*/

/*
 * Initialise the stack of a task to look exactly as if a call to
 * portSAVE_CONTEXT had been called.
 *
 * See the header file portable.h.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
uint16_t *pusTopOfStack;
uint32_t *pulTopOfStack, ulTemp;

	/*
		Place a few bytes of known values on the bottom of the stack.
		This is just useful for debugging and can be included if required.

		*pxTopOfStack = ( StackType_t ) 0x1111;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x2222;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x3333;
		pxTopOfStack--;
	*/

	/* Data types are need either 16 bits or 32 bits depending on the data 
	and code model used. */
	if( sizeof( pxCode ) == sizeof( uint16_t ) )
	{
		pusTopOfStack = ( uint16_t * ) pxTopOfStack;
		ulTemp = ( uint32_t ) pxCode;
		*pusTopOfStack = ( uint16_t ) ulTemp;
	}
	else
	{
		/* Make room for a 20 bit value stored as a 32 bit value. */
		pusTopOfStack = ( uint16_t * ) pxTopOfStack;		
		pusTopOfStack--;
		pulTopOfStack = ( uint32_t * ) pusTopOfStack;
		*pulTopOfStack = ( uint32_t ) pxCode;
	}

	pusTopOfStack--;
	*pusTopOfStack = portFLAGS_INT_ENABLED;
	pusTopOfStack -= ( sizeof( StackType_t ) / 2 );
	
	/* From here on the size of stacked items depends on the memory model. */
	pxTopOfStack = ( StackType_t * ) pusTopOfStack;

	/* Next the general purpose registers. */
	#ifdef PRELOAD_REGISTER_VALUES
		*pxTopOfStack = ( StackType_t ) 0xffff;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0xeeee;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0xdddd;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) pvParameters;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0xbbbb;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0xaaaa;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x9999;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x8888;
		pxTopOfStack--;	
		*pxTopOfStack = ( StackType_t ) 0x5555;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x6666;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x5555;
		pxTopOfStack--;
		*pxTopOfStack = ( StackType_t ) 0x4444;
		pxTopOfStack--;
	#else
		pxTopOfStack -= 3;
		*pxTopOfStack = ( StackType_t ) pvParameters;
		pxTopOfStack -= 9;
	#endif

	/* A variable is used to keep track of the critical section nesting.
	This variable has to be stored as part of the task context and is
	initially set to zero. */
	*pxTopOfStack = ( StackType_t ) portNO_CRITICAL_SECTION_NESTING;	

	/* Return a pointer to the top of the stack we have generated so this can
	be stored in the task control block for the task. */
	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the MSP430 port will get stopped.  If required simply
	disable the tick interrupt here. */
}
/*-----------------------------------------------------------*/

/*
 * Hardware initialisation to generate the RTOS tick.
 */
void vPortSetupTimerInterrupt( void )
{
	vApplicationSetupTimerInterrupt();
}
/*-----------------------------------------------------------*/
#include <stdio.h>
#pragma vector=configTICK_VECTOR
interrupt void vTickISREntry( void )
{
extern void vPortTickISR( void );

	__bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
	#if configUSE_PREEMPTION == 1
		extern void vPortPreemptiveTickISR( void );
		vPortPreemptiveTickISR();
	#else
		extern void vPortCooperativeTickISR( void );
		vPortCooperativeTickISR();
	#endif
}

// check point
#include "portable.h"
#include "cs.h"
#include "dma.h"

extern uint32_t sp_buffer1[ 1 ];
extern uint32_t sp_buffer2[ 1 ];
extern uint32_t pc_buffer1[ 1 ];
extern uint32_t pc_buffer2[ 1 ];
extern uint8_t ram_buffer[ 0x1000 ];
extern uint8_t heap_buffer[ configTOTAL_HEAP_SIZE ];
extern uint8_t ram_buffer2[ 0x1000 ];
extern uint8_t heap_buffer2[ configTOTAL_HEAP_SIZE ];
uint32_t* sp_buffer_addr_for_asm[ 1 ];
uint32_t* pc_buffer_addr_for_asm[ 1 ];


void vPortCheckPointRestore(){
    if(sp_buffer1[0] != 0 || sp_buffer2[0] != 0){
        printf("Found backuped data, restoring...\n");
        if(sp_buffer1[0]){
            DMA_setSrcAddress(DMA_CHANNEL_2,(uint32_t)ram_buffer,DMA_DIRECTION_INCREMENT);
            DMA_setSrcAddress(DMA_CHANNEL_3,(uint32_t)heap_buffer,DMA_DIRECTION_INCREMENT);
            *sp_buffer_addr_for_asm = sp_buffer1;
            *pc_buffer_addr_for_asm = pc_buffer1;
        }else{
            DMA_setSrcAddress(DMA_CHANNEL_2,(uint32_t)ram_buffer2,DMA_DIRECTION_INCREMENT);
            DMA_setSrcAddress(DMA_CHANNEL_3,(uint32_t)heap_buffer2,DMA_DIRECTION_INCREMENT);
            *sp_buffer_addr_for_asm = sp_buffer2;
            *pc_buffer_addr_for_asm = pc_buffer2;
        }
        vPortRestoreASM();
        printf("Error, u shall not pass!!\n");
        for(;;);
    }
    printf("No backuped data found\n");
}

void vPortBackup(){
    if(sp_buffer1[0] == 0) {
        DMA_setDstAddress(DMA_CHANNEL_0,(uint32_t)heap_buffer,DMA_DIRECTION_INCREMENT);
        DMA_setDstAddress(DMA_CHANNEL_1,(uint32_t)ram_buffer,DMA_DIRECTION_INCREMENT);
        *sp_buffer_addr_for_asm = sp_buffer1;
        *pc_buffer_addr_for_asm = pc_buffer1;
        vPortBackupASM();
        sp_buffer2[0] = 0;
    }else{
        DMA_setDstAddress(DMA_CHANNEL_0,(uint32_t)heap_buffer2,DMA_DIRECTION_INCREMENT);
        DMA_setDstAddress(DMA_CHANNEL_1,(uint32_t)ram_buffer2,DMA_DIRECTION_INCREMENT);
        *sp_buffer_addr_for_asm = sp_buffer2;
        *pc_buffer_addr_for_asm = pc_buffer2;
        vPortBackupASM();
        sp_buffer1[0] = 0;
    }

}

// timer_a a2 setting. Be used to get the time on processing checkpoint
uint16_t start_time = 0, end_time = 0;
uint32_t total_time;
bool TimerStatus = false;
uint16_t base_address;
void vStartTimerA(){
    total_time = 0;
    Timer_A_clear(base_address);
    Timer_A_startCounter(base_address,TIMER_A_CONTINUOUS_MODE);
    TimerStatus = true;
    start_time = Timer_A_getCounterValue(base_address);
}


void vEndTimerA(){
    if(TimerStatus){
        end_time = Timer_A_getCounterValue(base_address);
        Timer_A_stop(base_address);
        Timer_A_clear(base_address);
        TimerStatus = false;
    }
}

uint32_t vGetProcessTime(){
    if(TimerStatus) end_time = Timer_A_getCounterValue(base_address);
    return (total_time + ( end_time - start_time ));
}


void SetBaseAddress(uint16_t BaseAddress){
    base_address = BaseAddress;
}

#pragma vector=TIMER4_A1_VECTOR
__interrupt void vTimerA2Overflow( void )
{
    __bic_SR_register_on_exit( SCG1 + SCG0 + OSCOFF + CPUOFF );
    total_time += 0x10000;
    TA4CTL &= ~TAIFG;
}

