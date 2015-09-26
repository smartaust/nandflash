	mov ip,sp

	stmfd sp!,{fp,ip,lr,pc}
	sub fp,ip,#4

	ldr r0,=str
	mov lr,pc
	ldr pc,show
    sub sp ,fp,#12 
	ldmfd sp,{fp,sp,pc}
show:
	
	.word 0x33f91908

str:
	.asciz "hello world.\n"



