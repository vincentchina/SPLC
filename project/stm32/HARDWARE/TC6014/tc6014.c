#include "tc6014.h"

TC6014_TypeDef tc6014s_wregs[2];
TC6014_TypeDef* ptc6014_wregs;

uint32_t adr = TC6014A_BASE_ADDR;

//改变操作的芯片
void select_chip(int chip)
{
	if(chip == TC6014A)
	{
		adr = TC6014A_BASE_ADDR;
		ptc6014_wregs = &tc6014s_wregs[0];
	}
	else
	{
		adr = TC6014B_BASE_ADDR;
		ptc6014_wregs = &tc6014s_wregs[1];
	}
}

//正在操作哪个芯片
int which_chip(void)
{
	if(adr == TC6014A_BASE_ADDR)
		return TC6014A;
	else
		return TC6014B;
}

// wreg1(轴指定,数据)------------写入寄存器1 设定
void wreg1(int axis,int wdata)
{
	//D8~D11位为XYZU轴选择位，所以
	// 1      代表第1轴
	// 2      代表第2轴
	// 4      代表第3轴
	// 8      代表第4轴
	//0x0f是轴切换命令
	outpw(adr+wr0, (axis << 8) + 0xf);//轴指定
	outpw(adr+wr1, wdata);
}

// wreg2(轴指定,数据)-------------写入寄存器2 设定
void wreg2(int axis,int wdata)
{
	outpw(adr+wr0, (axis << 8) + 0xf);//轴指定
	outpw(adr+wr2, wdata);
}

// wreg3(轴指定,数据)-------------写入寄存器3 设定
void wreg3(int axis,int wdata)
{
	outpw(adr+wr0, (axis << 8) + 0xf);//轴指定
	outpw(adr+wr3, wdata);
}

// command(轴指定,命令编码)-----命令写入
void command(int axis,int cmd)
{
	outpw(adr+wr0, (axis << 8) + cmd);
}

// range(轴指定,数据)----------------范围(R) 设定
void range(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x00);
}

// acac(轴指定,数据)----------------变化率(K)设定
void acac(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x01);
}

// dcac(轴指定,数据)----------------变化率(L)设定
void dcac(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x0e);
}


// acc(轴指定,数据)------------------加/减速度(A)设定
void acc(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x02);
}

// dec(轴指定,数据) -------------------减速度(D) 设定
void dec(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x03);
}

// startv(轴指定,数据) -----------------初始速度(SV) 设定
void startv(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x04);
}

// speed(轴指定,数据) -----------------驱动速度(V) 设定
void speed(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x05);
}

// pulse(轴指定,数据) -----------------输出脉冲数/终点(P) 设定
void pulse(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x06);
}

// decp(轴指定,数据) -----------------手动减速点(DP) 设定
void decp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x07);
}

// center(轴指定,数据) ----------------圆弧中心点(C) 设定
void center(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x08);
}

// lp(轴指定,数据) --------------------逻辑位置计数器(LP)设定
void lp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x09);
}

// ep(轴指定,数据) ------------------实际位置计数器(EP)设定
void ep(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0a);
}

// compp(轴指定,数据) ------------COMP + (CP) 设定
void compp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0b);
}

// compm(轴指定,数据) -----------COMP - (CM) 设定
void compm(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0c);
}

// accofst(轴指定,数据) -----------加速计数器偏移(AO)设定
void accofst(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0d);
}

//---------------------------------原点检测速度(HV)设定
void hsspeed(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x61);
}

//---------------------------------外部模式(EM)设定
void expmode(int axis,int em6data,int em7data)
{
	outpw(adr+wr6, em6data);
	outpw(adr+wr7, em7data);
	outpw(adr+wr0, (axis << 8) + 0x60);
}

//---------------------------------同步操作模式(SM)设定
void syncmode(int axis,int sm6data,int sm7data)
{
	outpw(adr+wr6, sm6data);
	outpw(adr+wr7, sm7data);
	outpw(adr+wr0, (axis << 8) + 0x64);
}

// readlp(轴指定)---------------------逻辑位置计数器数值(LP)读出
long readlp(int axis)
{
	long a;long d6;long d7;
	outpw(adr+wr0, (axis << 8) + 0x10);
	d6 = inpw(adr+rr6);d7 = inpw(adr+rr7);
	a = d6 + (d7 << 16);
	return(a);
}

// readep(轴指定)--------------------实际位置计数器数值(EP)读出
long readep(int axis)
{
	long a;long d6;long d7;
	outpw(adr+wr0, (axis << 8) + 0x11);
	d6 = inpw(adr+rr6);d7 = inpw(adr+rr7);
	a = d6 + (d7 << 16);
	return(a);
}

// wait(轴指定)------------------------等驱动结束
void waitaxis(int axis)
{
	while(inpw(adr+rr0) & axis);
}
             
// next_wait( )--------------------------等连续插补下一个数据设定
void next_wait(void)       
{
	while((inpw(adr+rr0) & 0x0200) == 0x0);
}

// bp_wait( )----------------------------等BP 插补下一个数据设定
void bp_wait(void)        
{
	while((inpw(adr+rr0) & 0x6000) == 0x6000);
}

// homesrch( )---------------------全轴原点检出
void homesrch(void)
{     
	speed(0x3,2000);// 1、2轴的速度是2000   
	hsspeed(0x3,50);   
	pulse(0x1,3500); //第一轴的脉冲数 
	pulse(0x2,700);  //第二轴的脉冲数 

	     
	speed(0x4,40);  //第3轴的速度是40  
	hsspeed(0x4,40);   
	pulse(0x4,20);  //第三轴的脉冲数  
	     
	hsspeed(0x8,30);  
	command(0xf,0x62);//所有轴自动原点回归   
	waitaxis(0xf);  //等待驱动结束  
	if(inpw(adr+rr0) & 0x0010)  
	{
		printf("X-axis Home Search Error \n");
	}
	if(inpw(adr+rr0) & 0x0020)
	{
		printf("Y-axis Home Search Error \n");
	}
	if(inpw(adr+rr0) & 0x0040)
	{
		printf("Z-axis Home Search Error \n");
	}
	if(inpw(adr+rr0) & 0x0080)
	{
		printf("U-axis Home Search Error \n");
	}
}

void test(char flag)
{
	int  count;
	outpw(adr+wr0, 0x8000);//软件复位
	for(count = 0; count < 50; ++count);

	//------------全轴模式设定------------
	command(0xf,0xf);//第一二轴切换，即选定第一二轴
	outpw(adr+wr1, 0x0000);//模式寄存器1 00000000 00000000  
	outpw(adr+wr2, 0x0000);//模式寄存器2 00000000 00000000  
	outpw(adr+wr3, 0x0000);//模式寄存器3 00000000 00000000  
	//expmode(0x3,0x5d08,0x497f); //第一二轴外部模式设定 

	//--------全轴操作参数初期设定----
	accofst(0xf,0);//A0 = 0 ，即偏移为0，默认为8的   
	range(0xf,800000); //R = 800,000(倍率 = 10) 
	acac(0xf,1010); //K = 1,010(变化率 = 619KPPS/SEC2)   
	dcac(0xf,1010); //L = 1,010(变化率 = 619KPPS/SEC2)    
	acc(0xf,100);  //A = 100(加/减速度=125KPPS/SEC)  
	dec(0xf,100); //D = 100(减速度 = 125KPPS/SEC)  
	startv(0xf,100);  //SV = 100(初始速度 = 1,000PPS) 
	speed(0xf,4000);  //V = 4,000(驱动速度 = 40,000PPS) 
	pulse(0xf,100000); //P = 100,000(输出脉冲数 = 100,000)  
	lp(0xf,0);  //LP = 0(逻辑位置计数器 = 0)
	ep(0xf,0);  //EP = 0(实际位置计数器 = 0)   
	
	//wr4和wr5所有轴公用�
	outpw(adr+wr4, 0x0303); //nOUT3~0输出低电平      
	//outpw(adr+wr5, 0x0124); //2轴线速度恒定，X为主轴，Y为从轴，Z为从轴
	
//	acc(0x3,200);   
//	speed(0x3,4000);  
//	pulse(0x1,80000);  
//	pulse(0x2,40000);   
	//command(0xf,0x20); //正方向定量驱动 
	if(flag)
		command(0xf,0x20); //正方向定量驱动
	else
		command(0xf,0x21); //正方向定量驱动
	waitaxis(0xf);
}

/*
int main(void)
{
	int  count;
	outpw(adr+wr0, 0x8000);//软件复位
	for(count = 0; count < 2; ++count);

	//------------全轴模式设定------------
	command(0x3,0xf);//第一二轴切换，即选定第一二轴
	outpw(adr+wr1, 0x0000);//模式寄存器1 00000000 00000000  
	outpw(adr+wr2, 0xe000);//模式寄存器2 11100000 00000000  
	outpw(adr+wr3, 0x0000);//模式寄存器3 00000000 00000000  
	expmode(0x3,0x5d08,0x497f); //第一二轴外部模式设定 

	//--------全轴操作参数初期设定----
	accofst(0x3,0);//A0 = 0 ，即偏移为0，默认为8的   
	range(0x3,800000); //R = 800,000(倍率 = 10) 
	acac(0x3,1010); //K = 1,010(变化率 = 619KPPS/SEC2)   
	dcac(0x3,1010); //L = 1,010(变化率 = 619KPPS/SEC2)    
	acc(0x3,100);  //A = 100(加/减速度=125KPPS/SEC)  
	dec(0x3,100); //D = 100(减速度 = 125KPPS/SEC)  
	startv(0x3,100);  //SV = 100(初始速度 = 1,000PPS) 
	speed(0x3,4000);  //V = 4,000(驱动速度 = 40,000PPS) 
	pulse(0x3,100000); //P = 100,000(输出脉冲数 = 100,000)  
	lp(0x3,0);  //LP = 0(逻辑位置计数器 = 0)
	ep(0x3,0);  //EP = 0(实际位置计数器 = 0)  
	
	command(0xc,0xf);  //第三四轴切换，即选定第三四轴
	outpw(adr+wr1, 0x0000);                                            
	outpw(adr+wr2, 0x0000);  
	outpw(adr+wr3, 0x0000);   
	expmode(0x4,0x5d08,0x01c4); //第三轴外部模式设定  
	expmode(0x8,0x5d08,0x010c); //第四轴外部模式设定  
	 
	accofst(0xc,0);  
	range(0xc,800000);  
	acac(0xc,1010);    
	dcac(0xc,1010);   
	acc(0xc,100);    
	dec(0xc,100);   
	startv(0xc,50); //SV = 50(初始速度 = 500PPS)   
	speed(0xc,40);  //V = 40(驱动速度 = 400PPS)  
	pulse(0xc,10); //P = 10(输出脉冲数 = 10)   
	lp(0xc,0);    
	
	//wr4和wr5所有轴公用�
	outpw(adr+wr4, 0x0000); //nOUT3~0输出低电平      
	outpw(adr+wr5, 0x0124); //2轴线速度恒定，X为主轴，Y为从轴，Z为从轴 

	     
	acc(0x3,200);   
	speed(0x3,4000);  
	pulse(0x1,80000);  
	pulse(0x2,40000);   
	command(0x3,0x20); //正方向定量驱动 
	waitaxis(0x3);                           
	    
	wreg3(0x1, 0x0002); //第一轴使用减速度的数值 
	acc(0x1,200);  
	dec(0x1,50);   
	speed(0x1,4000);   
	pulse(0x1,80000);   
	command(0x1,0x20);  
	waitaxis(0x1);                         
	wreg3(0x1, 0x0000);                  
	    
	wreg3(0x3, 0x0004); //第一二轴使用s曲线加/减速  
	acac(0x3,1010);    
	acc(0x3,200);    
	speed(0x3,4000);   
	pulse(0x1,50000);   
	pulse(0x2,25000);   
	command(0x3,0x21);  //负方向定量驱动  
	waitaxis(0x3);
	wreg3(0x3, 0x0000);                 
	     
	startv(0x4,40);   
	speed(0x4,40);    
	pulse(0x4,700);    
	command(0x4,0x20);   
	waitaxis(0x4);   
	pulse(0x4,350);  
	command(0x4,0x21);  
	waitaxis(0x4);    
	                                          
	outpw(adr+wr5, 0x0124);  
	range(0x1,800000);   
	range(0x2,1131371);   
	speed(0x1,100);   
	pulse(0x1,5000);   
	pulse(0x2,-2000);  
	command(0x0,0x30);        
	waitaxis(0x3);
	                                           
	outpw(adr+wr5, 0x0124);  
	range(0x1,800000);  
	range(0x2,1131371);   
	speed(0x1,100);   
	center(0x1,-5000);  
	center(0x2,0);                     
	pulse(0x1,0);                         
	pulse(0x2,0);                        
	command(0x0,0x33);                   
	waitaxis(0x3);
	                                            
	speed(0x1,1);    
	command(0,0x36); //位模式插补 
	outpw(adr+bp1p, 0x0000);  
	outpw(adr+bp1m, 0x2bff);
	outpw(adr+bp2p, 0xffd4);
	outpw(adr+bp2m, 0x0000);
	command(0,0x38);
	outpw(adr+bp1p, 0xf6fe); 
	outpw(adr+bp1m, 0x0000);
	outpw(adr+bp2p, 0x000f);
	outpw(adr+bp2m, 0x3fc0);
	command(0,0x38);
	outpw(adr+bp1p, 0x1fdb);  
	outpw(adr+bp1m, 0x0000);
	outpw(adr+bp2p, 0x00ff);
	outpw(adr+bp2m, 0xfc00);
	command(0,0x38);
	command(0,0x34);
	bp_wait();    
	outpw(adr+bp1p, 0x4000);  
	outpw(adr+bp1m, 0x7ff5);
	outpw(adr+bp2p, 0x0000);
	outpw(adr+bp2m, 0x0aff);
	command(0,0x38);
	command(0,0x37);  
	waitaxis(0x3);  
	                                   
	speed(0x1,100);  
	pulse(0x1,4500);  
	pulse(0x2,0);
	command(0,0x30);
	next_wait();    
	center(0x1,0);    
	center(0x2,1500);
	pulse(0x1,1500);
	pulse(0x2,1500);
	command(0,0x33);
	next_wait();
	pulse(0x1,0);   
	pulse(0x2,1500);
	command(0,0x30);
	next_wait();
	center(0x1,-1500);   
	center(0x2,0);
	pulse(0x1,-1500);
	pulse(0x2,1500);
	command(0,0x33);
	next_wait();
	pulse(0x1,-4500);  
	pulse(0x2,0);
	command(0,0x30);
	next_wait();
	center(0x1,0);   
	center(0x2,-1500);
	pulse(0x1,-1500);
	pulse(0x2,-1500);
	command(0,0x33);
	next_wait();
	pulse(0x1,0);    
	pulse(0x2,-1500);
	command(0,0x30);
	next_wait();
	center(0x1,1500);   
	center(0x2,0);
	pulse(0x1,1500);
	pulse(0x2,-1500);
	command(0,0x33);
	waitaxis(0x3);
	range(0x6,800000);   
	acc(0x6,400);    
	startv(0x6,50);    
	speed(0x6,3000);   
	pulse(0x2,50000);  
	pulse(0x4,10000);  
	compp(0x2,15000);  
	lp(0x6,0);   
	syncmode(0x2,0x2001,0x0000);  
	    
	    
	syncmode(0x4,0x0000,0x0001);  
	     
	command(0x2,0x20);   
	waitaxis(0x6);  
	return 0;
}
*/
