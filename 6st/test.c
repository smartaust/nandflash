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
void nand_init(void);			//nand flash初始化函数
void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr);		//nand flash页读取函数
void nf_writepage(unsigned int block,unsigned int page,unsigned char *buffer);    //nand flash页写入函数
void erase_block(unsigned int block);      //nand flash块擦除函数
void read_id(unsigned char *buf);			//读取nand flash ID函数
void ramwrite(unsigned int block,unsigned int page,unsigned int add,unsigned char dat);//nand flash随机写函数
unsigned char ramread(unsigned int block,unsigned int page,unsigned int add);//nand flash随机读函数

static void nand_reset(void);

int main()
{

	char buf[2048];
	nand_init();
//	erase_block(64);
	nf_readpage(48,0,buf);
	return 0;
}



/* s3c2440的nand flash操作函数 */
static void nand_reset(void)
{
	enable();
	enable_rb();
	send_cmd(reset);  // 复位命令
	check_busy();
	disable();
}

void nand_init(void)
{
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(3<<2)|(1<<1);  /*需要加上|(3<<2)|(1<<1) */
	/* 初始化ECC, 禁止片选 */
    NFCONT = (1<<4)|(1<<1)|(1<<0);
	NFSTAT =0x0;
	/* 复位NAND Flash */
	nand_reset();
}

void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr)
{
	unsigned int i;
	unsigned int blockpage=(block<<6)+page;		//页的绝对地址=块号*64+页号，乘法使用了移位运算来实现，左移6位相当于乘以64
	/*计算page绝对地址*/											
	nand_reset();										//这三行是，复位nand flash，然后打开nand flash(在复位结束后关闭了nand flash,因此需要重新
	enable();												//打开)，同时开启忙信号检测功能，以后就可以对nand flash进行操作，然后通过检测忙信号来获取
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

void RdNF2SDRAM(void)			//将nand flash的代码复制到sdram中去
{
	unsigned int i;
	unsigned int start_addr =4096;		//4096表示第4096个字节
	unsigned char * dstaddr = (unsigned char *)0x30000000;/*留心指针类型转换*/ 
	unsigned int size = 0x100000;  //大小1MB
	for(i = (start_addr >> 11); size > 0;i ++ )	//因为NAND_SECTOR_SIZE等于2048个字节，所以page就等于start_addr右移了11位，因为
	{											//2^11=2048。这里i表示页的绝对地址，即该页距离第0页的绝对地址
		nf_readpage(i/64,i%64, dstaddr);
		size -= 2048;
		dstaddr += 2048;

	}
}		

void nf_writepage(unsigned int block,unsigned int page,unsigned char *buffer)
{
	unsigned int i,blockpage=(block<<6)+page;	
	/*计算page绝对地址*/
	unsigned char *buf=buffer;

	nand_reset();
	enable();
	enable_rb();
	
	send_cmd(writep1);	

	send_addr(0x00);
	send_addr(0x00);
	send_addr((blockpage)&0xff);
	send_addr((blockpage>>8)&0xff);
	send_addr((blockpage>>16)&0x01);

	for(i=0;i<2048;i++)
	{
		send_data(*buf++);
	}
	
	send_cmd(writep2);
	check_busy();

	disable();
}

void erase_block(unsigned int block)
{
	unsigned int blocknum=block<<6;

	nand_reset();
	enable();
	send_cmd(eraseb1);
	/*块地址A18-A28*/
	send_addr(blocknum&0xff);
	send_addr((blocknum>>8)&0xff);
	send_addr((blocknum>>16)&0xff);

	send_cmd(eraseb2);	
	check_busy();

	disable();
}

void read_id(unsigned char *buf)
{
	int  i;
	nand_reset();
	enable();
	enable_rb();
	send_cmd(readid);
	send_addr(0x0);

	for(i=0;i<100;i++);

	*buf =read_byte();
	*(buf+1) =read_byte();
	*(buf+2) =read_byte();
	*(buf+3) =read_byte();
	*(buf+4) =read_byte();

	disable();
}



void ramwrite(unsigned int block,unsigned int page,unsigned int add,unsigned char dat)
{
	unsigned int page_number = (block<<6)+page;

	nand_reset();
	enable();
	enable_rb();
	send_cmd(writep1);

	send_addr(0x00);
	send_addr(0x00);
	send_addr((page_number)&0xff);
	send_addr((page_number>>8)&0xff);
	send_addr((page_number>>16)&0xff);
	
	send_cmd(randomwrite);

	send_addr((char)add&0xff);
	send_addr((char)((add>>8)&0x0f));

	send_data(dat);
	
	send_cmd(writep2);

	check_busy();
	disable();

}

unsigned char ramread(unsigned int block,unsigned int page,unsigned int add)
{
	unsigned char buf;
	unsigned int page_number=(block<<6)+page;

	nand_reset();
	enable();
	enable_rb();
	send_cmd(readp1);

	send_addr(0x00);
	send_addr(0x00);
	send_addr((page_number)&0xff);
	send_addr((page_number>>8)&0xff);
	send_addr((page_number>>16)&0xff);

	send_cmd(readp2);

	check_busy();

	send_cmd(randomreadp1);

	send_addr((char)(add&0xff));
	send_addr((char)((add>>8)&0x0f));

	send_cmd(randomreadp2);

	check_busy();
	buf=read_byte();
	disable();

	return buf;
}
