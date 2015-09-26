/*
 *  nand_cp.c
 *  Copyright (C) 2009-2013 Kangear
 *  copy code from nand
 */

// NAND Flash registers
#define NFCONF		        (*(volatile unsigned long *)0x4e000000)	//nand falsh配置寄存器
#define NFCONT		        (*(volatile unsigned long *)0x4e000004)	//nand flash控制寄存器
#define NFCMD		        	(*(volatile unsigned long *)0x4e000008)	//nand flash命令寄存器
#define NFADDR		        (*(volatile unsigned long *)0x4e00000C) //nand flash地址寄存器
#define NFDATA		        (*(volatile unsigned char *)0x4e000010)	//nand flash数据寄存器
#define NFSTAT		        (*(volatile unsigned long *)0x4e000020) //nand flash状态寄存器
//注意上述的NFDATA为什么定义为char型而不是long型?可以查看板子上的nand flash接口电路，注意到数据线是IO0-IO7
//只有8根数据线，因此每次只能发送一个字节的数据，只需要使用数据寄存器NFDATA的低8位即可。


/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
//#define NAND_CMD_READ1		1
//#define NAND_CMD_RNDOUT		5
//#define NAND_CMD_PAGEPROG	0x10
//#define NAND_CMD_READOOB	0x50
//#define NAND_CMD_ERASE1		0x60
//#define NAND_CMD_STATUS		0x70
//#define NAND_CMD_STATUS_MULTI	0x71
//#define NAND_CMD_SEQIN		0x80
//#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
//#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15



#define NF_CMD(cmd)				(NFCMD =  cmd)          			 /* 写命令 */
#define NF_ADDR(addr) 			(NFADDR = addr)         			 /* 写地址 */
#define NF_RDDATA8    			(NFDATA)             				 /* 读8位数据 */
#define NF_ENABLE_CE			(NFCONT &= ~(1<<1))             	 /* 片选使能 */
#define NF_DISABLE_CE			(NFCONT |= (1<<1))              	/* 片选禁用 */
#define NF_CLEAR_RB				(NFSTAT |= (1<<2))          		/* 清除就绪/忙位 */
#define NF_DETECT_RB			do { \
                               	   	   while (!(NFSTAT & (1<<2))); \
                           	   	   } while (0)                     /* 等待操作完成 */

void (*show)(char *str ,...) =0x33f91908;

static void nandll_reset(void)
{
	NF_ENABLE_CE;             /* 片选使能 */
	NF_CLEAR_RB;         /* 清除就绪/忙位 */
	NF_CMD(NAND_CMD_RESET); /* 写复位命令 */
	NF_DETECT_RB;         /* 等待复位完成 */
	NF_DISABLE_CE;            /* 片选禁用 */
}

static int nandll_read_page(unsigned char *buf, unsigned long addr, int large_block)
{
	int i;
	int page_size = 512;

	if (large_block)
		page_size = 2048;

	show("in this fucn\n");
	NF_ENABLE_CE;
	NF_CLEAR_RB;

    NF_CMD(NAND_CMD_READ0);

	NF_ADDR(0x0);
	if (large_block)
		NF_ADDR(0x0); 				/*从0x0地址开始拷贝*/
	NF_ADDR(addr & 0xff);
	NF_ADDR((addr >> 8) & 0xff);
	NF_ADDR((addr >> 16) & 0xff);

    if (large_block)
	    NF_CMD(NAND_CMD_READSTART);

    NF_DETECT_RB;

	for (i = 0; i < page_size; i++)
		buf[i] =  NF_RDDATA8;

	NF_DISABLE_CE;

	return 0;
}

int main()
{
	char buf[2048];
	int i;
	show("time is here\n");
//	nandll_reset();
//	nandll_read_page(buf,0x600000,1);
//	buf[40] = 0;
//	show("buf is %s\n",buf);
	return 0;
}
