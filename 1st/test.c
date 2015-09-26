/*************************************************************************
 * File Name: test.c
 * Author: renwei
 * Mail: renweiaust@163.com 
 * Created Time: 2015年08月25日 星期二 23时21分44秒
 ************************************************************************/
void (* show) (char *s ,...) = 0x33f91908;

int main()
{
		show("hello world \n");
		return 0;
}
