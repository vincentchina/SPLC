#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "exti.h"
#include "pwm.h"
#include "adc.h"
#include "spi.h"
#include "w25qxx.h"
#include "sram.h" 
#include "can.h"
#include "ext_io.h"
#include "tc6014.h"
#include "iwdg.h"
#include "stmflash.h"
//ALIENTEK 探索者STM32F407开发板 实验36
//外部SRAM 实验 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//FLASH模拟IIC----------------------------------------------------------------------------------
//要写入到STM32 FLASH的字符串数组
const u8 IN_TEXT_Buffer[]={"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(IN_TEXT_Buffer)	 		  	//数组长度	
#define IN_SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0) 

//FLASH-----------------------------------------------------------------------------------------
//要写入到W25Q16的字符串数组
const u8 TEXT_Buffer[]={"Explorer STM32F4 SPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	

//----------------------------------------------------------------------------------------------

int main(void)
{        	 
 	u8 i=0;
	u8 key = 0;
	
	u8 led_cnt=0;
	
	u16 led0pwmval=0;    
	u8 dir=1;
//	u16 adcx;
//	float temp;
	
	u32 ts=0;
	
	u8 datatemp[SIZE];
	u32 FLASH_SIZE;
	
	u8 can_cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1;//CAN工作模式;0,普通模式;1,环回模式
	
//	union WR2 wr2_t;
//	uint8_t s = sizeof(uint8_t); //1
//	s = sizeof(uint16_t);					//2
//	s = sizeof(uint32_t);					//4
//	s = sizeof(int);							//4
//	s = sizeof(short);						//2
//	s = sizeof(long);							//4
//	s = sizeof(float);						//4
//	res = s;											//防止编译器会优化掉s

//	wr2_t.all |= 0x0001;
//	res = wr2_t.all&0x0001;
//	
//	wr2_t.bit.ALM_E = 1;
//	res = wr2_t.bit.ALM_E;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	uart2_init(9600);
	
	LED_Init();					//初始化LED,同时初始化STM32输出到TC6014的极限点引脚
//	EXTIO_Init();
//	EXTIX_Init();       //初始化外部中断输入（包含急停开关和TC6014的中断） 
//	TIM3_PWM_Init(500-1,84-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.
//	Adc_Init();         //初始化ADC
//	W25QXX_Init();			//W25QXX初始化
 	FSMC_SRAM_Init();			//初始化外部SRAM,TC6014和扩展IO的FSMC接口  
//	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);//CAN初始化环回模式,波特率500Kbps
//	
//	while(W25QXX_ReadID()!=W25Q128)								//检测不到W25Q128
//	{
//		printf("W25Q128 Check Failed!\n");
//		delay_ms(500);
//		printf("Please Check!      \n");
//		delay_ms(500);
//		LED0=!LED0;		//DS0闪烁
//	}
//	printf("W25Q128 Ready!\n"); 
//	FLASH_SIZE=16*1024*1024;	//FLASH 大小为16字节
	
	//while(1)
		printf("the testsram is:\n");
//	
//	for(ts=0;ts<250000;ts++)
//	{
//		testsram[ts]=ts;//预存测试数据	
//		//printf("%d\t", testsram[ts]);
////		PDout(14) = 0;
////		PDout(14) = 1;
//	}
	printf("\n");
	
//	fsmc_sram_test();//测试SRAM容量	
//	testsram[0] = 0xa5;	
	
	//testio[1] = 0x005a;
//	IWDG_Init(4,500); //与分频数为64,重载值为500,溢出时间为1s	
	
	//key = 1;
  while(1)
	{	
//		IWDG_Feed();//喂狗
		//SRAM测试----------------------------------------------------------------------------------
//		if(key==1)
//		{
//			key = 0;
//			fsmc_sram_test();//测试SRAM容量
//		}
//		else if(key==2)
//		{
//			for(ts=0;ts<250000;ts++)
//				printf("%d",testsram[ts]);//显示测试数据	
//			printf("\n");
//		}
		
		//FLASH测试---------------------------------------------------------------------------------
//		else if(key==3)
//		{
//			printf("Start Write W25Q128....\n");
//			W25QXX_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
//			printf("W25Q128 Write Finished!\n");	//提示传送完成
//		}
//		else if(key==4)
//		{
//			printf("Start Read W25Q128.... \n");
//			W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
//			printf("The Data Readed Is:   \n");	//提示传送完成
//			printf("%s\n",datatemp);					//显示读到的字符串
//		}
		
		//adc测试-----------------------------------------------------------------------------------
//		else if(key==5)
//		{
///*
//			adcx=Get_Adc_Average(ADC_Channel_10,20);//获取通道10的转换值，20次取平均
//			printf("the origin adc value is %d\n",adcx);    //显示ADCC采样后的原始值
//			temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
//			adcx=temp;                            //赋值整数部分给adcx变量，因为adcx为u16整形
//			printf("the integer value is %d\n",adcx);    //显示电压值的整数部分，3.1111的话，这里就是显示3
//			temp-=adcx;                           //把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
//			temp*=1000;                           //小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
//			printf("the decimal value is %f\n",temp); //显示小数部分（前面转换为了整形显示），这里显示的就是111.
//*/
//		}
		
		//can测试-----------------------------------------------------------------------------------
//		else if(key==6)//发送一次数据
//		{
//			printf("the send data is: ");
//			for(i=0;i<8;i++)
//			{
//				canbuf[i]=can_cnt+i;//填充发送缓冲区
//				printf("%d\t",canbuf[i]);	  //显示数据
// 			}
//			printf("\n");
//			res=CAN1_Send_Msg(canbuf,8);//发送8个字节 
//			if(res)printf("Failed\n");		//提示发送失败
//			else printf("OK    \n");	 		//提示发送成功								   
//		}
//		else if(key==7)//改变CAN的工作模式
//		{	   
//			mode=!mode;
//			CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,mode);	//CAN普通模式初始化,普通模式,波特率500Kbps 
//			if(mode==0)//普通模式，需要2个开发板
//			{
//				printf("Nnormal Mode \n");	    
//			}else //回环模式,一个开发板就可以测试了.
//			{
// 				printf("LoopBack Mode\n");
//			}
//		}
//		
//		//flash模拟IIC
//		else if(key==8)
//		{
//			printf("Start Write FLASH....\n");
//			STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)IN_TEXT_Buffer,IN_SIZE);
//			printf("FLASH Write Finished!\n");//提示传送完成
//		}
//		else if(key==9)
//		{
//			printf("Start Read FLASH.... \n");
//			STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,IN_SIZE);
//			printf("The Data Readed Is:  \n");//提示传送完成
//			printf("%s\n",datatemp);//显示读到的字符串
//		}
		
//		else 
			delay_ms(10);
		
//		if(dir)led0pwmval++;//dir==1 led0pwmval递增
//		else led0pwmval--;	//dir==0 led0pwmval递减 
// 		if(led0pwmval>300)dir=0;//led0pwmval到达300后，方向为递减
//		if(led0pwmval==0)dir=1;	//led0pwmval递减到0后，方向改为递增
//		TIM_SetCompare1(TIM3,led0pwmval);	//修改比较值，修改占空比
//		TIM_SetCompare2(TIM3,led0pwmval);	//修改比较值，修改占空比
//		
//		can_cnt=CAN1_Receive_Msg(canbuf);
//		if(can_cnt)//接收到有数据
//		{	
//			printf("the receive data is: ");		
// 			for(i=0;i<can_cnt;i++)			
//				printf("%d\t",canbuf[i]);	  //显示数据
//			printf("\n");
//		}
		
		led_cnt++;
		if(led_cnt==20)//DS0闪烁.
		{
			led_cnt=0;
			//printf("the testsram is:\n");
			//if(testsram[0]==0xa5)
				LED0=!LED0;
			can_cnt++;
 		}
	}
}
