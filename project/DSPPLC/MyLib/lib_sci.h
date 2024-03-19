//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_Sci.h
//
// TITLE:	DSP28 Device SCI Register Definitions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  0.55| 06 May 2002 | L.H. | EzDSP Alpha Release
//  0.56| 20 May 2002 | L.H. | No change
//  0.57| 27 May 2002 | L.H. | No change
//###########################################################################

#ifndef DSP28_SCI_H
#define DSP28_SCI_H

#include "DSP281x_Device.h"
#include "DSP281x_sci.h"
#include "com_circleBuf.h"

#define SCIDEVSZ		2
#define SCIID_A			0
#define SCIID_B			1

#define SCIBAUD_110		110
#define SCIBAUD_300		300
#define SCIBAUD_600		600
#define SCIBAUD_1200	1200
#define SCIBAUD_2400	2400
#define SCIBAUD_4800	4800
#define SCIBAUD_9600	9600
#define SCIBAUD_14400	14400
#define SCIBAUD_19200	19200
#define SCIBAUD_56000	56000
#define SCIBAUD_57600	57600
#define SCIBAUD_115200	115200
#define SCIBAUD_128000	128000
#define SCIBAUD_256000	256000


#define SCIPARITY_NONE	0
#define SCIPARITY_ODD	1
#define SCIPARITY_EVEN	2

#define SCISTOP_1		0
#define SCISTOP_2		1

#define SCIDATA_1		0
#define SCIDATA_2		1
#define SCIDATA_3		2
#define SCIDATA_4		3
#define SCIDATA_5		4
#define SCIDATA_6		5
#define SCIDATA_7		6
#define SCIDATA_8		7

#define SCIINT_RX		1
#define SCIINT_TX		2

#define SCIFIFO_ENABLE	1

typedef struct tagSCI_BUF{
	Uint16* buf;
	Uint16 sz;
}SCI_BUF,*LPSCI_BUF;

typedef int(*sci_revCallBack)( unsigned int* pbuf, unsigned int sz );

typedef struct  tagSCI_CONFIG {
	Uint16 		LCLK_kHz;
	Uint32 		baud;
	Uint16 		stop:2;
	Uint16 		parity:2;
	Uint16 		data:4;
	Uint16 		fifo:2;
	Uint16 		intterupt:2;
	Uint16 		sciId:4;
	Uint16		szFFRx:8;
	Uint16 		szFFTx:8;
	LPSCI_BUF 	pTxBuf;
	LPSCI_BUF 	pRxBuf;
	sci_revCallBack pCallBack;
}SCI_CONFIG,*LPSCI_CONFIG;

typedef struct tagDEVICE_SCI{
	volatile struct SCI_REGS* pSci;
	sci_revCallBack pCallBack;
	CIRCLEBUF 	TxBuf;
	CIRCLEBUF 	RxBuf;
}DEVICE_SCI,*LPDEVICE_SCI;

int Sci_Init( LPDEVICE_SCI pDev, LPSCI_CONFIG pConf );

__inline int Sci_TxReady( LPDEVICE_SCI pDev ){
	return ( pDev->pSci->SCICTL2.bit.TXRDY == 1 );
}

__inline int Sci_RxReady( LPDEVICE_SCI pDev ){
	return ( pDev->pSci->SCIRXST.bit.RXRDY == 1);
}

__inline int Sci_TxFFReady( LPDEVICE_SCI pDev ){
	return ( pDev->pSci->SCIFFTX.bit.TXFFST == 0 );
}

__inline int Sci_RxFFReady( LPDEVICE_SCI pDev ){
	return ( pDev->pSci->SCIFFRX.bit.RXFIFST > 0 );
}

__inline void Sci_putc( LPDEVICE_SCI pDev, Uint16 c ){
	pDev->pSci->SCITXBUF = ( c & 0x00ff );
}

__inline Uint16 Sci_Getc( LPDEVICE_SCI pDev ){
	return pDev->pSci->SCIRXBUF.all;
}

int Sci_DbufSend( LPDEVICE_SCI pDev, unsigned int* data, unsigned int sz );


#endif  // end of DSP28_SCI_H definition

//===========================================================================
// No more.
//===========================================================================

