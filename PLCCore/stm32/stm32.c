#include "PLCCore.h"
#include "apis.h"
#include "MemManager.h"
#include "insClass.h"

void M_DumpMem( PLPLCSystem psys )
{
}

void M_dDump( mbyte* pdata, uint16 sz )
{
}

/*
ProgramData m_prgData;
	//init plc core
	m_prgData.szFlash = SZ_PROGRAM_MEM / 2;
	m_prgData.szRam = SZ_PROGRAM_MEM;
	PLCSysInitInfo iniInfo;
	iniInfo.pSend0 = ComWrite0;
	iniInfo.pSend1 = ComWrite1;
	iniInfo.pScanEnd = PLCScanEnd;
	iniInfo.pSetOutPut = PLCSetOutPut;
	iniInfo.pPLCStop = PLCStop;
	iniInfo.pFlash = m_prgData.pFlash;
	iniInfo.szFlash = m_prgData.szFlash;
	iniInfo.pRam = m_prgData.pRam;
	iniInfo.szRam = m_prgData.szRam;
	*( (unsigned short*)m_prgData.pFlash ) = dinsEND;
	m_prgData.uScanInterval = 100;
	//g_proData.strprgFile[0] = '\0';
	if( lib_API_InitSystem( &iniInfo ) == RES_ERR )
	{
		MessageBox( _T("???PLC??"),_T("??"),MB_OK );
		PostQuitMessage( -1 );
	}
*/