#ifndef __INS_CLASS_H__
#define __INS_CLASS_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "Device.h"
#define MAX_INS_LEN	( 0x10 + 1 )


//DSP指令系统
//iargc:iclass:itype:ins;
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

/* itype = 0 为输入LD
 * >= a 为输出
 * =1 单独一行的指令，不带参数
 */
#ifdef _STM32
#pragma anon_unions
#endif
typedef union tagPLCIns{
	uint16 all;
	struct{
		uint16 ins:4;
		uint16 itype:4;
		uint16 iclass:4;
		uint16 iargc:4;
	};
}PLCIns,*LPPLCIns;

typedef union tagPLCCoil{
	uint16 all;
	struct{
		uint16 coil:4;
		uint16 ctype:4;
		uint16 cargc:4;
		uint16 reserve:4;
	};
}PLCCoil,*LPPLCCoil;

typedef union tagCoilData{
	uint16 udata16;
	int16  sdata16;
	uint32 udata32;
	int32  sdata32;
	struct{
		uint16  u16L;
		uint16  u16H;
	};
}CoilData,*LPCoilData;

typedef union tagCoilData64{
	uint32 udata32;
	int32  sdata32;
	uint64 udata64;
	int64  sdata64;
	struct{
		CoilData d32L;
		CoilData d32H;
	};
}CoilData64,*LPCoilData64;

typedef struct tagRegGet{
	mbyte* pSource;
	mbyte iReg;
	union{
		uint16 addr;
		uint16 udata16;
		int16  sdata16;
		uint32 udata32;
		int32  sdata32;
	};
}RegGet,*LPRegGet;

enum eInsFunRes{
	eIFunContinue,
	eIFunErr,
	eIFunEnd,
};

typedef enum eInsFunRes (*funInsClass)( LPPLCIns pins, int argc, LPRegGet argv);
typedef struct tagInsClassFuns{
	funInsClass* funs;
	int sz;
}InsClassFuns,*LPInsClassFuns;

api_res InitInsClassFuns( void );
api_res GetInsLine( mbyte** ppStep, LPPLCIns *pins, mbyte argc, LPRegGet argv );
api_res SkipInsLine( mbyte** ppStep );

#ifdef __cplusplus
}
#endif
#endif	

