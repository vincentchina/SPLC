#ifndef __LIB_PLCCORE_H__
#define __LIB_PLCCORE_H__

#ifdef __cplusplus
extern "C"{
#endif

#ifndef PLCCORE_API
#ifdef PLCCORE_EXPORTS
#define PLCCORE_API __declspec(dllexport)
#else
#define PLCCORE_API __declspec(dllimport)
#endif
#endif

typedef unsigned short (*pComWrite)( unsigned char* data, unsigned short sz );

typedef struct tagPLCSysInitInfo{
	void (*pSetOutPut)( unsigned short addr, unsigned char b );
	void (*pScanEnd)();
	void (*pPLCStop)();
	int (*pMachine)( int aix, int speed, int location, int bAbsolute );
	pComWrite pSend[ 2 ];
	unsigned char* pFlash;
	unsigned short szFlash;
	unsigned char* pRam;
	unsigned short szRam;
}PLCSysInitInfo,*LPPLCSysInitInfo;


PLCCORE_API BOOL lib_API_InitSystem( LPPLCSysInitInfo pInitInfo );
PLCCORE_API void lib_API_UninitSystem();

PLCCORE_API void lib_API_FeedWDG();
PLCCORE_API void lib_API_SetWDG( unsigned short ums );

PLCCORE_API int lib_API_LoadProgram();
PLCCORE_API int lib_API_PLCScan();
PLCCORE_API int lib_API_PLCRun();

PLCCORE_API unsigned char lib_API_ReadBit( unsigned char nCoil, unsigned short addr );
PLCCORE_API int lib_API_WriteBit( unsigned char nCoil, unsigned short addr, unsigned char d );

//一次只能一个字节读写
PLCCORE_API int lib_API_ReadData( unsigned char nCoil, unsigned short addr, unsigned int* pData );
PLCCORE_API int lib_API_WriteData( unsigned char nCoil, unsigned short addr, unsigned int d );

PLCCORE_API int lib_API_IsPLCRuning();
PLCCORE_API int lib_API_ProGramRead( unsigned short offset, unsigned short* pData );
PLCCORE_API int lib_API_ProGramWrite( unsigned short offset, unsigned short data );

PLCCORE_API void lib_API_TimerUp();

PLCCORE_API int lib_API_Protocol( const int id, unsigned char* pData, unsigned short sz );

#define DUMP_MEM_X		0x01
#define DUMP_MEM_Y		0x02
#define DUMP_MEM_S		0x04
#define DUMP_MEM_M		0x08
#define DUMP_MEM_D		0x10
#define DUMP_MEM_C		0x20
#define DUMP_MEM_DC		0x40
#define DUMP_MEM_DT		0x80
#define DUMP_MEM_RAM	0x100
#define DUMP_MEM_FLASH	0x200

PLCCORE_API void lib_API_DumpMem( FILE* fp, unsigned int uFlag );
PLCCORE_API void lib_API_LoadMem( FILE* fp, unsigned int uFlag );
PLCCORE_API void lib_API_ResetPLC();

PLCCORE_API void lib_API_StopMachine( int id );
#ifdef __cplusplus
}
#endif
#endif