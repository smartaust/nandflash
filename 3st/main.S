	.section .text
	.global  main 
main:
	mov ip,sp
	stmfd sp!,{fp,ip,lr,pc}
	sub fp,ip,#4

	.word 0x77777777

	sub sp,fp,#12
	ldmfd sp,{fp,sp,pc}
