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


//abount nand flash read things;
#define NFCONF (*(volatile unsigned long *)0x4E000000)
#define NFCONT (*(volatile unsigned long *)0x4E000004) 

#define NFCMMD (*(volatile unsigned char *)0x4E000008)
#define NFADDR (*(volatile unsigned char *)0x4E00000C) 
#define NFDATA (*(volatile unsigned char *)0x4E000010)
#define NFSTAT (*(volatile unsigned char *)0x4E000020) 

void (*show)(char *str ,...) =0x33f91908;

int main(void)
{
	int i;
	char buf[2048];

	unsigned long addr = 48;
	addr = addr << 6;

	NFCONF = (0<<12)|(1<<8)|(0<<4);
//    NFCONT = (1<<1)|(1<<0);

	NFCONT &= ~(1<<1);
    NFCONT |= 1;
	//NFSTAT |= (1<<2);

	show("init nand ok\n");

	NFCMMD = 0x00;

	NFADDR = 0;

	NFADDR = 0;

	NFADDR = addr & 0xff;

	NFADDR = (addr>>8) & 0xff;

	NFADDR = (addr>>16) & 0x01;

	NFCMMD = 0x30;
	show("cmd addr translate ok...\n");
	while(!(NFSTAT&1))
		;
	show("CONF:%04x\n",NFCONF);

	show("CONT:%04x\n",NFCONT);

	show("stat:%04x\n",NFSTAT);

	for(i = 0;i < 2048; i++)
	{
		buf[i] = NFDATA;
	}

	buf[40] = 0;
	show("buf = %s\n",buf);
    NFCONT = (1<<1);
	NFCONT &=~(1<<0);
	return 0;

}

