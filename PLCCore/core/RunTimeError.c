#include "RunTimeError.h"

extern PLCSystem g_sys;

api_res SetRunTimeError( enum ePLCErrCode errCode, uint32 uType, uint32 uMsg, void* pData )
{
	g_sys.R[REG_D].pData[SYSADDRD_ERROR] = errCode;
	*( (uint32*)( g_sys.R[REG_D].pData + SYSADDRD_ERRORARG ) ) = uType;
	return RES_ERR;
}

api_res IsErrorOK()
{
	return ( g_sys.R[REG_D].pData[SYSADDRD_ERROR] == eErrOK );
}

void CleanError()
{
	g_sys.R[REG_D].pData[SYSADDRD_ERROR] = eErrOK;
	*( (uint32*)( g_sys.R[REG_D].pData + SYSADDRD_ERRORARG ) ) = eErrOK;
}
