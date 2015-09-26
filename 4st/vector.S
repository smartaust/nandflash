	b reset
	b undef
	b swi
	b abt_pre 
	b abt_dat
	.word 0
	b irq
	b fiq

reset:

undef:

	mov sp,#0x32000000
	stmfd sp!,{r0-r14}

	adr r0,str
	ldr r3,show
	mov lr,pc 
	mov pc,r3

	ldmfd sp,{r0-r13,pc}^

swi:

abt_pre:

abt_dat:

irq:

	mov sp,#0x32000000
	sub lr,lr,#4
	stmfd sp!,{r0-r14}

	mov r1,#0x33000000
	ldr r0,[r1]
	mov lr,pc 
	mov pc,r0

	ldmfd sp,{r0-r13,pc}^
fiq:

show:
	.word 0x33f91908
str:
	.asciz "undefine mode.\n"
