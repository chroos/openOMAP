	.state32
	.global c_FIQ
	.global _testRoutine
	.global _c_mytest
	.global _c_IRQ_HANDLER

c_gpir .long 0xFFFEE080  ;which active
c_hipir1 .long 0xFFFEE900  ;which active
c_hipir2 .long 0xFFFEE904  ;which active
c_sicr .long 0xFFFEE024  ;for clear

c_FIQ .long _testRoutine

_c_mytest:
	LDR PC, [PC, #4]

_c_IRQ_HANDLER:
	LDR		R0, c_FIQ
	MOV	LR, PC
	BX R0
	
	LDR		R1, c_hipir1
	LDR		R0, [R1]			; Get the interrupt index
	STR		R0, [R1]			; Write it back to HIPIR to unlock it
	LDR R1, c_sicr				; Now get the system interrupt status clear register
	STR R0, [R1]				; Clear the host interrupt
	.end

