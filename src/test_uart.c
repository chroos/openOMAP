//-----------------------------------------------------------------------------
// \file    test_uart.c
// \brief   implementation of OMAP-L138 uart test.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "omapl138.h"
#include "omapl138_timer.h"
#include "omapl138_uart.h"
#include "test_uart.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------
static uint32_t terminal_counting(void);

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   tests the us timer and uart. counts to 1...10, printing messages
//          out the debug port along the way.
//
// \param   none.
//
// \return  uint32_t
//-----------------------------------------------------------------------------
uint32_t TEST_uart(void)
{
   uint32_t rtn;

   printf("------------------------------------------------------------\r\n");
   printf("                     OMAP-L138 UART Test\r\n\r\n");

   printf("Additional Equipment\r\n");
	printf("--------------------\r\n");
	printf("- RS232 serial cable \r\n\r\n");

   printf("Test Description\r\n");
   printf("----------------\r\n");
   printf("Connect the serial cable from the board to a computer and \r\n");
   printf("open a terminal program (Tera Term), configuring it for  \r\n");
   printf("115200 baud, 8-bit, no parity, 1 stop bit, no flow control.  \r\n");
   printf("This code will configure the UART and use a microsecond timer \r\n");
   printf("to count to 10, printing each second.  \r\n");
   printf("and write a portion of SPI flash. \r\n");
   printf("------------------------------------------------------------\r\n\r\n");

   //-------------------------------------
   // initialize the required bsl modules.
   //-------------------------------------
   printf("Initialize the Required BSL Modules\r\n");
   printf("-----------------------------------\r\n\r\n");
   
   rtn = UART_init(UART2, 115200);
   if (rtn != ERR_NO_ERROR)
   {
      printf("error initializing uart!\r\n");
      return (rtn);
   }
   
   //--------------
   // execute test.
   //--------------
   printf("\r\nExecute Test\r\n");
   printf("------------\r\n\r\n");

   // print to a terminal...must have EVM connected to a PC.
   rtn = terminal_counting();
   
   return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// counts/prints 0 to 10 to the debug port
// designed to be run with an RS-232 cable connecting the EVM to a
// PC with a terminal program open to view output.
//-----------------------------------------------------------------------------
uint32_t terminal_counting(void)
{
   uint32_t rtn;
   uint32_t i;

   printf("--- open terminal (hyperterminal/tera term) window to view debug messages ---\r\n");

   UART_txString(UART2, "start counting...\r\n\r\n");

   for (i = 0; i < 11; i++)
   {
      // delay 1 second.
      USTIMER_delay(1000000);

      // print count out debug port.
      UART_txUint32(DEBUG_PORT, i);
      UART_txString(DEBUG_PORT, "\t");
      UART_txUint32(DEBUG_PORT, USTIMER_get());
      UART_txString(DEBUG_PORT, "\r\n");
   }

   UART_txString(DEBUG_PORT, "\n\rfinish time:\t");
   UART_txUint32(DEBUG_PORT, USTIMER_get());
   rtn = UART_txString(DEBUG_PORT, "\r\n");
   
   return (rtn);
}

