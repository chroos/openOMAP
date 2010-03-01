-f 0
-c                                         /* LINK USING C CONVENTIONS      */
-stack  0x2000
-heap   0x1000                             /* HEAP AREA SIZE                */

__I_STACK_SIZE = 0x400;		/* IRQ Stack */
__S_STACK_SIZE = 0x400;		/* SUPERVISOR Stack */
__F_STACK_SIZE = 0x400;		/* FIQ Stack */
__U_STACK_SIZE = 0x400;		/* UNDEF Stack */
__Y_STACK_SIZE = 0x400;		/* SYSYEM Stack */
__A_STACK_SIZE = 0x400;		/* ABORT Stack */


/* SPECIFY THE SYSTEM MEMORY MAP */
MEMORY
{

  
  ARM_ROM          :  o = 0xFFFE0000  l = 0x0000FFFF  /* ARM ROM   -  64k */
  ARM_RAM          :  o = 0xFFFF0000  l = 0x000003FF  /* ARM RAM   - 1k */
  
  ENTRY_POINT	   :  o = 0x80000000  l = 0x00000400  /* Entry point for the Test case        40h       */
  L3_RAM		   :  o = 0x80000400  l = 0x00014BC4  /* L3 CBA RAM                           128kB-20KB-40h       */
  L3_RAM_DATA	   :  o = 0x80015000  l = 0x00005000  /* L3 CBA RAM Data                    20kB       */	

  /* SDRAM EMIF3c Memory Region 128 MB  */	
    
	/*SDRAM EMIF3c Data Region Memory Map SIZE 28 MB */
	SDRAMEMIF3c_0     o= 0xC0000000    l= 0x01C00000

	/*DDR EMIF3c Data Region Memory Map SIZE 20 MB */
	SDRAMEMIF3c_1     o= 0xC1C00000    l= 0x01400000

	/*SDRAM EMIF3c Data Region Memory Map SIZE 20 MB */
	SDRAMEMIF3c_2     o= 0xC3000000    l= 0x01400000

	/*SDRAM EMIF3c Data Region Memory Map SIZE 20 MB */
	SDRAMEMIF3c_3     o= 0xC4400000    l= 0x01400000

	/*SDRAM EMIF3c Data Region Memory Map SIZE 20 MB */
	SDRAMEMIF3c_4     o= 0xC5800000    l= 0x01400000

	/*SDRAM EMIF3c Data Region Memory Map SIZE 20 MB */
	SDRAMEMIF3c_5     o= 0xC6C00000    l= 0x01400000

	
	
}

SECTIONS
{          

/*****  ARM Mappings  *****/
  GROUP 0xFFFF0000:
  {
    .intvect
    .entrytable
  }

    .entryPoint >       ENTRY_POINT
    .text       >       L3_RAM
    .stack      >       L3_RAM
    .bss        >       L3_RAM
    .sstack     >  		L3_RAM
    .istack     >   	L3_RAM
    .fstack     >   	L3_RAM
    .ustack     >   	L3_RAM   
    .ystack     >   	L3_RAM   
    .astack     >   	L3_RAM       
    .cinit      >       L3_RAM
    .cio        >       L3_RAM
    .const      >       L3_RAM
    .data       >       L3_RAM
    .switch     >       L3_RAM
    .sysmem     >       L3_RAM
    .far        >       L3_RAM
    .mmu_level_1_table > L3_RAM
    
 /*Data Buffer Region in SDRAM EMIF3c Region*/
    .SDRAMEMIF3c_0_BUF  >       SDRAMEMIF3c_0		/*Data Buffer Region in SDRAM Region  */     
    .SDRAMEMIF3c_1_BUF  >       SDRAMEMIF3c_1		/*Data Buffer Region in SDRAM Region  */                                                     
    .SDRAMEMIF3c_2_BUF	>       SDRAMEMIF3c_2		/*Data Buffer Region in SDRAM Region  */                                                     
    .SDRAMEMIF3c_3_BUF	>       SDRAMEMIF3c_3		/*Data Buffer Region in SDRAM Region  */                                                     
    .SDRAMEMIF3c_4_BUF	>       SDRAMEMIF3c_4		/*Data Buffer Region in SDRAM Region  */                                                     
    .SDRAMEMIF3c_5_BUF	>       SDRAMEMIF3c_5		/*Data Buffer Region in SDRAM Region  */                                                     
          
}                             

 

