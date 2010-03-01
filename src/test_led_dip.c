//-----------------------------------------------------------------------------
// \file    test_led_dip_pb.c
// \brief   implementation of OMAP-L138 leds/dip switch/pushbutton test.
//
//-----------------------------------------------------------------------------

#include "stdio.h"
#include "types.h"
#include "omapl138.h"
#include "omapl138_timer.h"
#include "omapl138_led.h"
#include "omapl138_dip.h"
#include "omapl138_uart.h"
#include "test_led_dip.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------

#define DIP_MASK_LED_1     (0x0000000F)
#define DIP_MASK_LED_2     (0x000000F0)

//-----------------------------------------------------------------------------
// Static Variable Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   tests the leds and dip switches. lights each led.
//          prints message to stdout for dip changes.
//          lights leds based on dip switches.
//
// \param   none.
//
// \return  uint32_t
//-----------------------------------------------------------------------------
uint32_t TEST_ledDip(void)
{
   uint32_t rtn;
   uint32_t i;
   uint8_t tmp_val;
   uint32_t last_dip_state = 0;

   //-------------------------------------
   // initialize the required bsl modules.
   //-------------------------------------

   rtn = LED_init();
   if (rtn != ERR_NO_ERROR) return (rtn);
      
   rtn = DIP_init();
   if (rtn != ERR_NO_ERROR) return (rtn);
   
   //--------------
   // execute test.
   //--------------

   // turn led 0 on and wait 1 second.
   LED_turnOn(LED_1);
   USTIMER_delay(DELAY_1_SEC);
   
   // turn led 1 on and wait 1 second.
   LED_turnOn(LED_2);
   USTIMER_delay(DELAY_1_SEC);
   
   // flash leds for a few seconds.
   for (i = 0; i < 5; i++)
   {
      LED_toggle(LED_1);
      LED_toggle(LED_2);
      USTIMER_delay(DELAY_HALF_SEC);
   }
   
   while (1)
   {
      // check the dip switches and print status changes.
      for (i = 0; i < MAX_DIP_NUM; i++)
      {
         rtn = DIP_get(i, &tmp_val);
         if (rtn != ERR_NO_ERROR)
            return (rtn);

         if (tmp_val)
         {
            if (!CHKBIT(last_dip_state, bitval_u32[i]))
            {
     //          printf("DIP switch[%u] was switched on\r\n\r\n", i + 1);
            }
            SETBIT(last_dip_state, bitval_u32[i]);
         }
         else
         {
            if (CHKBIT(last_dip_state, bitval_u32[i]))
            {
       //        printf("DIP switch[%u] was switched off\r\n\r\n", i + 1);
            }
            CLRBIT(last_dip_state, bitval_u32[i]);
         }
      }
      
      // set leds based on latest dip status.
      if (CHKBIT(last_dip_state, DIP_MASK_LED_1))
         LED_turnOn(LED_1);
      else
         LED_turnOff(LED_1);
         
      if (CHKBIT(last_dip_state, DIP_MASK_LED_2))
         LED_turnOn(LED_2);
      else
         LED_turnOff(LED_2);
   }

   // currently can't reach this due to the forever loop.
//    return (rtn);
}

//-----------------------------------------------------------------------------
// Private Function Definitions
//-----------------------------------------------------------------------------

