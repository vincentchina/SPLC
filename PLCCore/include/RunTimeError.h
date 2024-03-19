#ifndef __RUNTIME_ERROR_H__
#define __RUNTIME_ERROR_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "PLCCore.h"

enum ePLCErrCode{
	eErrOK,
	eErrLoadPrg,
	eErrScan,
	eErrScanEndCheck,
	eErrWDG,
	eErrArithmedic,
};

api_res SetRunTimeError( enum ePLCErrCode errCode, uint32 uType, uint32 uMsg, void* pData );
api_res IsErrorOK( void );
void CleanError();

#ifdef __cplusplus
}
#endif
#endif
