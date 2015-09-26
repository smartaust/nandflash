//我所用的开发板是mini2440，nand  flash型号是K9F2G08U0B，大小为256M
//函数功能：对该型号的nand flash的读写等功能的实现，另外还有一个将
//					nand flash的程序复制到sdram运行的函数


// NAND Flash registers 
#define NFCONF		        (*(volatile unsigned long *)0x4e000000)	//nand falsh配置寄存器
#define NFCONT		        (*(volatile unsigned long *)0x4e000004)	//nand flash控制寄存器
#define NFCMD		        	(*(volatile unsigned long *)0x4e000008)	//nand flash命令寄存器
#define NFADDR		        (*(volatile unsigned long *)0x4e00000C) //nand flash地址寄存器
#define NFDATA		        (*(volatile unsigned char *)0x4e000010)	//nand flash数据寄存器
#define NFSTAT		        (*(volatile unsigned long *)0x4e000020) //nand flash状态寄存器
//注意上述的NFDATA为什么定义为char型而不是long型?可以查看板子上的nand flash接口电路，注意到数据线是IO0-IO7
//只有8根数据线，因此每次只能发送一个字节的数据，只需要使用数据寄存器NFDATA的低8位即可。


#define NAND_SECTOR_SIZE_LP    2048			//K9F2G型号nand falsh的1page=2048KB
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

//nand flash的时序参数
#define TACLS   0
#define TWRPH0  1 
#define TWRPH1  0

//为了方便，将各条命令以宏定义的形式写出
#define readp1          	0x00 
#define readp2          	0x30
#define readid         	0x90
#define writep1        	0x80
#define writep2        	0x10
#define eraseb1        	0x60
#define eraseb2        	0xd0
#define randomwrite     0x85
#define randomreadp1     0x05
#define randomreadp2     0xe0
#define readstatus      0x70
#define reset          	0xff //复位

//使用宏的形式来实现规模较小的函数
#define send_cmd(cmd)	{NFCMD  = (cmd);  }					//向nand flash发送命令
#define send_addr(addr)	{NFADDR = (addr); }				//向nand flssh发送地址
#define send_data(date) {NFDATA = (date); }				//向nand flash发送数据
#define read_byte()	(NFDATA)											//从nand falsh读取一个字节
#define enable()	{NFCONT &= ~(1<<1);}           //nand flash控制器使能
#define disable()	{NFCONT |= (1<<1); }					//使nand flash无效
#define enable_rb()	{NFSTAT |= (1<<2); }         //开启rnb监视模式；
#define check_busy()	{while(!(NFSTAT&(1<<2)));}  //相当于等待rnb置1----这说明nand flash不忙了
#define waitrb()	{while(!(NFSTAT&(1<<0)));} 


void (*show)(char * args,...) = 0x33f91908;

/* S3C2440的NAND Flash处理函数 */
void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr);		//nand flash页读取函数

int main()
{

	unsigned char buf[2048];

	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);  /*需要加上|(3<<2)|(1<<1) */
    NFCONT = (1<<1)|(1<<0);//打开芯片使能，关闭片选
	NFCONT &= ~(1<<1);
	nf_readpage(48,0,buf);
	return 0;
}

void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr)
{
	unsigned int i;
	unsigned int blockpage=(block<<6)+page;		//页的绝对地址=块号*64+页号，乘法使用了移位运算来实现，左移6位相当于乘以64
	/*计算page绝对地址*/											
	enable_rb();										//nand falsh内部的工作状态
	send_cmd(readp1);
	send_addr(0x00);								//地址的低12位用于页内寻址，这里是对整页写入，因此，低12位设为0即可
	send_addr(0x00);								
	send_addr((blockpage)&0xff);		//发送页的绝对地址
	send_addr((blockpage>>8)&0xff);
	send_addr((blockpage>>16)&0x01);

	send_cmd(readp2);

	check_busy();

	for(i=0;i<2048;i++)
	{
		dstaddr[i]=read_byte();
	}
    dstaddr[20] = 0;
	show("dstaddr = %s.\n",dstaddr);
	disable();
}

