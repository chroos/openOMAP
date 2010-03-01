;****************************************************************************
;* arm_boot.asm
;*
;* THIS MODULE PERFORMS THE FOLLOWING ACTIONS:
;*   1) ALLOCATES THE STACK AND INITIALIZES THE STACK POINTER
;*   2) PERFORMS AUTO-INITIALIZATION
;*   3) CALLS INITALIZATION ROUTINES FOR FILE SCOPE CONSTRUCTION
;*   4) CALLS THE FUNCTION MAIN TO START THE C PROGRAM
;*   5) CALLS THE STANDARD EXIT ROUTINE
;*
;****************************************************************************

	.state32
	.global	__stack
	.global	__STACK_SIZE

  .global __mmu_level_1_table
  .sect	    ".entryPoint"

;***************************************************************
;* DEFINE THE DIFFERENT STACK SECTIONS (C STACK should be 
;* defined in RTSx.lib)
;***************************************************************
__sstack:	.usect  ".sstack", 0x400, 4        
__istack:	.usect  ".istack", 0x400, 4
__fstack:	.usect  ".fstack", 0x400, 4
__ustack:	.usect  ".ustack", 0x400, 4
__ystack:	.usect  ".ystack", 0x400, 4
__astack:	.usect  ".astack", 0x400, 4
__mmu_level_1_table:.usect	".mmu_level_1_table", 0x4000, 16384
	.align	4
$C$CON1:	.field  	00002001h,32
	.align	4
$C$CON2:	.field  	00002078h,32  ;SBO set and Vector is 0xFFFF0000

	.global	_c_intOMAP
;***************************************************************
;* FUNCTION DEF: ENTRY POINT                                   
;***************************************************************
_c_intOMAP:
        ;*------------------------------------------------------
; Handle MMU now
	LDR R1, $C$CON1
	MCR p15, #0, r0, c15, c1, #0   ;allow access to all co processors
	NOP
	NOP
	NOP
	LDR r0, $C$CON2  ; disable MMU, caches, write buffer
	MCR p15, #0, r0, c1, c0, #0
	NOP
	NOP
	NOP
	mvn r0, #0
	mcr p15, #0, r0, c8, c7, #0  ; flush tlb's
	mcr p15, #0, r0, c7, c7, #0  ; flush Caches
	mcr p15, #0, r0, c7, c10, #4 ; flush Write Buffer
	NOP
	NOP
	NOP
	mvn r0, #0
	mcr p15, #0, r0, c3, c0, #0  ; grant manager access to all domains
	NOP
	NOP
	NOP
    ;*------------------------------------------------------
    ;* Disable ITCM bus
    ;*------------------------------------------------------
    LDR R0 , ITCM_RegionReg_Val
    MCR p15, #00, R0, c9, c1, #1
    NOP
    NOP
    ;*------------------------------------------------------
    ;* Disable DTCM bus
    ;*------------------------------------------------------
    LDR R0 , DTCM_RegionReg_Val
    NOP
    NOP
    MCR p15, #00, R0, c9 , c1, #0
    NOP
    NOP

;*------------------------------------------------------
;*  STACK INITIALIZE (see .CMD file)       
;*------------------------------------------------------
	.global __S_STACK_SIZE
	.global __I_STACK_SIZE
	.global __U_STACK_SIZE
	.global __F_STACK_SIZE
	.global __Y_STACK_SIZE
	.global __A_STACK_SIZE

	.global _initStack
_initStack:
;*------------------------------------------------------
;* SET TO IRQ  MODE
;*------------------------------------------------------
    MRS     r0, cpsr
    BIC     r0, r0, #0x1F  ; CLEAR MODES
    ORR     r0, r0, #0x12  ; SET  IRQ MODE
	MSR     cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE IRQ  MODE STACK                      
;*------------------------------------------------------
	LDR     SP, i_stack
	LDR     R0, i_STACK_SIZE
	ADD	SP, SP, R0

;*------------------------------------------------------
;* SET TO FIQ  MODE
;*------------------------------------------------------
	MRS	r0, cpsr
	BIC	r0, r0, #0x1F	; CLEAR MODES
	ORR	r0, r0, #0x11	; SET FIQ mode
	MSR	cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE FIQ  MODE STACK                      
;*------------------------------------------------------
	LDR     SP, f_stack
	LDR     R0, f_STACK_SIZE
	ADD	SP, SP, R0
	
;*------------------------------------------------------
;* SET TO UNDEF  MODE
;*------------------------------------------------------
  	MRS	r0, cpsr
	BIC	r0, r0, #0x1F	; CLEAR MODES
	ORR	r0, r0, #0x1B	; SET UNDEF mode
	MSR	cpsr, r0

;*------------------------------------------------------
;* INITIALIZE THE UNDEF  MODE STACK                      
;*------------------------------------------------------
	LDR     SP, u_stack
	LDR     R0, u_STACK_SIZE
	ADD	SP, SP, R0
	                	        
;*------------------------------------------------------
;* SET TO SYSTEM  MODE
;*------------------------------------------------------
	MRS	r0, cpsr
	BIC	r0, r0, #0x1F	; CLEAR MODES
	ORR	r0, r0, #0x1F	; SET SYSTEM mode
	MSR	cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE SYSTEM MODE STACK                      
;*------------------------------------------------------
	LDR     SP, y_stack
	LDR     R0, y_STACK_SIZE
	ADD	SP, SP, R0

;*------------------------------------------------------
;* SET TO ABORT  MODE
;*------------------------------------------------------
	MRS	r0, cpsr
	BIC	r0, r0, #0x1F	; CLEAR MODES
	ORR	r0, r0, #0x17	; SET ABORT mode
	MSR	cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE ABORT MODE STACK                      
;*------------------------------------------------------
	LDR     SP, a_stack
	LDR     R0, a_STACK_SIZE
	ADD	SP, SP, R0


;*------------------------------------------------------
;* SET TO SUPERVISOR  MODE
;*------------------------------------------------------
	MRS	r0, cpsr
	BIC	r0, r0, #0x1F	; CLEAR MODES
	ORR	r0, r0, #0x13	; SET SUPERVISOR mode
	MSR	cpsr, r0       
        
;*------------------------------------------------------
;* INITIALIZE THE SUPERVISOR  MODE STACK                      
;*------------------------------------------------------
	LDR     SP, s_stack
	LDR     R0, s_STACK_SIZE
	ADD	SP, SP, R0

;****************************************************************************
;*  32 BIT STATE BOOT ROUTINE                                               *
;****************************************************************************

	;*------------------------------------------------------
	;* PERFORM AUTO-INITIALIZATION.  IF CINIT IS -1, THEN
	;* THERE IS NONE.
	;*------------------------------------------------------
	LDR		r0, c_cinit
	CMN		r0, #1
	BLNE		auto_init

	;*------------------------------------------------------
	;* CALL INITIALIZATION ROUTINES FOR CONSTRUCTORS. IF
	;* PINIT IS -1, THEN THERE ARE NONE.
	;*------------------------------------------------------
	
	LDR		r5, c_pinit
	CMN		r5, #1
	BEQ		_c2_
	B		_c1_

_loop_:
	BL		IND_CALL
	
_c1_:
	LDR		r4, [r5], #4
	CMP		r4, #0
	BNE		_loop_


	;*------------------------------------------------------
	;* CALL APPLICATION                                     
	;*------------------------------------------------------
_c2_:
; Sets up 


; Initialize ARM926 internal registers.

	MOV	r0, #0
	MOV	r1, #0
	MOV	r2, #0
	MOV	r3, #0
	MOV	r4, #0
	MOV	r5, #0
	MOV	r6, #0
	MOV	r7, #0
	MOV	r8, #0
	MOV	r9, #0
	MOV	r10, #0
	MOV	r11, #0
	MOV	r12, #0

	B		_main

	;*------------------------------------------------------
	;* IF APPLICATION DIDN'T CALL EXIT, CALL EXIT(1)
	;*------------------------------------------------------
	MOV		R0, #1
	BL		_exit

	;*------------------------------------------------------
	;* DONE, LOOP FOREVER
	;*------------------------------------------------------
L1:
	B		L1

;***************************************************************************
;*  _enableID_MMU.
;*
;* Enable MMU handled here instead of mmu.c due to problems using #asm in code composer
;*                                                                          
;***************************************************************************
	.global _enableID_MMU
_enableID_MMU:
    MRC     p15, #0, r0, c1, c0, #0       ; read CP15 register 1 into r0

    ORR     r0, r0, #(0x1  <<12)        ; enable I Cache
    ORR     r0, r0, #(0x1  <<2)         ; enable D Cache
    ORR     r0, r0, #(0x1  )         ; enable MMU
;    ORR     r0, r0, #(0x3 <<30)        ; enable asynchronous clocking mode

    MCR     p15, #0, r0, c1, c0, #0       ; write CP15 register 1
	mov		r0,#0
	MRC     p15, #0, r0, c1, c0, #0       ; read CP15 register 1 into r0

    MOV     pc, lr


;***************************************************************************
;*  PROCESS INITIALIZATION TABLE.
;*
;*  THE TABLE CONSISTS OF A SEQUENCE OF RECORDS OF THE FOLLOWING FORMAT:
;*                                                                          
;*       .word  <length of data (bytes)>
;*       .word  <address of variable to initialize>                         
;*       .word  <data>
;*                                                                          
;*  THE INITIALIZATION TABLE IS TERMINATED WITH A ZERO LENGTH RECORD.
;*                                                                          
;***************************************************************************

tbl_addr: .set    R0
var_addr: .set    R1
length:   .set    R2
data:     .set    R3

auto_init:
	B		rec_chk

	;*------------------------------------------------------
	;* PROCESS AN INITIALIZATION RECORD
	;*------------------------------------------------------
record:
	LDR		var_addr, [tbl_addr], #4   ;

	;*------------------------------------------------------
	;* COPY THE INITIALIZATION DATA
	;*------------------------------------------------------
	TST		var_addr, #3				; SEE IF DEST IS ALIGNED
	BNE		_bcopy						; IF NOT, COPY BYTES
	SUBS	length, length, #4			; IF length <= 3, ALSO
	BMI		_bcont						; COPY BYTES

_wcopy:
	LDR		data, [tbl_addr], #4		;
	STR		data, [var_addr], #4		; COPY A WORD OF DATA
	SUBS	length, length, #4			;
	BPL		_wcopy						;

_bcont:
	ADDS	length, length, #4			;
	BEQ		_cont						;

_bcopy:
	LDRB	data, [tbl_addr], #1		;
	STRB	data, [var_addr], #1		; COPY A BYTE OF DATA
	SUBS	length, length, #1			;
	BNE		_bcopy						;

_cont:
	ANDS	length, tbl_addr, #0x3		; MAKE SURE THE ADDRESS
	RSBNE	length, length, #0x4		; IS WORD ALIGNED
	ADDNE	tbl_addr, tbl_addr, length	;

rec_chk:
	LDR		length, [tbl_addr], #4		; PROCESS NEXT
	CMP		length, #0					; RECORD IF LENGTH IS
	BNE		record						; NONZERO

	MOV		PC, LR

;***************************************************************
;* CONSTANTS USED BY THIS MODULE
;***************************************************************
c_stack		.long	__stack
c_STACK_SIZE  	.long	__STACK_SIZE
c_cinit       	.long	cinit
c_pinit       	.long	pinit

;******************************************************
;* UNDEFINED REFERENCES                               *
;******************************************************
	.global IND_CALL
	.global	_exit
	.global	_main
	.global	cinit
	.global	pinit
	.global	__STACK_SIZE
     
s_stack         .long    __sstack
s_STACK_SIZE    .long    __S_STACK_SIZE
i_stack         .long    __istack
i_STACK_SIZE    .long    __I_STACK_SIZE  
f_stack         .long    __fstack
f_STACK_SIZE    .long    __F_STACK_SIZE  
u_stack         .long    __ustack
u_STACK_SIZE    .long    __U_STACK_SIZE  
y_stack         .long    __ystack
y_STACK_SIZE    .long    __Y_STACK_SIZE  
a_stack         .long    __astack
a_STACK_SIZE    .long    __A_STACK_SIZE  

ITCM_RegionReg_Val .word 0x00000000
DTCM_RegionReg_Val .word 0x00000000
	.end

