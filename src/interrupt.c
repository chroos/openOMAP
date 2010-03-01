#include "interrupt.h"
#include "omapl138.h"
#include "omapl138_uart.h"
#include "omapl138_timer.h"

unsigned char opCode[] = {0x04,0xF0,0x1F,0xE5}; //ARM OpCode to read next word as address and jump to it.
VTABLE ISR_VectorTable[101];					//ISR Table

void FIQ_HANDLER(void)				//ISR TEST-ROUTINE
{
	UART_txString(UART2, "FIQ\r\n");
	UART_txString(UART2, "HIPVR0,1:\t");
	UART_txUint32(UART2, AINTC->HIPVR[0]);UART_txString(UART2, " ");
	UART_txUint32(UART2, AINTC->HIPVR[1]);UART_txString(UART2, "\t");
	UART_txString(UART2, "HIPIR0,1:\t");
	UART_txUint32(UART2, AINTC->HIPIR[0]);UART_txString(UART2, " ");
	UART_txUint32(UART2, AINTC->HIPIR[1]);UART_txString(UART2, "\r\n");
}
void IRQ_HANDLER(void)				//ISR TEST-ROUTINE
{
	UART_txString(UART2, "IRQ\r\n");
	UART_txString(UART2, "HIPVR0,1:\t");
	UART_txUint32(UART2, AINTC->HIPVR[0]);UART_txString(UART2, " ");
	UART_txUint32(UART2, AINTC->HIPVR[1]);UART_txString(UART2, "\r\n");
	UART_txUint32(UART2, AINTC->HIPIR[0]);UART_txString(UART2, " ");
	UART_txUint32(UART2, AINTC->HIPIR[1]);UART_txString(UART2, "\r\n");
//	TMR2->INTCTLSTAT |= 2;
//	AINTC->SICR = 24; 
}

void ISR_undefined(void){ }
void RESET_HANDLER(void){UART_txString(UART2, "Resetting Device\r\n"); }
void UNDEFINED_HANDLER(void){UART_txString(UART2, "ERROR: UNDEFINED INSTRUCTION\r\n"); }
void SWI_HANDLER(void){UART_txString(UART2, "ERROR: SWI EXCEPTION\r\n"); }
void PREFETCH_HANDLER(void){UART_txString(UART2, "ERROR: PREFETCH EXCEPTION\r\n"); }
void ABORT_HANDLER(void){UART_txString(UART2, "ERROR: ABORT EXCEPTION\r\n"); }

//--------------------------------------------------------------------------------------------->
// init
//--------------------------------------------------------------------------------------------->

void init_Interrupt(void)
{
	AINTC->HIER = 0;  				// disable IRQ & FIQ
	AINTC->GER = 0x0;

	asm(" STMDB	SP!, {R0}"); 		/* Push R0.						*/ //Disable IRQ
	asm(" MRS	R0, cpsr");			/* Get CPSR.					*/
	asm(" ORR	R0, R0, #0xC0");	/* Disable IRQ, FIQ.			*/
	asm(" MSR	cpsr_c, R0");		/* Write back modified value.	*/
	asm(" MRS	R0, cpsr");
	asm(" LDMIA	SP!, {R0}");		/* Pop R0.						*/
//-----------------------------------------------------------------------------
	AINTC->ECR[0] = 0xffffffff;				// disable all system interrupts
	AINTC->ECR[1] = 0xffffffff;
	AINTC->ECR[2] = 0xffffffff;
	AINTC->SECR[0] = 0xffffffff;			// clear all system interrupts
	AINTC->SECR[1] = 0xffffffff;
	AINTC->SECR[2] = 0xffffffff;

	AINTC->CR = 0x4;						// Nesting mode auto individual
//-----------------------------------------------------------------------------
	asm(" STMDB	SP!, {R0}");				// Push R0.						//Enable IRQ
	asm(" MRS	R0, cpsr");					// Get CPSR.
	asm(" BIC	R0, R0, #0xC0");			// Enable IRQ, + FIQ?.
	asm(" MSR	cpsr_c, R0");				// Write back modified value.
	asm(" MRS	R0, cpsr");					// Get CPSR.
	asm(" LDMIA	SP!, {R0}");

  	AINTC->GER = 0x01;						// Enable interrupts
	AINTC->HIER = 0x03;						// enable IRQ interrupts for host interrupt

}
//--------------------------------------------------------------------------------------------->
// ISR VTABLE
//--------------------------------------------------------------------------------------------->
void IRQ_init_VTable(void)
{
	int i;						//incremental counter

	for(i=0;i<101;i++)			//give all interrupts the standart vector
	{	
		memcpy(ISR_VectorTable[i].code, opCode, 4);				// Opcode
		ISR_VectorTable[i].function = (uint32_t)ISR_undefined;	// Standart ISR
	}

	AINTC->VBR = (uint32_t)&ISR_VectorTable; 	//Set the Starting address of the vector table
	AINTC->VNR = (uint32_t)&ISR_VectorTable; 	//Set the Starting address of the vector table
	AINTC->VSR = 1;								//set the size to 8 byte
}
void IRQ_mapVTable(uint32_t offset, uint32_t function)
{
	ISR_VectorTable[offset].function = function;		// Map ISR	
}
//--------------------------------------------------------------------------------------------->
// m_func
//--------------------------------------------------------------------------------------------->
void enableInterrupt(uint32_t offset)
{
	AINTC->SICR = offset; 		 			// clear interrupt
	AINTC->EICR = offset;					// At ARM level
	AINTC->EISR = offset; 					// enable Interrupt
}

void INT_Channel_map(uint32_t ch_number, uint32_t offset, uint32_t INT_number)
{
	uint32_t temp_CMR = AINTC->CMR[ch_number];		//Copy Channel Map Register
													
	if		(offset==0){temp_CMR &= 0xFFFFFF00;}	// Bitmask for Offset 0
	else if	(offset==1){temp_CMR &= 0xFFFF00FF;}	// 1
	else if	(offset==2){temp_CMR &= 0xFF00FFFF;}	// 2
	else if	(offset==3){temp_CMR &= 0x00FFFFFF;}	// 3

	AINTC->CMR[ch_number] = (temp_CMR | (INT_number<<(offset*8)) );	//Map new Channel
}



