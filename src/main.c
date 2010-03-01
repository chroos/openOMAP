/*
	This is my first testproject to develop GPIO functionality
	on OMAP-l138's ARM Core

	The Main() Routine gets called by the starter script from boot.c
*/

//Standard Headers
#include "stdio.h"					//Standard header
#include "types.h"					//Datatype definitions

//Library Headers
#include "omapl138.h"
#include "omapl138_timer.h"
#include "omapl138_i2c.h"
#include "omapl138_uart.h"
#include "interrupt.h"

//Applikation headers
#include "main.h"					//This Files Header
#include "test_led_dip.h"			//The Application Header

void getAddresses(interrupt_regs_t* IRQx)
{
	UART_txString(UART2, "AINTC REGISTERS ------------------------------------------------------------------->\r\n");
	UART_txString(UART2, "CR:\t");		UART_txUint32(UART2, (uint32_t)IRQx->CR);		UART_txString(UART2, "\t\t");	//0x0004
	UART_txString(UART2, "EICR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->EICR);		UART_txString(UART2, "\t\t");	//0x002C
	UART_txString(UART2, "GPIR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->GPIR);		UART_txString(UART2, "\t\t");	//0x0080	
	UART_txString(UART2, "CMR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->CMR[0]);	UART_txString(UART2, "\r\n");	//0x0400
	UART_txString(UART2, "GER:\t");		UART_txUint32(UART2, (uint32_t)IRQx->GER);		UART_txString(UART2, "\t\t");		//0x0010
	UART_txString(UART2, "HIEISR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIEISR);	UART_txString(UART2, "\t\t");	//0x0034
	UART_txString(UART2, "GPVR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->GPVR);		UART_txString(UART2, "\t\t");	//0x0084
	UART_txString(UART2, "HIPIR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIPIR[0]);	UART_txString(UART2, "\r\n");	//0x0900
	UART_txString(UART2, "GNLR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->GNLR);		UART_txString(UART2, "\t\t");	//0x001C
	UART_txString(UART2, "HIDISR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIDISR);	UART_txString(UART2, "\t\t");	//0x0038
	UART_txString(UART2, "SRSR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->SRSR[0]);	UART_txString(UART2, "\t\t");	//0x0200
	UART_txString(UART2, "DSR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->DSR[0]);	UART_txString(UART2, "\r\n");	//0x0F00
	UART_txString(UART2, "SISR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->SISR);		UART_txString(UART2, "\t\t");	//0x0020
	UART_txString(UART2, "VBR:\t");		UART_txUint32(UART2, (uint32_t)IRQx->VBR);		UART_txString(UART2, "\t\t");		//0x0050
	UART_txString(UART2, "SECR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->SECR[0]);	UART_txString(UART2, "\t\t");	//0x0280
	UART_txString(UART2, "HINLR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HINLR[0]);	UART_txString(UART2, "\r\n");	//0xF100
	UART_txString(UART2, "SICR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->SICR);		UART_txString(UART2, "\t\t");	//0x0024	
	UART_txString(UART2, "VSR:\t");		UART_txUint32(UART2, (uint32_t)IRQx->VSR);		UART_txString(UART2, "\t\t");		//0x0054
	UART_txString(UART2, "ESR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->ESR[0]);	UART_txString(UART2, "\t\t");	//0x0300
	UART_txString(UART2, "HIER:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIER);		UART_txString(UART2, "\r\n");	//0xF500
	UART_txString(UART2, "EISR:\t");	UART_txUint32(UART2, (uint32_t)IRQx->EISR);		UART_txString(UART2, "\t\t");	//0x0028
	UART_txString(UART2, "VNR:\t");		UART_txUint32(UART2, (uint32_t)IRQx->VNR);		UART_txString(UART2, "\t\t");		//0x0058
	UART_txString(UART2, "ECR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->ECR[0]);	UART_txString(UART2, "\t\t");	//0x0380
	UART_txString(UART2, "HIPVR0:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIPVR[0]);	UART_txString(UART2, "\r\n");	//0xF600
	UART_txString(UART2, "HIPVR1:\t");	UART_txUint32(UART2, (uint32_t)IRQx->HIPVR[1]);	UART_txString(UART2, "\r\n");	//0xF600
	UART_txString(UART2, "AINTC END ------------------------------------------------------------------------->\r\n");
}

void m_init(void)
{
	EVMOMAPL138_init();				 // configure power, sysconifg, and clocks
	EVMOMAPL138_initRAM();

	IRQ_init_VTable();
	init_Interrupt();
	IRQ_mapVTable(T64P1_TINT34, (uint32_t)FIQ_HANDLER);
	INT_Channel_map(5,0,T64P1_TINT34);
	enableInterrupt(T64P1_TINT34);

	USTIMER_init();					//init the us timer
	I2C_init(I2C0, I2C_CLK_400K);	//init I2C
	
	UART_init(UART2, 115200);		//UART2 for RS232 Debuging 	
	UART_txString(UART2, "Init Done.\r\n");
	UART_txString(UART2, "Power up DSP now..");
//	EVMOMAPL138_enableDsp();		//Wake up DSP Core
	UART_txString(UART2, " done..\r\n");
}

void Timer_Init(void)
{
	TIM_TimerBaseInitTypedef TIM_IN;
	
	TIM_IN.TIM_Timermode = TIMMODE_64BIT_;
	TIM_IN.TIM_Period34 = (300000000-1);
	TIM_IN.TIM_ENAMODE12 = ENAMODE12_CONT;
	TIMER_init(TMR2, &TIM_IN);
}

// Main entry point
void main(void)
{		
	m_init(); UART_txString(UART2, "Welcome\r\n");
		
	getAddresses(AINTC);
//	Timer_Init();
	
	while(1)
	{	
		USTIMER_delay(1000000);
		UART_txString(UART2, "HIPVR0:\t");	UART_txUint32(UART2, (uint32_t)AINTC->HIPVR[0]);	UART_txString(UART2, "\r\n");	//0xF600
		UART_txString(UART2, "HIPVR1:\t");	UART_txUint32(UART2, (uint32_t)AINTC->HIPVR[1]);	UART_txString(UART2, "\r\n");	//0xF604
		UART_txString(UART2, "HIPVR0:\t");	UART_txUint32(UART2, (uint32_t)AINTC->SRSR[0]);	UART_txString(UART2, "\r\n");	//0xF600
		getAddresses(AINTC);
	}

	TEST_ledDip();

	while(1) {}
}

/*
	End file
*/

