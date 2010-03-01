	.state32
    .global _c_intOMAP

    .global _vec_reset_handler
    .global _vec_undefined_handler
    .global _vec_swi_handler
    .global _vec_prefetch_handler
    .global _vec_abort_handler
	.global _vec_reserved_handler
    .global _vec_irq_handler
    .global _vec_fiq_handler

	.global _RESET_HANDLER
	.global _UNDEFINED_HANDLER
	.global _PREFETCH_HANDLER
	.global _ABORT_HANDLER
	.global _SWI_HANDLER

	.sect		".intvect"
_arm9_reset_vector:
    LDR  pc, _vec_reset_handler         ; Get address of Reset handler 
    LDR  pc, _vec_undefined_handler     ; Get address of Undefined handler 
    LDR  pc, _vec_swi_handler          	; Get address of SWI handler 
    LDR  pc, _vec_prefetch_handler      ; Get address of Prefetch handler 
    LDR  pc, _vec_abort_handler         ; Get address of Abort handler 
    LDR  pc, _vec_reserved_handler      ; Reserved 
	LDR  pc, [pc, #-0xA1C]  			; HPIVR[1] IRQs
;	LDR  pc, [pc, #-0xA24]  			; HPIVR[0] FIQs  [pc, #-0xA20] ?
;	LDR  pc, _vec_irq_handler
	LDR  pc, _vec_fiq_handler   		 

; Below vectors may be overwritten as needed for user routines.
_vec_fiq_handler:
    .word arm9_fiq_handler

_vec_reset_handler:
    .word arm9_reset_handler

_vec_undefined_handler:
    .word arm9_undefined_handler

_vec_swi_handler:
    .word arm9_swi_handler

_vec_prefetch_handler:
    .word arm9_prefetch_handler

_vec_abort_handler:
    .word arm9_abort_handler

_vec_reserved_handler:
    .word arm9_undefined_handler

_vec_irq_handler:
    .word arm9_irq_handler

c_RESET_HANDLER .long _RESET_HANDLER
c_UNDEFINED_HANDLER .long _UNDEFINED_HANDLER
c_SWI_HANDLER .long _SWI_HANDLER
c_PREFETCH_HANDLER .long _PREFETCH_HANDLER
c_ABORT_HANDLER .long _ABORT_HANDLER

hipvr0 .long 0xFFFEF600  ;address to jump FIQ
hipvr1 .long 0xFFFEF604  ;address to jump IRQ
hipir0 .long 0xFFFEE900  ;Interrupt index FIQ
hipir1 .long 0xFFFEE904  ;interrupt index IRQ
sicr .long 0xFFFEE024  ;iterrupt clear register

arm9_reset_handler:
    LDR		R0, c_RESET_HANDLER	; get addres with functions address
	MOV	LR, PC				; ? move it into pc
	BX R0					; ? jump
	B    _c_intOMAP

arm9_undefined_handler:
	LDR		R0, c_UNDEFINED_HANDLER	; get addres with functions address
	MOV	LR, PC				; ? move it into pc
	BX R0					; ? jump
	B	arm9_undefined_handler

arm9_swi_handler:
	LDR		R0, c_SWI_HANDLER	; get addres with functions address
	MOV	LR, PC				; ? move it into pc
	BX R0					; ? jump
	B	arm9_swi_handler

arm9_prefetch_handler:
	LDR		R0, c_PREFETCH_HANDLER	; get addres with functions address
	MOV	LR, PC				; ? move it into pc
	BX R0					; ? jump
	B	arm9_prefetch_handler

arm9_abort_handler:
	LDR		R0, c_ABORT_HANDLER	; get addres with functions address
	MOV	LR, PC				; ? move it into pc
	BX R0					; ? jump
	B	arm9_abort_handler

arm9_irq_handler:
	STMFD	SP!, {R0-R14, LR}	; Push all the system mode registers onto the task stack.
	LDR		R1, hipvr1			; get addres with functions address
	LDR		R0, [R1]			; get functions address
	MOV	LR, PC					; ? move it into pc
	BX R0						; ? jump

	LDR		R1, hipir1
	LDR		R0, [R1]			; Get the interrupt index
	STR		R0, [R1]			; Write it back to HIPIR to unlock it
	LDR R1, sicr				; Now get the system interrupt status clear register
	STR R0, [R1]				; Clear the host interrupt 
	LDMFD	SP!, {R0-R14, LR}	; Restore all system mode registers for the task.
	
arm9_fiq_handler:
	STMFD	SP!, {R0-R14, LR}	; Push all the system mode registers onto the task stack.

	LDR		R1, hipvr0			; get addres with functions address
	LDR		R0, [R1]			; get functions address
	MOV	LR, PC					; ? move it into pc
	BX R0						; ? jump

	LDR		R1, hipir0
	LDR		R0, [R1]			; Get the interrupt index
	STR		R0, [R1]			; Write it back to HIPIR to unlock it
	LDR R1, sicr				; Now get the system interrupt status clear register
	STR R0, [R1]				; Clear the host interrupt 	

	LDMFD	SP!, {R0-R14, LR}	; Restore all system mode registers for the task.
	SUBS	PC, LR, #4			; jump back

	.end
