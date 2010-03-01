//-----------------------------------------------------------------------------
// \file    omapl138_timer.c
// \brief   implementation of a timer driver for OMAP-L138.
//
//-----------------------------------------------------------------------------

#include "types.h"
#include "omapl138.h"
#include "omapl138_timer.h"

//-----------------------------------------------------------------------------
// Private Defines and Macros
//-----------------------------------------------------------------------------
#define TIMER_DIV       (12)
#define TICKS_PER_US    (2)

//-----------------------------------------------------------------------------
// Public Function Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// \brief   initialize the microsecond timer.
//
// \param   none.
//
// \return  uint32_t
//    ERR_NO_ERROR - everything is ok...us timer ready to use.
//    ERR_INIT_FAIL - something happened during initialization.
//-----------------------------------------------------------------------------
uint32_t USTIMER_init(void)
{
	// configure timer 0 for free run.
	// 32-bit unchained mode, timer3:4 /12 prescaler.

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR0->GPINT_GPEN = GPENO12 | GPENI12;
   TMR0->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR0->TGCR = 0x00000000;
   TMR0->TCR = 0x00000000;

   // enable interrupts and set emulation to free run.
   
   SETBIT(TMR0->EMUMGT, SOFT | FREE);

   // config timer0 in 32-bit unchained mode.
   // remove timer0 - 3:4 from reset.
   SETBIT(TMR0->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED | TIM34RS );

   // init timer0 - 1:2 period....use full range of counter.
   TMR0->TIM34 = 0x00000000;
   TMR0->PRD34 = 0xFFFFFFFF;
	TMR0->INTCTLSTAT = 1;
   // start timer0 - 3:4.
   SETBIT(TMR0->TCR, ENAMODE34_CONT);

   // configure timer 1 for start/stop.
	// 32-bit unchained mode, timer3:4 /12 prescaler.

	// enable muxed pins as gpio outputs and disable all related interrupts.
	// would need to also setup the pinmux register to select the gpio
	// function of these pins in order to use as gpio.
   TMR1->GPINT_GPEN = GPENO12 | GPENI12;
   TMR1->GPDATA_GPDIR = GPDIRO12 | GPDIRI12;

   // stop and reset timer.
   TMR1->TGCR = 0x00000000;
   TMR1->TCR = 0x00000000;

   // enable interrupts and set emulation to free run.
   TMR1->INTCTLSTAT = 1;
   SETBIT(TMR1->EMUMGT, SOFT | FREE);

   // config timer1 in 32-bit unchained mode.
   SETBIT(TMR1->TGCR, PRESCALER(TIMER_DIV - 1) | TIMMODE_32BIT_UNCHAINED);

   // init timer1 - 3:4 period....0 until someone calls delay().
   TMR1->TIM34 = 0x00000000;
   TMR1->PRD34 = 0x00000000;
   
   return (ERR_NO_ERROR);
}

//-----------------------------------------------------------------------------
// \brief   halts execution for "in_delay" number of microseconds.
//
// \param   uint32_t in_delay - the number of us to delay.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_delay(uint32_t in_delay)
{
   // stop the timer, clear int stat, and clear timer value.
   CLRBIT(TMR1->TGCR, TIM34RS);
   TMR1->TCR = 0x00000000;
   SETBIT(TMR1->INTCTLSTAT, PRDINTSTAT34);
   TMR1->TIM34 = 0x00000000;

   // setup compare time.
   // NOTE: not checking for possible rollover here...do not pass in a
   // value > 0x7FFFFFFF....would result in a much shorter delay than expected.
   TMR1->PRD34 = TICKS_PER_US * in_delay;
   
   // start timer1 - 3:4 to run once up to the period.
   SETBIT(TMR1->TCR, ENAMODE34_ONETIME);
   SETBIT(TMR1->TGCR, TIM34RS);
   
   // wait for the signal that we have hit our period.
   while (!CHKBIT(TMR1->INTCTLSTAT, PRDINTSTAT34))
   {
      asm("nop");
   }
//   uint32_t i;
//   for (i = 0; i < in_delay * 1; i++) {}
}

//-----------------------------------------------------------------------------
// \brief   resets the timer to 0.
//
// \param   none.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_reset(void)
{
   CLRBIT(TMR0->TGCR, TIM34RS);
   TMR0->TIM34 = 0x00000000;
   SETBIT(TMR0->TGCR, TIM34RS);
}

//-----------------------------------------------------------------------------
// \brief   returns the current timer value, in microseconds.
//
// \param   none.
//
// \return  uint32_t - timer value, in us.
//-----------------------------------------------------------------------------
uint32_t USTIMER_get(void)
{
   return (TMR0->TIM34 / TICKS_PER_US);
}

//-----------------------------------------------------------------------------
// \brief   sets the timer value, in microseconds.
//
// \param   uint32_t in_time - value to set the timer, in us.
//
// \return  none.
//-----------------------------------------------------------------------------
void USTIMER_set(uint32_t in_time)
{
   TMR0->TIM34 = TICKS_PER_US * in_time;
}


/////////////////////////////////////////////////////////////////////////////////7

uint32_t TIMER_init(timer_regs_t * TMRx, TIM_TimerBaseInitTypedef* TMR_Struct)
{
	TMRx->TGCR = 0x00000000;
	TMRx->TCR = 0x00000000;
	//TMRx->TCR |= TMR_Struct->TIM_Clock; //SET Timer Clock (Internal or External)

	TMRx->TGCR |= TMR_Struct->TIM_Timermode; //Select Timer Mode
		
	if(TMR_Struct->TIM_Timermode == TIMMODE_64BIT_)
	{
		TMRx->TGCR |= TIM12RS_OPERATE | TIM34RS_OPERATE;//Remove timer from reset
		TMRx->PRD12 |= TMR_Struct->TIM_Period12;		//Select the desired timer period
		TMRx->PRD34 |= TMR_Struct->TIM_Period34;
		TMRx->REL12 |= TMR_Struct->TIM_Period12;		//Select the desired timer reload period
		TMRx->REL34 |= TMR_Struct->TIM_Period34;

		TMRx->GPINT_GPEN = 2;

		TMRx->TIM12 = 0x00000000;						//reset the timer
		TMRx->TCR |= TMR_Struct->TIM_ENAMODE12;			//Enable the Timer
	}
	else if(TMR_Struct->TIM_Timermode == TIMMODE_32BIT_UNCHAINED)
	{
		//TIM12
		TMRx->TGCR |= TIM12RS_OPERATE; 					//Remove timer from reset
		TMRx->PRD12 |= TMR_Struct->TIM_Period12;		//Select the desired timer period
		TMRx->REL12 |= TMR_Struct->TIM_Period12;		//Select the desired timer reload period
		TMRx->TCR |= TMR_Struct->TIM_CLKSRC12;  		//Select the desired clock source
		TMRx->TIM12 = 0x00000000;						//reset timer12
		TMRx->TCR |= TMR_Struct->TIM_ENAMODE12;			//Enable Timer12
		//TIM34
		TMRx->TGCR |= TIM34RS_OPERATE; 					//Remove timer from reset
		TMRx->PRD34 |= TMR_Struct->TIM_Period34;		//Select the desired timer period
		TMRx->REL34 |= TMR_Struct->TIM_Period34;		//Select the desired timer reload period
		TMRx->TCR |= TMR_Struct->TIM_CLKSRC34;  		//Select the desired clock source
		TMRx->TIM34 = 0x00000000;						//reset  timer34
		TMRx->TCR |= TMR_Struct->TIM_ENAMODE34;			//Enable Timer34
	}

	TMRx->INTCTLSTAT |= PRDINTEN12_ENABLE | PRDINTEN34_ENABLE;				//ENABLE_INTERRUPT

	 return (ERR_NO_ERROR);
}
uint32_t getTime(timer_regs_t * TMRx, int thetimer)
{
	if(!thetimer)
		return (TMRx->TIM12);
	else 
		return (TMRx->TIM34);
}

