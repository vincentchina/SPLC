#ifndef __TC6014_H
#define __TC6014_H

#include "sys.h"
#include "sram.h"
#include "usart.h"

#define TC6014A_BASE_ADDR Bank1_SRAM1_ADDR
#define TC6014B_BASE_ADDR Bank1_SRAM2_ADDR

#define	TC6014A 0
#define	TC6014B 1

#define outpw(port, data) (*(volatile unsigned short *)(port)) = data
#define inpw(port) (*(volatile unsigned short *)(port))

//-----tc6014 寄存器地址定义
#define  wr0  0x0  //命令寄存器
#define  wr1  0x2  //模式寄存器1
#define  wr2  0x4  //模式寄存器2
#define  wr3  0x6  //模式寄存器3 
#define  wr4  0x8  //输出寄存器
#define  wr5  0xa  //插补模式寄存器
#define  wr6  0xc  //低位写入数据寄存器 
#define  wr7  0xe  //高位写入数据寄存器

#define  rr0  0x0 //主状态寄存器
#define  rr1  0x2 //状态寄存器1  
#define  rr2  0x4 //状态寄存器2 
#define  rr3  0x6 //状态寄存器3  
#define  rr4  0x8 //输入寄存器1  
#define  rr5  0xa //输入寄存器2  
#define  rr6  0xc //低位读出数据寄存器 
#define  rr7  0xe //高位读出数据寄存器 

#define  bp1p  0x4  //BP第1 轴+方向数据寄存器 
#define  bp1m  0x6 //BP第1 轴-方向数据寄存器  
#define  bp2p  0x8  //BP第2 轴+方向数据寄存器
#define  bp2m  0xa //BP第2 轴-方向数据寄存器  
#define  bp3p  0xc  //BP第3 轴+方向数据寄存器
#define  bp3m  0xe //BP第3 轴-方向数据寄存器

//typedef enum
//{
//	TC6014A = 0,
//	TC6014B
//}ChipEnum;


/*-----------------------wr0------------------------------------------------*/
struct WR0_BITS
{
	uint16_t CMD:7;	//bit0~6 		命令码
	uint16_t rsvd0:1;	//bit7 			保留  	需设置为0
	uint16_t X_AXIS_SEL:1;	//bit8		轴X的选择位
	uint16_t Y_AXIS_SEL:1;	//bit9		轴Y的选择位
	uint16_t Z_AXIS_SEL:1;	//bit10		轴Z的选择位
	uint16_t U_AXIS_SEL:1;	//bit11		轴U的选择位
	uint16_t rsvd1:3;	//bit12~14 	保留  	需设置为0
	uint16_t RESET:1;	//bit15 		复位位  写0x80实现芯片复位，注意其他情况下要设为0
};

union WR0
{
	struct WR0_BITS bit;
	uint16_t all;
};

/*-----------------------wr1------------------------------------------------*/
struct WR1_BITS
{
	uint16_t IN0_L:1;	//bit0 		IN0引脚有效电平位 0：低电平有效    1：高电平有效
	uint16_t IN0_E:1;	//bit1 		IN0引脚使能位     0：不使能        1：使能  
	uint16_t IN1_L:1;	//bit2 		IN1引脚有效电平位 0：低电平有效    1：高电平有效
	uint16_t IN1_E:1;	//bit3 		IN1引脚使能位     0：不使能        1：使能 
	uint16_t IN2_L:1;	//bit4 		IN2引脚有效电平位 0：低电平有效    1：高电平有效
	uint16_t IN2_E:1;	//bit5 		IN2引脚使能位     0：不使能        1：使能 
	uint16_t IN3_L:1;	//bit6 		IN3引脚有效电平位 0：低电平有效    1：高电平有效
	uint16_t IN3_E:1;	//bit7 		IN3引脚使能位     0：不使能        1：使能 
	
	//以下是中断允许/禁止位  1：允许   0：禁止�
	uint16_t PLUSE:1;	//bit8 		每一个驱动脉冲上升沿，中断将发生（驱动脉冲用正逻辑）
	uint16_t P_MTE_CN:1;	//bit9  逻辑/实位计数器的数值 >= COMP-寄存器的数值时，中断发生
	uint16_t P_LT_CN:1;	//bit10 逻辑/实位计数器的数值 < COMP-寄存器的数值时，中断发生
	uint16_t P_LT_CP:1;	//bit11 逻辑/实位计数器的数值 >= COMP+寄存器的数值时，中断发生
	uint16_t P_MTE_CP:1;	//bit12 逻辑/实位计数器的数值 < COMP+寄存器的数值时，中断发生
	uint16_t C_END:1;	//bit13   加减速驱动时，在定速域结束脉冲输出后，中断发生
	uint16_t C_STA:1;	//bit14   加减速驱动时，在定速域开始脉冲输出后，中断发生
	uint16_t E_END:1;	//bit15   驱动结束时，中断发生
};

union WR1
{
	struct WR1_BITS bit;
	uint16_t all;
};

/*-----------------------wr2------------------------------------------------*/
struct WR2_BITS
{
	uint16_t SLMTP:1;	//bit0 	  是否把COMP+寄存器作为软件限制 0：否   1：是
	uint16_t SLMTN:1;	//bit1 		是否把COMP-寄存器作为软件限制 0：否   1：是
	uint16_t LMTMD:1;	//bit2 		硬件限制有效时的脉冲停止方式  0：立即停止  1：减速停止
	uint16_t HLMTP:1;	//bit3 		正方向限制输入信号(nLMTP)的逻辑电平  0：低有效   1：高有效
	uint16_t HLMTN:1;	//bit4 		负方向限制输入信号(nLMTN)的逻辑电平  0：低有效   1：高有效
	uint16_t CMPSL:1;	//bit5 		把COMP+/-寄存器的比较对象设定为逻辑位置计数器或者实际位置计数器
	uint16_t PLSMD:1;	//bit6 		驱动脉冲的输出方式  0：独立2脉冲方式   1：脉冲+方向方式
	uint16_t PLS_L:1;	//bit7 		驱动脉冲的逻辑电平  0：正逻辑    1：负逻辑
	uint16_t DIR_L:1; //bit8 		方向引脚逻辑电平  0：正方向(L) 负方向(H)  1：正方向(H) 负方向(L) 
	uint16_t PINMD:1;	//bit9    编码器输入信号模式 0：2相脉冲输入   1：上/下脉冲输入
	uint16_t PIND:2;	//bit10~11 	2相脉冲输入分周比  00：1/1  01：1/2  10：1/4  11：无效
	uint16_t ALM_L:1;	//bit12   输入信号(nALARM)的逻辑电平 0：低有效   1：高有效
	uint16_t ALM_E:1;	//bit13   是否使能接收伺服驱动器的nALARM信号  0：否   1：是
	uint16_t INP_L:1;	//bit14   输入信号(nINPOS)的逻辑电平 0：低有效   1：高有效
	uint16_t INP_E:1;	//bit15   是否使能接收伺服驱动器的nINPOS信号  0：否   1：是
};

union WR2
{
	struct WR2_BITS bit;
	uint16_t all;
};

/*-----------------------wr3------------------------------------------------*/
struct WR3_BITS
{
	uint16_t MANLD:1;	//bit0 	  把加/减速定量驱动的减速设定为自动减速或手动减速 0：自动减速   1：手动减速
	uint16_t DSNDE:1;	//bit1 		把加/减速驱动时的减速速度设定为加速度的数值或者独立的减速度的数值 0：使用加速度的数值   1：使用减速度的数值
	uint16_t SACC:1;	//bit2 		设定直线加/减速或者S曲线加/减速 0：直线加/减速  1：S曲线加/减速（使用前一定要设好加/减速度的变化率）  
	uint16_t EXOP0:2;	//bit3~4 		根据外部输入信号(nEXPP,nEXPM)设定驱动操作 00/11：无效   01：连续脉冲驱动  10：定量脉冲驱动
	uint16_t rsvd0:2;	//bit5~6 	保留  	需设置为0	
	uint16_t OUTSL:1;	//bit7 		选择输出信号nOUT7~4为通用输出还是输出驱动状态  0：通用  1：状态
													/*
													nOUT4/CMPP   当逻辑/实际计数器的值 >= COMP+寄存器时，输出高电平，否则输出低电�
													nOUT5/CMPM   当逻辑/实际计数器的值 >= COMP-寄存器时，输出高电平，否则输出低电平
													nOUT6/ASND   驱动命令正在执行，并处于加速状态时，输出高电平，否则输出低电平
													nOUT7/DSND   驱动命令正在执行，并处于减速状态时，输出高电平，否则输出低电平�
													*/
	uint16_t OUT4:1;	//bit8 		把输出信号nOUT4作为通用输出使用时的数值  0：低电平输出   1：高电平输出
	uint16_t OUT5:1;	//bit9 		把输出信号nOUT5作为通用输出使用时的数值  0：低电平输出   1：高电平输出
	uint16_t OUT6:1;	//bit10 		把输出信号nOUT6作为通用输出使用时的数值  0：低电平输出   1：高电平输出
	uint16_t OUT7:1;	//bit11 		把输出信号nOUT7作为通用输出使用时的数值  0：低电平输出   1：高电平输出
	uint16_t rsvd1:4;	//bit12~15 	保留  	需设置为0	
};

union WR3
{
	struct WR3_BITS bit;
	uint16_t all;
};

/*-----------------------wr4------------------------------------------------*/
struct WR4_BITS
{
	uint16_t XOUT0:1;	//bit0 	  XOUT0通用输出 0：低电平输出   1：高电平输出
	uint16_t XOUT1:1;	//bit1 	  XOUT1通用输出 0：低电平输出   1：高电平输出
	uint16_t XOUT2:1;	//bit2 	  XOUT2通用输出 0：低电平输出   1：高电平输出
	uint16_t XOUT3:1;	//bit3 	  XOUT3通用输出 0：低电平输出   1：高电平输出
	
	uint16_t YOUT0:1;	//bit4 	  YOUT0通用输出 0：低电平输出   1：高电平输出
	uint16_t YOUT1:1;	//bit5 	  YOUT1通用输出 0：低电平输出   1：高电平输出
	uint16_t YOUT2:1;	//bit6 	  YOUT2通用输出 0：低电平输出   1：高电平输出
	uint16_t YOUT3:1;	//bit7 	  YOUT3通用输出 0：低电平输出   1：高电平输出
	
	uint16_t ZOUT0:1;	//bit8 	  ZOUT0通用输出 0：低电平输出   1：高电平输出
	uint16_t ZOUT1:1;	//bit9 	  ZOUT1通用输出 0：低电平输出   1：高电平输出
	uint16_t ZOUT2:1;	//bit10 	  ZOUT2通用输出 0：低电平输出   1：高电平输出
	uint16_t ZOUT3:1;	//bit11 	  ZOUT3通用输出 0：低电平输出   1：高电平输出

	uint16_t UOUT0:1;	//bit12 	  UOUT0通用输出 0：低电平输出   1：高电平输�
	uint16_t UOUT1:1;	//bit13 	  UOUT1通用输出 0：低电平输出   1：高电平输出
	uint16_t UOUT2:1;	//bit14 	  UOUT2通用输出 0：低电平输出   1：高电平输出
	uint16_t UOUT3:1;	//bit15 	  UOUT3通用输出 0：低电平输出   1：高电平输出�
};

union WR4
{
	struct WR4_BITS bit;
	uint16_t all;
};

/*-----------------------wr5------------------------------------------------*/
struct WR5_BITS
{
	uint16_t AX1:2;	//bit0~1 	  指定运行插补驱动的第一轴（主轴）
	uint16_t AX2:2;	//bit2~3 	  指定运行插补驱动的第二轴
	uint16_t AX3:2;	//bit4~5 	  指定运行插补驱动的第三轴
							/*
							轴			编码
							X   		00
							Y				01
							Z				10
							U				11
							*/
	uint16_t rsvd0:2;	//bit6~7 	  保留  	需设置为0	
	
	uint16_t LSPD:2;	//bit8~9 	  设定插补驱动的固定线速度模式
							/*
					bit9:bit8		操作模式
							00		线速度恒定无效
							01		2轴线速度恒定
							10		（不能设定）
							11		3轴线速度恒定
							*/

	uint16_t rsvd1:1;	//bit10 	  保留  	需设置为0	
	
	uint16_t EXPLS:1;	//bit11 	  是否根据外部信号(EXPLSN)进行步进插补驱动 0：否   1：是�
	uint16_t CMPLS:1;	//bit12 	  是否根据命令进行步进插补驱动 0：否   1：是�
	uint16_t rsvd2:1;	//bit13 	  保留  	需设置为0
	uint16_t CIINT:1;	//bit14 	  是否使能连续插补中断 0：否   1：是�
	uint16_t BPINT:1;	//bit15 	  是否使能位模式插补中断 0：否   1：是�
};

union WR5
{
	struct WR5_BITS bit;
	uint16_t all;
};

/*-----------------------tc6014寄存器------------------------------------------------*/
typedef struct TC6014_Wregs
{
	//TC6014的寄存器
	union WR0 _wr0;
	union WR1 _wr1[4];
	union WR2 _wr2[5];
	union WR3 _wr3[5];
	union WR4 _wr4[2];
	union WR5 _wr5[2];
	uint16_t _wr6[2];
	uint16_t _wr7[2];
	
	//TC6014的参数
	uint32_t R; //range  范围
	uint32_t SV; //start velocity 初始速度
	uint32_t V;  //drive velocity 驱动速度
	uint32_t A;  //acceleration 加速度
	uint32_t D;  //deceleration 加速度
	uint32_t K;  //jerk  加速度变化率
	uint32_t L;  //deceleration increase rate 减速度变化率
	
	uint8_t clock; //时钟=fclk/16000000，fclk(外部晶振频率)一般为16MHz，即clock = 1
	uint16_t multiple; //倍率=8000000/R，最大为500，即R=16000时
	uint32_t initial_speed; //初始速度(pps)=SV*multiple*clock
	uint32_t drive_speed; //驱动速度(pps)=V*multiple*clock
	uint32_t acceleration; //加速度(pps/sec)=A*125*multiple*clock*clock
	uint32_t deceleration; //减速度(pps/sec)=D*125*multiple*clock*clock
	uint32_t jerk; //加速度变化率(pps/sec/sec/sec)=62.5*1000000*multiple*clock*clock*clock/K
	uint32_t dec_inc_tate; //减速度变化率(pps/sec/sec/sec)=62.5*1000000*multiple*clock*clock*clock/L
}TC6014_TypeDef;

extern TC6014_TypeDef* ptc6014_wregs;
extern uint32_t adr;

void select_chip(int chip);
int which_chip(void);
void wreg1(int axis,int wdata);
void wreg2(int axis,int wdata);
void wreg3(int axis,int wdata);
void command(int axis,int cmd);
void range(int axis,long wdata);
void acac(int axis,int wdata);
void dcac(int axis,int wdata);
void acc(int axis,int wdata);
void dec(int axis,int wdata);
void startv(int axis,int wdata);
void speed(int axis,int wdata);
void pulse(int axis,long wdata);
void decp(int axis,long wdata);
void center(int axis,long wdata);
void lp(int axis,long wdata);
void ep(int axis,long wdata);
void compp(int axis,long wdata);
void compm(int axis,long wdata);
void accofst(int axis,long wdata);
void hsspeed(int axis,int wdata);
void expmode(int axis,int em6data,int em7data);
void syncmode(int axis,int sm6data,int sm7data);
long readlp(int axis);
long readep(int axis);
void waitaxis(int axis);
void next_wait(void);
void bp_wait(void);
void homesrch(void);


void test(char flag);
	
#endif
