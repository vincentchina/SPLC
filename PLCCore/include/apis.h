/*
 *	第二个版本，解析运行
 *	提供给用户的接口
 */


#ifndef __API_H__
#define __API_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "Device.h"
#include "Protocol.h"
#include "user.h"

typedef struct tagPLCCallBack{
	void (*pSetOutPut)( uint16 addr, mbyte b );
	void (*pScanEnd)( void );
	void (*pPLCStop)( void );
	int (*pMachine)( int32 aix, int32 speed, int32 location, int32 bAbsolute );
	pComWrite pSend[SZ_DEV];
}PLCCallBack,*LPPLCCallBack;

typedef struct tagSysInitInfo{
	PLCCallBack callBack;
	mbyte* pFlash;
	uint16 szFlash;
	mbyte* pRam;
	uint16 szRam;
}SysInitInfo,*LPSysInitInfo;

api_res API_InitSystem( LPSysInitInfo pInitInfo );
void API_UninitSystem( void );
api_res API_ResetPLC( void );
api_res API_LoadProgram( void );
api_res API_PLCScan( void );
api_res API_PLCRun( void );

void API_FeedWDG( void );
void API_SetWDG( uint16 ums );

int API_ReadBit( mbyte nCoil, uint16 addr, mbyte* pData, int szByte );
int API_WriteBit( mbyte nCoil, uint16 addr, mbyte* pData, int szByte );

int API_ReadData( mbyte nCoil, uint16 addr, uint16* pData, int szU16 );
int API_WriteData( mbyte nCoil, uint16 addr, uint16* pData, int szU16 );

int API_ProGramRead( uint16 offset, uint16* pData, int szU16 );
int API_ProGramWrite( uint16 offset, uint16* pData, int szU16 );


int API_IsPLCRuning( void );
void API_TimerUp( void );

int API_Protocol( const int id, mbyte* pData, uint16 sz );

void API_SetInput( uint16 u16Group, uint16 d );
uint16 API_GetInput( uint16 u16Group );
void API_SetOutput( uint16 u16Group, uint16 d );
uint16 API_GetOutput( uint16 u16Group );

void API_StopMachine( int id );

#ifdef __cplusplus
}
#endif
#endif
