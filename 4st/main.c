/*************************************************************************
 * File Name: main.c
 * Author: renwei
 * Mail: renweiaust@163.com 
 * Created Time: Tue 01 Sep 2015 08:59:00 PM CST
 ************************************************************************/
#define SRCPND (*(volatile unsigned long *)0X4A000000) 
#define INTMOD (*(volatile unsigned long *)0X4A000004)
#define INTMSK (*(volatile unsigned long *)0X4A000008)
#define INTPND (*(volatile unsigned long *)0X4A000010)
#define EXTINT1 (*(volatile unsigned long *) 0x5600008c)
#define EINTMASK (*(volatile unsigned long *) 0x560000a4) 
#define EINTPEND (*(volatile unsigned long *)0x560000a8)
#define GPGCON (*(volatile unsigned long *)0x56000060)
#define GPGDAT (*(volatile unsigned long *) 0x56000064) 
#define GPGUP  (*(volatile unsigned long *)0x56000068) 

//#define SUBSRCPND (*(volatile unsigned long *)0X4A000018) 
//INTSUBMSK 0X4A00001C

void (*myshow)(char *str ,...) =0x33f91908;

void irq_handle(void);

int main(void)
{
	int * p = 0x33000000;
	*p = irq_handle;

	//cpu allow irq interrupt
	__asm__ __volatile__(
		"mrs r0,cpsr\n"
		"bic r0,r0,#0x80\n"
		"msr cpsr,r0\n"
		:
		:
		: "r0"
		);

//allow this interrupt 
	INTMSK &= ~(1<<5);//allow this interrupt 
	GPGCON |= 0x2;//config GPG0 as in
	EXTINT1 |= 0x2;//config follow down interrupt
	EINTMASK &= ~(1<<8);//allow extern interrupt
	return 0;
}

void irq_handle(void)
{
	myshow("irq is coming\n");
	EINTPEND |= (1<<8);
	SRCPND = INTPND;
	INTPND = INTPND;
}
