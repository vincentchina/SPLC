#include "Cmc8401.h"
#include "tc6014.h"

int __stdcall c8401_board_initial(void)
{
	int  count;
	outpw(adr+wr0, 0x8000);//软件复位
	for(count = 0; count < 2; ++count);
	
	//------------全轴模式设定------------
	command(0xf,0xf);//轴切换，所有轴
	outpw(adr+wr1, 0x0000);//模式寄存器1 00000000 00000000  
	outpw(adr+wr2, 0xe000);//模式寄存器2 11100000 00000000  
	outpw(adr+wr3, 0x0000);//模式寄存器3 00000000 00000000  
	expmode(0xf,0x5d08,0x497f); //所有轴外部模式设定 

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
	
	return 2;
}

void __stdcall c8401_board_close(void){}
	
BOOL __stdcall c8401_set_pulse_mode(WORD cardno, WORD axis, WORD pulsemode)
{
	int axis_t = 0;
	//芯片选择
	select_chip((int)cardno);
	
	//记录轴
	axis_t = (0x01<<axis);
	ptc6014_wregs->_wr0.all = (uint16_t)(axis_t << 8);//记录wr0
	
	//记录要写的寄存器位
	switch(pulsemode)
	{
		case 0:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 0;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 0;
			break;
		case 1:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 0;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 1;
			break;
		case 2:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 0;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 1;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 0;
			break;
		case 3:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 0;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 1;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 1;
			break;
		case 4:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 1;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 0;
			break;
		case 5:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 1;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 1;
			break;
		case 6:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 1;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 1;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 0;
			break;
		case 7:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 1;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 1;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 1;
			break;
		default:
			ptc6014_wregs->_wr2[axis].bit.PLSMD = 1;
			ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;
			ptc6014_wregs->_wr2[axis].bit.DIR_L = 0;//其他值时，设置为默认值4
			break;
	}
	wreg2(axis_t, ptc6014_wregs->_wr2[axis].all);//写寄存器
	return 1;
}

BOOL __stdcall c8401_set_encoder_mode(WORD cardno, WORD axis, WORD pulsemode, WORD dir, WORD countmode)
{
	int axis_t = 0;
	select_chip((int)cardno);
	
	axis_t = (0x01<<axis);
	ptc6014_wregs->_wr0.all = (axis_t << 8);//记录wr0
	
	if(pulsemode)//上下脉冲方式
		ptc6014_wregs->_wr2[axis].bit.PINMD = 1;//D9 = 1为上下脉冲方式
	else
		ptc6014_wregs->_wr2[axis].bit.PINMD = 0;
	
	if(dir)//其实这个也是正负逻辑位D7
		ptc6014_wregs->_wr2[axis].bit.PLS_L = 1;//负逻辑
	else
		ptc6014_wregs->_wr2[axis].bit.PLS_L = 0;//正逻辑
	
	switch(countmode)//设置D10~D11
	{
		case 0:
			ptc6014_wregs->_wr2[axis].bit.PIND = 2;//4倍
			break;
		case 1:
			ptc6014_wregs->_wr2[axis].bit.PIND = 1;//2倍
			break;
		case 2:
			ptc6014_wregs->_wr2[axis].bit.PIND = 0;//1倍
			break;
		default:
			ptc6014_wregs->_wr2[axis].bit.PIND = 2;//其他值默认4倍
			break;
	}
	wreg2(axis_t, ptc6014_wregs->_wr2[axis].all);//写寄存器
	return 1;
}

BOOL __stdcall c8401_set_t_profile(WORD cardno, WORD axis, long sspd, long rspd, double tacc, double tdec)
{
	int axis_t = 0;
	select_chip((int)cardno);
	
	axis_t = (0x01<<axis);
	ptc6014_wregs->_wr0.all = (axis_t << 8);//记录wr0
	
	
	return 1;
}

