// This module's header file 
#include "boot.h"

extern void main(void);

// Boot entry point to setup the C environment
  #pragma TASK(boot);
  #pragma NO_HOOKS(boot);
  #pragma CODE_SECTION(boot,".boot");
void boot(void)
{
  asm(" .global STACKStart");
  asm(" .global _stack");
  asm(" .global main");  
	asm(" NOP");
  asm(" MRS  r0, cpsr");
  asm(" BIC  r0, r0, #0x1F");       // CLEAR MODES
  asm(" ORR  r0, r0, #0x13");       // SET SUPERVISOR mode
 // asm(" ORR  r0, r0, #0xC0");       // Disable FIQ and IRQ
  asm(" MSR  cpsr, r0");
  asm(" NOP");
  
  // Set the IVT to low memory, leave MMU & caches disabled
  asm(" MRC  p15,#0,r0,c1,c0,#0");
  asm(" BIC  r0,r0,#0x00002300");
  asm(" BIC  r0,r0,#0x00000087");
  asm(" ORR  r0,r0,#0x00000002");
  asm(" ORR  r0,r0,#0x00001000");  
  asm(" MCR  p15,#0,r0,c1,c0,#0");
  asm(" NOP");
  
  // Setup the stack pointer
  asm(" LDR  sp,_stack");
  asm(" SUB  sp,sp,#4");
  asm(" BIC  sp, sp, #7");
  
  // Call to main entry point
  main();
  
  asm("_stack:");
  asm(" .word STACKStart");
}

/************************************************************
* End file                                                  *
************************************************************/
