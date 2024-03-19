/*
 * 第二版本，支持多个通信口
 */
//#include "PLCCore.h"
#include "apis.h"
#include "insClass.h"
#include "MemManager.h"
//#include "Protocol.h"

typedef struct tagProtocolData{
	ProtocolConf conf;
	int nTimeout;
	mbyte revBuf[ MAX_PROTOCL_BUF_SZ ];
	uint16 revIndex;
	mbyte bESCGet;
	mbyte bSTXGet;
}ProtocolData,*LPProtocolData;

static ProtocolData g_protocol[SZ_DEV] = {0};

int Asiic2Hex( mbyte* pstr,uint16 sz,uint32* phex )
{
	mbyte* pc;
	*phex = 0;
	pc = pstr + sz - 1;
	while( pc >= pstr )
	{
		*phex <<= 4;
		if( *pc >= 'A' && *pc <= 'F' )
			*phex |= *pc - 55;
		else if( *pc >= '0' && *pc <= '9' )
			*phex |= *pc - 48;
		else
			return 0;
		pc--;
	}
	return 1;
}

__inline void SendNAK( LPProtocolData pDev )
{
	mbyte nak = NAK;
	if( pDev->conf.write )
		pDev->conf.write( &nak, 1 );
}

__inline void SendACK( LPProtocolData pDev )
{
	mbyte enq = ACK;
	if( pDev->conf.write )
		pDev->conf.write( &enq, 1 );
}

int ProtocolSend( LPProtocolData pDev, mbyte* pData, uint16 sz )
{
	mbyte* p;
	mbyte* pEnd;
	mbyte sendBuf[ MAX_PROTOCL_BUF_SZ * 2 ];
	mbyte* ps = sendBuf;
	mbyte check;
	check = 0;
	*ps++ = STX;
	pEnd = pData + sz;
	p = pData;
	while( p < pEnd )
	{
		/**/switch( *p )
		{
		case STX:
		case ENQ:
		case ETX:
		case ESC:
			*ps++ = ESC;
		break;
		}
		/*if( *p == STX	
			||  *p == ENQ
			|| 	*p == ETX
			|| 	*p == ESC )
		{
			*ps++ = ESC;
		}*/
		check += *p;
		*ps++ = *p++;
	}
	switch( check )
	{
	case STX:
	case ENQ:
	case ETX:
	case ESC:
		*ps++ = ESC;
		break;
	}
	*ps++ = check;
	*ps++ = ETX;
	if( pDev->conf.write )
		pDev->conf.write( sendBuf, ps -  sendBuf );
	return 1;
}

int API_ProtocolSend( const int id, mbyte* pData, uint16 sz )
{
	if( id < 0 || id >= SZ_DEV )
		return 0;
	return ProtocolSend( g_protocol + id, pData, sz );
}

void ProcessData( LPProtocolData pDev )
{
	//检测校验和
	mbyte cmd;
	mbyte coil;
	uint16 addr;
	uint16 sz;
	uint16 dataGet;
	mbyte* pData;
	mbyte* pc;
	int i;
	mbyte senBuf[ MAX_PROTOCL_BUF_SZ ];
	mbyte tmp = 0;

	for ( i =0; i < pDev->revIndex - 1; i++ )
		tmp += pDev->revBuf[i];
	if( ( tmp & 0x00ff ) != pDev->revBuf[ pDev->revIndex - 1 ] )
		SendNAK( pDev );
	else
	{
		pc = pDev->revBuf;
		cmd = *pc++;
		if( cmd != CMD_PRO_READ && cmd != CMD_PRO_WRITE )
			coil = (*pc++) & 0x00ff;
		addr = ( ( *pc++ & 0x00ff ) << 8 );
		addr |= *pc++ & 0x00ff;
		sz = (*pc++) & 0x00ff;
		pData = pc;
		
		switch( cmd )
		{
		case CMD_READ:
			if( sz > ( MAX_PROTOCL_BUF_SZ - 4 ) / 2 )
				SendNAK( pDev );
			pData = senBuf;
#ifdef _OLD_PROTOCOL
			*pData++ = CMD_WRITE;
			*pData++ = coil;
			*pData++ = ( ( addr & 0xff00 ) >> 8 );
			*pData++ = ( addr & 0x00ff);
			*pData++ = (mbyte)sz;
#endif
			if( coil == api_coilD 
				|| coil == api_coilDT 
				|| coil == api_coilDC
				)
			{
				while( sz )
				{
					if( API_ReadData( coil, addr++, (uint16*)&dataGet, 1 ) != 1 )
						break;
					else
					{
						*pData++ = (dataGet & 0xff00 ) >> 8;
						*pData++ = dataGet & 0x00ff;
					}
					--sz;
				}
				if( sz != 0 )
				{
					SendNAK( pDev );
					break;
				}
			}else if( coil == api_coilX
				|| coil == api_coilY
				|| coil == api_coilM
				|| coil == api_coilS
				|| coil == api_coilT
				|| coil == api_coilC
				)
			{
				if( API_ReadBit( coil, addr, pData, sz ) != sz  )
				{
					SendNAK( pDev );
					break;
				}
				pData += sz;
			}
			if( !ProtocolSend( pDev, senBuf, pData - senBuf ) )
				SendNAK( pDev );
			break;
		case CMD_WRITE:
			if( sz > ( MAX_PROTOCL_BUF_SZ - 4 ) / 2 )
				SendNAK( pDev );
			if( coil == api_coilD 
				|| coil == api_coilDT 
				|| coil == api_coilDC
				)
			{
				while( sz )
				{
					dataGet = ( *pData++ & 0x00ff ) << 8;
					dataGet |= ( *pData++ & 0x00ff );
					if( API_WriteData( coil, addr++, (uint16*)&dataGet, 1 ) != 1 )
						break;
					--sz;
				}
				if( sz == 0 )
				{
					SendACK( pDev );
				}else
					SendNAK( pDev );
			}else if( coil == api_coilX
				|| coil == api_coilY
				|| coil == api_coilM
				|| coil == api_coilS
				|| coil == api_coilT
				|| coil == api_coilC
				)
			{
				if( API_WriteBit( coil, addr, pData, sz ) == sz )
					SendACK( pDev );
				else
					SendNAK( pDev );
			}
			break;
		case CMD_FORCEON:
			tmp = 1;
			if( API_WriteBit( coil, addr, &tmp, 1 ) )
				SendACK( pDev );
			else
				SendNAK( pDev );
			break;
		case CMD_FORCEOFF:
			tmp = 0;
			if( API_WriteBit( coil, addr, &tmp, 1 ) )
				SendACK( pDev );
			else
				SendNAK( pDev );
			break;
		case CMD_PRO_READ:
			if( API_IsPLCRuning() )
			{
				SendNAK( pDev );
				break;
			}
			pData = senBuf;
			while( sz > 0 )
			{
				if( API_ProGramRead( addr, (uint16*)&dataGet, 1 ) != 1 )
					break;
				else
				{
					*pData++ = (dataGet & 0xff00 ) >> 8;
					*pData++ = dataGet & 0x00ff;
				}
				--sz;
			}
			if( sz == 0 )
			{
				if( !ProtocolSend( pDev, senBuf, pData - senBuf ) )
					SendNAK( pDev );
			}else
				SendNAK( pDev );
			break;
		case CMD_PRO_WRITE:
			if( API_IsPLCRuning() )
			{
				SendNAK( pDev );
				break;
			}
			_DPRINTF( "PRO_WRITE:addr=%d\n", addr );
			while( sz )
			{
				dataGet = ( *pData++ & 0x00ff ) << 8;
				dataGet |= ( *pData++ & 0x00ff );
				if( API_ProGramWrite( addr, (uint16*)&dataGet,1 ) != 1 )
					break;
				--sz;
			}
			if( sz == 0 )
				SendACK( pDev );
			else
				SendNAK( pDev );
			break;
		default:
			SendNAK( pDev );
			break;
		}
	}
}

int Protocol( LPProtocolData pDev, mbyte* pData, uint16 sz )
{
	while( sz-- )
	{
		if( pDev->revIndex >= MAX_PROTOCL_BUF_SZ )
		{
			SendNAK( pDev );
			pDev->revIndex = 0;
			pDev->bSTXGet = 0;
		}
		switch( *pData )
		{
		case ENQ:
			if( pDev->bESCGet )
			{
				pDev->revBuf[ pDev->revIndex++] = *pData;
				pDev->bESCGet = 0;
			}else
			{
				pDev->bSTXGet = 0;
				pDev->revIndex = 0;
				SendACK( pDev );
			}
			break;
		case STX:
			if( pDev->bESCGet )
			{
				pDev->revBuf[ pDev->revIndex++] = *pData;
				pDev->bESCGet = 0;
			}else
			{
				pDev->bSTXGet = 1;
				pDev->revIndex = 0;
			}
			break;
		case ESC:
			if( pDev->bESCGet )
			{
				pDev->revBuf[ pDev->revIndex++] = *pData;
				pDev->bESCGet = 0;
			}else
				pDev->bESCGet = 1;
			break;
		case ETX:
			if( pDev->bESCGet )
			{
				pDev->revBuf[ pDev->revIndex++] = *pData;
				pDev->bESCGet = 0;
			}else
			{
				pDev->bSTXGet = 0;
				ProcessData( pDev );
			}
			break;
		default:
			if( pDev->bSTXGet )
				pDev->revBuf[ pDev->revIndex++] = *pData;
			break;
		}
		++pData;
	}
	return 1;//返回1表示已经由该函数识别并处理
}

int API_Protocol( const int id, mbyte* pData, uint16 sz )
{
	if( id < 0 || id >= SZ_DEV )
		return 0;
	return Protocol( g_protocol + id, pData, sz );
}

void API_InitProtocol( void )
{
	tiny_mest( &g_protocol, 0, sizeof( g_protocol ) );
}

void SetProtocolConf( const int id, LPProtocolConf conf, unsigned int uFlag )
{
	LPProtocolData pDev;
	if( id < 0 || id >= SZ_DEV )
		return;
	pDev = g_protocol + id;
	if( PROTOCOL_CONF_WRITEFUN & uFlag )
		pDev->conf.write = conf->write;
	if( PROTOCOL_CONF_SETTIMERFUN & uFlag )
		pDev->conf.setTimer = conf->setTimer;
	if( PROTOCOL_CONF_TIMEOUTCOUNT & uFlag )
		pDev->conf.nTimeOutCount = conf->nTimeOutCount;
	if( PROTOCOL_CONF_TIMEOUT & uFlag )
		pDev->conf.lTimerout = conf->lTimerout;
}
