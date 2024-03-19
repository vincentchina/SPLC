#ifndef __PLC_INSTRUCTION_H__
#define __PLC_INSTRUCTION_H__

#ifdef __cplusplus
extern "C"{
#endif
//#include "..\..\PLCCore\include\insClass.h"
#define MAX_INS_LEN	( 0x10 + 1 )

//三菱PLC指令系统
enum e_sinsMap{
	sinsLD		= 0x00,	
	sinsLDI		= 0x01,
	sinsLDP		= 0x0202,
	sinsLDF		= 0x0302,

	sinsAND		= 0x0c,
	sinsANI		= 0x0d,
	sinsANDP	= 0x0e02,
	sinsANDF	= 0x0f02,
	sinsOR		= 0x06,
	sinsORI		= 0x07,
	sinsORP		= 0x0802,
	sinsORF		= 0x0902,
	sinsANB		= 0x19,
	sinsORB		= 0x18,

	sinsMPS		= 0x1a,
	sinsMRD		= 0x1b,
	sinsMPP		= 0x1c,
	sinsINV		= 0x14,
	sinsMEP		= 0x13,
	sinsMEF		= 0x12,

	sinsOUT		= 0x20,
	sinsOUTT	= 0x2103,
	sinsOUTC	= 0x2105,
	sinsOUTMS	= 0x2002,
	sinsSET		= 0x23,
	sinsRST		= 0x24,
	sinsPLS		= 0x25,
	sinsPLF		= 0x26,

	sinsMC		= 0x2c03,
	sinsMCR		= 0x3002,

	sinsNOP		= -1,
	sinsEND		= 0x34,

	sinsSTL		= 0x6c0123,
	sinsRET		= 0x6c0124,
	sinsSETMS	= 0x2302,
	sinsRSTMSTC	= 0x2402,		//T、C同一个

	sinsADD		= 0x490728,
	sinsSUB		= 0x49072A,
	sinsMUL		= 0x49072C,
	sinsDIV		= 0x49072E,
	sinsINC		= 0x4A0300,
	sinsDEC		= 0x4A0302,

	sinsDADD	= 0x490D29,
	sinsDSUB	= 0x490D2B,
	sinsDMUL	= 0x490D2D,
	sinsDDIV	= 0x490D2F,
	sinsDINC	= 0x4A0501,
	sinsDDEC	= 0x4A0503,

	sinsDDRVA	= 0x63111D,
	sinsDDRVI	= 0x63111B,
	sinsDRVA	= 0x63091C,
	sinsDRVI	= 0x63091A,

	//sinsAND<	= 0x40050411;
	sinsLD_E	= 0x40050010,
	sinsAND_E	= 0x40050011,
	sinsOR_E	= 0x40050012,
	sinsLD_NE	= 0x40050110,
	sinsAND_NE	= 0x40050111,
	sinsOR_NE	= 0x40050112,
	sinsLD_B	= 0x40050210,
	sinsAND_B	= 0x40050211,
	sinsOR_B	= 0x40050212,
	sinsLD_EB	= 0x40050310,
	sinsAND_EB	= 0x40050311,
	sinsOR_EB	= 0x40050312,
	sinsLD_S	= 0x40050410,
	sinsAND_S	= 0x40050411,
	sinsOR_S	= 0x40050412,
	sinsLD_ES	= 0x40050510,
	sinsAND_ES	= 0x40050511,
	sinsOR_ES	= 0x40050512,

	sinsLDD_E	= 0x40090610,
	sinsANDD_E	= 0x40090611,
	sinsORD_E	= 0x40090612,
	sinsLDD_NE	= 0x40090710,
	sinsANDD_NE = 0x40090711,
	sinsORD_NE	= 0x40090712,
	sinsLDD_B	= 0x40090810,
	sinsANDD_B	= 0x40090811,
	sinsORD_B	= 0x40090812,
	sinsLDD_EB	= 0x40090910,
	sinsANDD_EB = 0x40090911,
	sinsORD_EB	= 0x40090912,
	sinsLDD_S	= 0x40090A10,
	sinsANDD_S	= 0x40090A11,
	sinsORD_S	= 0x40090A12,
	sinsLDD_ES	= 0x40090B10,
	sinsANDD_ES	= 0x40090B11,
	sinsORD_ES	= 0x40090B12,

	sinsMOV		= 0x4C0500,
	sinsCMP		= 0x480706,

	sinsDMOV	= 0x4C0901,
	sinsDCMP	= 0x480d07,

	sinsCJ		= 0x4d0300,
	sinsCJP		= 0x4d030002,

	sinsSRET	= 0x6a0107,
	sinsFEND	= 0x33,
	sinsCALL	= 0x540301,
	sinsP		= 0x3c
};


enum e_scoilMap{
	scoilX	= 0x9c,	//输入继电器 0~160
	scoilY	= 0x9d,	//输出继电器 0~160
	scoilM	= 0x90,	//辅助继电器 0~9000
	scoilS	= 0x98,	//状态		 0~1000
	scoilT	= 0xc2,	//定时器		 0~255
	scoilC	= 0xc5,	//计数器		 0~255
	scoilD	= 0xa8,	//数据寄存器 0~9000
	scoilR	= 'R',		//扩展
	scoilP	= 0xd0,	//指针
	scoilN	= 0xd2,	//嵌套
	scoilK	= 0xe8,	//常量
	scoilDK	= 0xe9,	//常量
	scoilV	= 0xcd,	//
	scoil_V	= 0xf4,	//
};

//DSP指令系统
enum e_dinsMap{
	dinsDEBUG	= 0x2fff,
	//基本指令 低8位
	dinsLD		= 0x2101,
	dinsLDI		= 0x2102,
	dinsLDP		= 0x2103,
	dinsLDF		= 0x2104,

	dinsAND		= 0x2221,
	dinsANI		= 0x2222,
	dinsANDP	= 0x2223,
	dinsANDF	= 0x2224,
	dinsOR		= 0x2231,
	dinsORI		= 0x2232,
	dinsORP		= 0x2233,
	dinsORF		= 0x2234,

	dinsANB		= 0x1311,
	dinsORB		= 0x1312,

	dinsMPS		= 0x1411,
	dinsMRD		= 0x1412,
	dinsMPP		= 0x1413,

	dinsINV		= 0x1511,
	dinsMEP		= 0x2512,
	dinsMEF		= 0x2513,

	dinsOUT		= 0x26f1,
	dinsSET		= 0x26f2,
	dinsRST		= 0x26f3,
	dinsRSTTC	= 0x26f4,
	dinsOUTT	= 0x36f5,
	dinsOUTC	= 0x46d1,
	dinsPLS		= 0x36e7,
	dinsPLF		= 0x36e8,

	dinsMC		= 0x27f1,
	dinsMCR		= 0x27f2,

	dinsNOP		= 0x1811,
	dinsEND		= 0x1812,

	dinsSTL		= 0x2901,
	dinsOUTS	= 0x29f1,
	dinsSETS	= 0x29f2,
	dinsRSTS	= 0x29f3,
	dinsRET		= 0x191A,

	dinsSRET	= 0x1A11,
	dinsFEND	= 0x1A12,
	dinsCALL	= 0x2Af3,
	dinsCJ		= 0x2Af4,
	dinsCJP		= 0x3Af5,
	dinsP		= 0x2A26,

	//操作指令
	dinsADD		= 0x4Bf1,
	dinsSUB		= 0x4Bf2,
	dinsMUL		= 0x4Bf3,
	dinsDIV		= 0x4Bf4,
	dinsINC		= 0x2Bf5,
	dinsDEC		= 0x2Bf6,
	dinsMOV		= 0x3Bf7,
	dinsCMP		= 0x4Bf8,

	dinsDADD	= 0x4Be1,
	dinsDSUB	= 0x4Be2,
	dinsDMUL	= 0x4Be3,
	dinsDDIV	= 0x4Be4,
	dinsDINC	= 0x2Be5,
	dinsDDEC	= 0x2Be6,
	dinsDMOV	= 0x3Be7,
	dinsDCMP	= 0x4Be8,

	dinsLD_E	= 0x3C01,
	dinsAND_E	= 0x3C21,
	dinsOR_E	= 0x3C31,
	dinsLD_NE	= 0x3C02,
	dinsAND_NE	= 0x3C22,
	dinsOR_NE	= 0x3C32,
	dinsLD_B	= 0x3C03,
	dinsAND_B	= 0x3C23,
	dinsOR_B	= 0x3C33,
	dinsLD_EB	= 0x3C04,
	dinsAND_EB	= 0x3C24,
	dinsOR_EB	= 0x3C34,
	dinsLD_S	= 0x3C05,
	dinsAND_S	= 0x3C25,
	dinsOR_S	= 0x3C35,
	dinsLD_ES	= 0x3C06,
	dinsAND_ES	= 0x3C26,
	dinsOR_ES	= 0x3C36,

	dinsLDD_E	= 0x3C07,
	dinsANDD_E	= 0x3C27,
	dinsORD_E	= 0x3C37,
	dinsLDD_NE	= 0x3C08,
	dinsANDD_NE = 0x3C28,
	dinsORD_NE	= 0x3C38,
	dinsLDD_B	= 0x3C09,
	dinsANDD_B	= 0x3C29,
	dinsORD_B	= 0x3C39,
	dinsLDD_EB	= 0x3C0a,
	dinsANDD_EB = 0x3C2a,
	dinsORD_EB	= 0x3C3a,
	dinsLDD_S	= 0x3C0b,
	dinsANDD_S	= 0x3C2b,
	dinsORD_S	= 0x3C3b,
	dinsLDD_ES	= 0x3C0c,
	dinsANDD_ES	= 0x3C2c,
	dinsORD_ES	= 0x3C3c,

	dinsDDRVA	= 0x5D01,
	dinsDDRVI	= 0x5D02,
	dinsDRVA	= 0x5D11,
	dinsDRVI	= 0x5D12,
};

enum e_dcoilMap{
	dcoilX	= 0x0290,	//输入继电器
	dcoilY	= 0x0291,	//输出继电器
	dcoilM	= 0x0292,	//辅助继电器
	dcoilS	= 0x0293,	//状态		
	dcoilT	= 0x0294,	//定时器		
	dcoilC	= 0x0295,	//计数器		
	dcoilD	= 0x0296,	//数据寄存器 
	dcoilP	= 0x0297,
	dcoilV	= 0x0298,
	dcoilN	= 0x0299,

	dcoilK	= 0x02f0,	//常量
	dcoilDK	= 0x03f1,	//常量双字节
	dcoilG	= 0x02ff,	//调试信息

	dcoilDT	= 0x02a4,	//定时器数据
	dcoilDC	= 0x02a5,	//计数器数据
	dcoilPV	= 0x02a8,

	dcoilDDT= 0x02b4,	//定时器数据	
};

enum e_api_coilMap{
	api_coilX	= 0x90,	//输入继电器
	api_coilY	= 0x91,	//输出继电器
	api_coilM	= 0x92,	//辅助继电器
	api_coilS	= 0x93,	//状态		
	api_coilT	= 0x94,	//定时器		
	api_coilC	= 0x95,	//计数器		
	api_coilD	= 0x96,	//数据寄存器 
	api_coilP	= 0x97,
	api_coilV	= 0x98,
	api_coilN	= 0x99,

	api_coilK	= 0xf0,	//常量
	api_coilDK	= 0xf1,	//常量双字节

	api_coilDT	= 0xa4,	//定时器数据
	api_coilDC	= 0xa5,	//计数器数据
	api_coilDD	= 0xa6,	//32位D
	api_coilPV	= 0xa8,

	api_coilDDT= 0xb4,	//定时器数据	
};
#ifdef __cplusplus
}
#endif
#endif	

