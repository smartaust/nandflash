//�����õĿ�������mini2440��nand  flash�ͺ���K9F2G08U0B����СΪ256M
//�������ܣ��Ը��ͺŵ�nand flash�Ķ�д�ȹ��ܵ�ʵ�֣����⻹��һ����
//					nand flash�ĳ����Ƶ�sdram���еĺ���


// NAND Flash registers 
#define NFCONF		        (*(volatile unsigned long *)0x4e000000)	//nand falsh���üĴ���
#define NFCONT		        (*(volatile unsigned long *)0x4e000004)	//nand flash���ƼĴ���
#define NFCMD		        	(*(volatile unsigned long *)0x4e000008)	//nand flash����Ĵ���
#define NFADDR		        (*(volatile unsigned long *)0x4e00000C) //nand flash��ַ�Ĵ���
#define NFDATA		        (*(volatile unsigned char *)0x4e000010)	//nand flash���ݼĴ���
#define NFSTAT		        (*(volatile unsigned long *)0x4e000020) //nand flash״̬�Ĵ���
//ע��������NFDATAΪʲô����Ϊchar�Ͷ�����long��?���Բ鿴�����ϵ�nand flash�ӿڵ�·��ע�⵽��������IO0-IO7
//ֻ��8�������ߣ����ÿ��ֻ�ܷ���һ���ֽڵ����ݣ�ֻ��Ҫʹ�����ݼĴ���NFDATA�ĵ�8λ���ɡ�


#define NAND_SECTOR_SIZE_LP    2048			//K9F2G�ͺ�nand falsh��1page=2048KB
#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

//nand flash��ʱ�����
#define TACLS   0
#define TWRPH0  1 
#define TWRPH1  0

//Ϊ�˷��㣬�����������Ժ궨�����ʽд��
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
#define reset          	0xff //��λ

//ʹ�ú����ʽ��ʵ�ֹ�ģ��С�ĺ���
#define send_cmd(cmd)	{NFCMD  = (cmd);  }					//��nand flash��������
#define send_addr(addr)	{NFADDR = (addr); }				//��nand flssh���͵�ַ
#define send_data(date) {NFDATA = (date); }				//��nand flash��������
#define read_byte()	(NFDATA)											//��nand falsh��ȡһ���ֽ�
#define enable()	{NFCONT &= ~(1<<1);}           //nand flash������ʹ��
#define disable()	{NFCONT |= (1<<1); }					//ʹnand flash��Ч
#define enable_rb()	{NFSTAT |= (1<<2); }         //����rnb����ģʽ��
#define check_busy()	{while(!(NFSTAT&(1<<2)));}  //�൱�ڵȴ�rnb��1----��˵��nand flash��æ��
#define waitrb()	{while(!(NFSTAT&(1<<0)));} 

void (*show)(char * args,...) = 0x33f91908;

/* S3C2440��NAND Flash������ */
void nand_init(void);			//nand flash��ʼ������
void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr);		//nand flashҳ��ȡ����
void nf_writepage(unsigned int block,unsigned int page,unsigned char *buffer);    //nand flashҳд�뺯��
void erase_block(unsigned int block);      //nand flash���������
void read_id(unsigned char *buf);			//��ȡnand flash ID����
void ramwrite(unsigned int block,unsigned int page,unsigned int add,unsigned char dat);//nand flash���д����
unsigned char ramread(unsigned int block,unsigned int page,unsigned int add);//nand flash���������

static void nand_reset(void);

int main()
{

	char buf[2048];
	nand_init();
//	erase_block(64);
	nf_readpage(48,0,buf);
	return 0;
}



/* s3c2440��nand flash�������� */
static void nand_reset(void)
{
	enable();
	enable_rb();
	send_cmd(reset);  // ��λ����
	check_busy();
	disable();
}

void nand_init(void)
{
	NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4)|(3<<2)|(1<<1);  /*��Ҫ����|(3<<2)|(1<<1) */
	/* ��ʼ��ECC, ��ֹƬѡ */
    NFCONT = (1<<4)|(1<<1)|(1<<0);
	NFSTAT =0x0;
	/* ��λNAND Flash */
	nand_reset();
}

void nf_readpage(unsigned int block,unsigned int page,unsigned char *dstaddr)
{
	unsigned int i;
	unsigned int blockpage=(block<<6)+page;		//ҳ�ľ��Ե�ַ=���*64+ҳ�ţ��˷�ʹ������λ������ʵ�֣�����6λ�൱�ڳ���64
	/*����page���Ե�ַ*/											
	nand_reset();										//�������ǣ���λnand flash��Ȼ���nand flash(�ڸ�λ������ر���nand flash,�����Ҫ����
	enable();												//��)��ͬʱ����æ�źż�⹦�ܣ��Ժ�Ϳ��Զ�nand flash���в�����Ȼ��ͨ�����æ�ź�����ȡ
	enable_rb();										//nand falsh�ڲ��Ĺ���״̬
	send_cmd(readp1);
	send_addr(0x00);								//��ַ�ĵ�12λ����ҳ��Ѱַ�������Ƕ���ҳд�룬��ˣ���12λ��Ϊ0����
	send_addr(0x00);								
	send_addr((blockpage)&0xff);		//����ҳ�ľ��Ե�ַ
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

void RdNF2SDRAM(void)			//��nand flash�Ĵ��븴�Ƶ�sdram��ȥ
{
	unsigned int i;
	unsigned int start_addr =4096;		//4096��ʾ��4096���ֽ�
	unsigned char * dstaddr = (unsigned char *)0x30000000;/*����ָ������ת��*/ 
	unsigned int size = 0x100000;  //��С1MB
	for(i = (start_addr >> 11); size > 0;i ++ )	//��ΪNAND_SECTOR_SIZE����2048���ֽڣ�����page�͵���start_addr������11λ����Ϊ
	{											//2^11=2048������i��ʾҳ�ľ��Ե�ַ������ҳ�����0ҳ�ľ��Ե�ַ
		nf_readpage(i/64,i%64, dstaddr);
		size -= 2048;
		dstaddr += 2048;

	}
}		

void nf_writepage(unsigned int block,unsigned int page,unsigned char *buffer)
{
	unsigned int i,blockpage=(block<<6)+page;	
	/*����page���Ե�ַ*/
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
	/*���ַA18-A28*/
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
