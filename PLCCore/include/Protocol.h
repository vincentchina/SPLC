#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "Device.h"

#define _OLD_PROTOCOL	//support for old protocol

#ifdef _WIN32
#define SZ_DEV				2
#define DEV_FOR_PROGRAM		1
#else
#define SZ_DEV				1
#endif
#define DEV_FOR_GUI			0
#define MAX_PROTOCL_BUF_SZ	130

#define STX		0x02
#define ETX		0x03
#define ESC		0x10

#define ENQ		0x05
#define ACK		0x06
#define NAK		0x15



#define CMD_READ		'0'
#define CMD_WRITE		'1'

#define CMD_FORCEON		'7'
#define CMD_FORCEOFF	'8'

#define CMD_PRO_READ	'a'
#define CMD_PRO_WRITE	'b'

#define CMDLEN_COIL		16
#define CMDLEN_PRO		18

typedef uint16 (*pComWrite)( mbyte* data, uint16 sz );
typedef void (*pTimerOut)( void* pData );
typedef void (*pSetTimer)( int nID, unsigned long time, void* pData, pTimerOut callback );

typedef struct tagProtocolConf{
	pComWrite write;
	pSetTimer setTimer;
	int nTimeOutCount;
	unsigned long lTimerout;
}ProtocolConf,*LPProtocolConf;

#define PROTOCOL_CONF_WRITEFUN		0x01
#define PROTOCOL_CONF_SETTIMERFUN	0x02
#define PROTOCOL_CONF_TIMEOUTCOUNT	0x04
#define PROTOCOL_CONF_TIMEOUT		0x08

int API_Protocol( const int id, mbyte* pData, uint16 sz );
void API_InitProtocol( void );
void SetProtocolConf( const int id,LPProtocolConf conf, unsigned int uFlag );
int API_ProtocolSend( const int id, mbyte* pData, uint16 sz );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __MEMCHECKER_H__ */
