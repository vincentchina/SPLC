#include "tc6014.h"

TC6014_TypeDef tc6014s_wregs[2];
TC6014_TypeDef* ptc6014_wregs;

uint32_t adr = TC6014A_BASE_ADDR;

//¸Ä±ä²Ù×÷µÄÐ¾Æ¬
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

//ÕýÔÚ²Ù×÷ÄÄ¸öÐ¾Æ¬
int which_chip(void)
{
	if(adr == TC6014A_BASE_ADDR)
		return TC6014A;
	else
		return TC6014B;
}

// wreg1(ÖáÖ¸¶¨,Êý¾Ý)------------Ð´Èë¼Ä´æÆ÷1 Éè¶¨
void wreg1(int axis,int wdata)
{
	//D8~D11Î»ÎªXYZUÖáÑ¡ÔñÎ»£¬ËùÒÔ
	// 1      ´ú±íµÚ1Öá
	// 2      ´ú±íµÚ2Öá
	// 4      ´ú±íµÚ3Öá
	// 8      ´ú±íµÚ4Öá
	//0x0fÊÇÖáÇÐ»»ÃüÁî
	outpw(adr+wr0, (axis << 8) + 0xf);//ÖáÖ¸¶¨
	outpw(adr+wr1, wdata);
}

// wreg2(ÖáÖ¸¶¨,Êý¾Ý)-------------Ð´Èë¼Ä´æÆ÷2 Éè¶¨
void wreg2(int axis,int wdata)
{
	outpw(adr+wr0, (axis << 8) + 0xf);//ÖáÖ¸¶¨
	outpw(adr+wr2, wdata);
}

// wreg3(ÖáÖ¸¶¨,Êý¾Ý)-------------Ð´Èë¼Ä´æÆ÷3 Éè¶¨
void wreg3(int axis,int wdata)
{
	outpw(adr+wr0, (axis << 8) + 0xf);//ÖáÖ¸¶¨
	outpw(adr+wr3, wdata);
}

// command(ÖáÖ¸¶¨,ÃüÁî±àÂë)-----ÃüÁîÐ´Èë
void command(int axis,int cmd)
{
	outpw(adr+wr0, (axis << 8) + cmd);
}

// range(ÖáÖ¸¶¨,Êý¾Ý)----------------·¶Î§(R) Éè¶¨
void range(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x00);
}

// acac(ÖáÖ¸¶¨,Êý¾Ý)----------------±ä»¯ÂÊ(K)Éè¶¨
void acac(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x01);
}

// dcac(ÖáÖ¸¶¨,Êý¾Ý)----------------±ä»¯ÂÊ(L)Éè¶¨
void dcac(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x0e);
}


// acc(ÖáÖ¸¶¨,Êý¾Ý)------------------¼Ó/¼õËÙ¶È(A)Éè¶¨
void acc(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x02);
}

// dec(ÖáÖ¸¶¨,Êý¾Ý) -------------------¼õËÙ¶È(D) Éè¶¨
void dec(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x03);
}

// startv(ÖáÖ¸¶¨,Êý¾Ý) -----------------³õÊ¼ËÙ¶È(SV) Éè¶¨
void startv(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x04);
}

// speed(ÖáÖ¸¶¨,Êý¾Ý) -----------------Çý¶¯ËÙ¶È(V) Éè¶¨
void speed(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x05);
}

// pulse(ÖáÖ¸¶¨,Êý¾Ý) -----------------Êä³öÂö³åÊý/ÖÕµã(P) Éè¶¨
void pulse(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x06);
}

// decp(ÖáÖ¸¶¨,Êý¾Ý) -----------------ÊÖ¶¯¼õËÙµã(DP) Éè¶¨
void decp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x07);
}

// center(ÖáÖ¸¶¨,Êý¾Ý) ----------------Ô²»¡ÖÐÐÄµã(C) Éè¶¨
void center(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x08);
}

// lp(ÖáÖ¸¶¨,Êý¾Ý) --------------------Âß¼­Î»ÖÃ¼ÆÊýÆ÷(LP)Éè¶¨
void lp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x09);
}

// ep(ÖáÖ¸¶¨,Êý¾Ý) ------------------Êµ¼ÊÎ»ÖÃ¼ÆÊýÆ÷(EP)Éè¶¨
void ep(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0a);
}

// compp(ÖáÖ¸¶¨,Êý¾Ý) ------------COMP + (CP) Éè¶¨
void compp(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0b);
}

// compm(ÖáÖ¸¶¨,Êý¾Ý) -----------COMP - (CM) Éè¶¨
void compm(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0c);
}

// accofst(ÖáÖ¸¶¨,Êý¾Ý) -----------¼ÓËÙ¼ÆÊýÆ÷Æ«ÒÆ(AO)Éè¶¨
void accofst(int axis,long wdata)
{
	outpw(adr+wr7, (wdata >> 16) & 0xffff);
	outpw(adr+wr6, wdata & 0xffff);
	outpw(adr+wr0, (axis << 8) + 0x0d);
}

//---------------------------------Ô­µã¼ì²âËÙ¶È(HV)Éè¶¨
void hsspeed(int axis,int wdata)
{
	outpw(adr+wr6, wdata);
	outpw(adr+wr0, (axis << 8) + 0x61);
}

//---------------------------------Íâ²¿Ä£Ê½(EM)Éè¶¨
void expmode(int axis,int em6data,int em7data)
{
	outpw(adr+wr6, em6data);
	outpw(adr+wr7, em7data);
	outpw(adr+wr0, (axis << 8) + 0x60);
}

//---------------------------------Í¬²½²Ù×÷Ä£Ê½(SM)Éè¶¨
void syncmode(int axis,int sm6data,int sm7data)
{
	outpw(adr+wr6, sm6data);
	outpw(adr+wr7, sm7data);
	outpw(adr+wr0, (axis << 8) + 0x64);
}

// readlp(ÖáÖ¸¶¨)---------------------Âß¼­Î»ÖÃ¼ÆÊýÆ÷ÊýÖµ(LP)¶Á³ö
long readlp(int axis)
{
	long a;long d6;long d7;
	outpw(adr+wr0, (axis << 8) + 0x10);
	d6 = inpw(adr+rr6);d7 = inpw(adr+rr7);
	a = d6 + (d7 << 16);
	return(a);
}

// readep(ÖáÖ¸¶¨)--------------------Êµ¼ÊÎ»ÖÃ¼ÆÊýÆ÷ÊýÖµ(EP)¶Á³ö
long readep(int axis)
{
	long a;long d6;long d7;
	outpw(adr+wr0, (axis << 8) + 0x11);
	d6 = inpw(adr+rr6);d7 = inpw(adr+rr7);
	a = d6 + (d7 << 16);
	return(a);
}

// wait(ÖáÖ¸¶¨)------------------------µÈÇý¶¯½áÊø
void waitaxis(int axis)
{
	while(inpw(adr+rr0) & axis);
}
             
// next_wait( )--------------------------µÈÁ¬Ðø²å²¹ÏÂÒ»¸öÊý¾ÝÉè¶¨
void next_wait(void)       
{
	while((inpw(adr+rr0) & 0x0200) == 0x0);
}

// bp_wait( )----------------------------µÈBP ²å²¹ÏÂÒ»¸öÊý¾ÝÉè¶¨
void bp_wait(void)        
{
	while((inpw(adr+rr0) & 0x6000) == 0x6000);
}

// homesrch( )---------------------È«ÖáÔ­µã¼ì³ö
void homesrch(void)
{     
	speed(0x3,2000);// 1¡¢2ÖáµÄËÙ¶ÈÊÇ2000   
	hsspeed(0x3,50);   
	pulse(0x1,3500); //µÚÒ»ÖáµÄÂö³åÊý 
	pulse(0x2,700);  //µÚ¶þÖáµÄÂö³åÊý 

	     
	speed(0x4,40);  //µÚ3ÖáµÄËÙ¶ÈÊÇ40  
	hsspeed(0x4,40);   
	pulse(0x4,20);  //µÚÈýÖáµÄÂö³åÊý  
	     
	hsspeed(0x8,30);  
	command(0xf,0x62);//ËùÓÐÖá×Ô¶¯Ô­µã»Ø¹é   
	waitaxis(0xf);  //µÈ´ýÇý¶¯½áÊø  
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
	outpw(adr+wr0, 0x8000);//Èí¼þ¸´Î»
	for(count = 0; count < 50; ++count);

	//------------È«ÖáÄ£Ê½Éè¶¨------------
	command(0xf,0xf);//µÚÒ»¶þÖáÇÐ»»£¬¼´Ñ¡¶¨µÚÒ»¶þÖá
	outpw(adr+wr1, 0x0000);//Ä£Ê½¼Ä´æÆ÷1 00000000 00000000  
	outpw(adr+wr2, 0x0000);//Ä£Ê½¼Ä´æÆ÷2 00000000 00000000  
	outpw(adr+wr3, 0x0000);//Ä£Ê½¼Ä´æÆ÷3 00000000 00000000  
	//expmode(0x3,0x5d08,0x497f); //µÚÒ»¶þÖáÍâ²¿Ä£Ê½Éè¶¨ 

	//--------È«Öá²Ù×÷²ÎÊý³õÆÚÉè¶¨----
	accofst(0xf,0);//A0 = 0 £¬¼´Æ«ÒÆÎª0£¬Ä¬ÈÏÎª8µÄ   
	range(0xf,800000); //R = 800,000(±¶ÂÊ = 10) 
	acac(0xf,1010); //K = 1,010(±ä»¯ÂÊ = 619KPPS/SEC2)   
	dcac(0xf,1010); //L = 1,010(±ä»¯ÂÊ = 619KPPS/SEC2)    
	acc(0xf,100);  //A = 100(¼Ó/¼õËÙ¶È=125KPPS/SEC)  
	dec(0xf,100); //D = 100(¼õËÙ¶È = 125KPPS/SEC)  
	startv(0xf,100);  //SV = 100(³õÊ¼ËÙ¶È = 1,000PPS) 
	speed(0xf,4000);  //V = 4,000(Çý¶¯ËÙ¶È = 40,000PPS) 
	pulse(0xf,100000); //P = 100,000(Êä³öÂö³åÊý = 100,000)  
	lp(0xf,0);  //LP = 0(Âß¼­Î»ÖÃ¼ÆÊýÆ÷ = 0)
	ep(0xf,0);  //EP = 0(Êµ¼ÊÎ»ÖÃ¼ÆÊýÆ÷ = 0)   
	
	//wr4ºÍwr5ËùÓÐÖá¹«ÓÃá
	outpw(adr+wr4, 0x0303); //nOUT3~0Êä³öµÍµçÆ½      
	//outpw(adr+wr5, 0x0124); //2ÖáÏßËÙ¶Èºã¶¨£¬XÎªÖ÷Öá£¬YÎª´ÓÖá£¬ZÎª´ÓÖá
	
//	acc(0x3,200);   
//	speed(0x3,4000);  
//	pulse(0x1,80000);  
//	pulse(0x2,40000);   
	//command(0xf,0x20); //Õý·½Ïò¶¨Á¿Çý¶¯ 
	if(flag)
		command(0xf,0x20); //Õý·½Ïò¶¨Á¿Çý¶¯
	else
		command(0xf,0x21); //Õý·½Ïò¶¨Á¿Çý¶¯
	waitaxis(0xf);
}

/*
int main(void)
{
	int  count;
	outpw(adr+wr0, 0x8000);//Èí¼þ¸´Î»
	for(count = 0; count < 2; ++count);

	//------------È«ÖáÄ£Ê½Éè¶¨------------
	command(0x3,0xf);//µÚÒ»¶þÖáÇÐ»»£¬¼´Ñ¡¶¨µÚÒ»¶þÖá
	outpw(adr+wr1, 0x0000);//Ä£Ê½¼Ä´æÆ÷1 00000000 00000000  
	outpw(adr+wr2, 0xe000);//Ä£Ê½¼Ä´æÆ÷2 11100000 00000000  
	outpw(adr+wr3, 0x0000);//Ä£Ê½¼Ä´æÆ÷3 00000000 00000000  
	expmode(0x3,0x5d08,0x497f); //µÚÒ»¶þÖáÍâ²¿Ä£Ê½Éè¶¨ 

	//--------È«Öá²Ù×÷²ÎÊý³õÆÚÉè¶¨----
	accofst(0x3,0);//A0 = 0 £¬¼´Æ«ÒÆÎª0£¬Ä¬ÈÏÎª8µÄ   
	range(0x3,800000); //R = 800,000(±¶ÂÊ = 10) 
	acac(0x3,1010); //K = 1,010(±ä»¯ÂÊ = 619KPPS/SEC2)   
	dcac(0x3,1010); //L = 1,010(±ä»¯ÂÊ = 619KPPS/SEC2)    
	acc(0x3,100);  //A = 100(¼Ó/¼õËÙ¶È=125KPPS/SEC)  
	dec(0x3,100); //D = 100(¼õËÙ¶È = 125KPPS/SEC)  
	startv(0x3,100);  //SV = 100(³õÊ¼ËÙ¶È = 1,000PPS) 
	speed(0x3,4000);  //V = 4,000(Çý¶¯ËÙ¶È = 40,000PPS) 
	pulse(0x3,100000); //P = 100,000(Êä³öÂö³åÊý = 100,000)  
	lp(0x3,0);  //LP = 0(Âß¼­Î»ÖÃ¼ÆÊýÆ÷ = 0)
	ep(0x3,0);  //EP = 0(Êµ¼ÊÎ»ÖÃ¼ÆÊýÆ÷ = 0)  
	
	command(0xc,0xf);  //µÚÈýËÄÖáÇÐ»»£¬¼´Ñ¡¶¨µÚÈýËÄÖá
	outpw(adr+wr1, 0x0000);                                            
	outpw(adr+wr2, 0x0000);  
	outpw(adr+wr3, 0x0000);   
	expmode(0x4,0x5d08,0x01c4); //µÚÈýÖáÍâ²¿Ä£Ê½Éè¶¨  
	expmode(0x8,0x5d08,0x010c); //µÚËÄÖáÍâ²¿Ä£Ê½Éè¶¨  
	 
	accofst(0xc,0);  
	range(0xc,800000);  
	acac(0xc,1010);    
	dcac(0xc,1010);   
	acc(0xc,100);    
	dec(0xc,100);   
	startv(0xc,50); //SV = 50(³õÊ¼ËÙ¶È = 500PPS)   
	speed(0xc,40);  //V = 40(Çý¶¯ËÙ¶È = 400PPS)  
	pulse(0xc,10); //P = 10(Êä³öÂö³åÊý = 10)   
	lp(0xc,0);    
	
	//wr4ºÍwr5ËùÓÐÖá¹«ÓÃá
	outpw(adr+wr4, 0x0000); //nOUT3~0Êä³öµÍµçÆ½      
	outpw(adr+wr5, 0x0124); //2ÖáÏßËÙ¶Èºã¶¨£¬XÎªÖ÷Öá£¬YÎª´ÓÖá£¬ZÎª´ÓÖá 

	     
	acc(0x3,200);   
	speed(0x3,4000);  
	pulse(0x1,80000);  
	pulse(0x2,40000);   
	command(0x3,0x20); //Õý·½Ïò¶¨Á¿Çý¶¯ 
	waitaxis(0x3);                           
	    
	wreg3(0x1, 0x0002); //µÚÒ»ÖáÊ¹ÓÃ¼õËÙ¶ÈµÄÊýÖµ 
	acc(0x1,200);  
	dec(0x1,50);   
	speed(0x1,4000);   
	pulse(0x1,80000);   
	command(0x1,0x20);  
	waitaxis(0x1);                         
	wreg3(0x1, 0x0000);                  
	    
	wreg3(0x3, 0x0004); //µÚÒ»¶þÖáÊ¹ÓÃsÇúÏß¼Ó/¼õËÙ  
	acac(0x3,1010);    
	acc(0x3,200);    
	speed(0x3,4000);   
	pulse(0x1,50000);   
	pulse(0x2,25000);   
	command(0x3,0x21);  //¸º·½Ïò¶¨Á¿Çý¶¯  
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
	command(0,0x36); //Î»Ä£Ê½²å²¹ 
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
