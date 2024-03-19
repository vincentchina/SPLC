//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_Sci.c
//
// TITLE:	DSP28 SCI Initialization & Support Functions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  0.55| 06 May 2002 | L.H. | EzDSP Alpha Release
//  0.56| 20 May 2002 | L.H. | No change
//  0.57| 27 May 2002 | L.H. | No change
//###########################################################################

#include "lib_sci.h"

static LPDEVICE_SCI g_pDev[ SCIDEVSZ ]={0};

interrupt void _scia_RxInt(void)
{
	LPDEVICE_SCI pDev = g_pDev[SCIID_A];
	Uint16 c;
   	Uint16 sz = SciaRegs.SCIFFRX.bit.RXFIFST;
	while( sz-- )
   	{
		c = SciaRegs.SCIRXBUF.all & 0x00ff;
		pDev->pCallBack(&c, 1 );
	}
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
	//SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
	//SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;
	
}

interrupt void _scia_TxInt(void)
{
	LPDEVICE_SCI pDev = g_pDev[SCIID_A];
	Uint16 sz = 16;
	while( sz-- && !CBuf_IsEmpty( &pDev->TxBuf ) )
	{	
		SciaRegs.SCITXBUF = CBuf_Dequeue( &pDev->TxBuf );
	}
	if( CBuf_IsEmpty( &pDev->TxBuf ) )
		SciaRegs.SCIFFTX.bit.TXFFIENA = 0;
	SciaRegs.SCIFFTX.bit.TXINTCLR = 1;
	PieCtrlRegs.PIEACK.bit.ACK9 = 1;
	//SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	//SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
}

interrupt void _scib_RxInt(void)
{
	LPDEVICE_SCI pDev = g_pDev[SCIID_B];
}

interrupt void _scib_TxInt(void)
{
	LPDEVICE_SCI pDev = g_pDev[SCIID_B];
}

int Sci_Init( LPDEVICE_SCI pDev, LPSCI_CONFIG pConf )
{
	Uint16 baud;
	EALLOW;
	if( pConf->sciId == SCIID_A )
	{
		pDev->pSci = &SciaRegs;
		GpioMuxRegs.GPFMUX.all = 0x0030;
	}else if( pConf->sciId == SCIID_B )
	{
		pDev->pSci = &ScibRegs;
		GpioMuxRegs.GPFMUX.all = 0x0030;
	}else
	{
		EDIS;
		return 0;
	}
	EDIS;
	//baud
	baud = (Uint16)( (float)pConf->LCLK_kHz / (float)pConf->baud / 8.0 * 1000.0 );
	if( baud == 0 )		//baud err
		return 0;
	g_pDev[ pConf->sciId ] = pDev;
	baud -= 1;
	pDev->pSci->SCIHBAUD = ( ( baud & 0xff00 ) >> 8 );
	pDev->pSci->SCILBAUD = baud & 0x00ff;
	
	//stop bit
	pDev->pSci->SCICCR.bit.STOPBITS = pConf->stop;
	
	//parity
	if( pConf->parity )
	{
		pDev->pSci->SCICCR.bit.PARITYENA = 1;
		pDev->pSci->SCICCR.bit.PARITY = ( pConf->parity == SCIPARITY_EVEN );
	}else
		pDev->pSci->SCICCR.bit.PARITYENA = 0;

	//data bit 
	pDev->pSci->SCICCR.bit.SCICHAR = pConf->data;
	
	// enable TX RX
	pDev->pSci->SCICTL1.all = 0x03;

	pDev->pSci->SCICCR.bit.LOOPBKENA = 0;

	//interrupt
	EALLOW;	
	if( pConf->intterupt & SCIINT_RX )
	{
		pDev->pSci->SCICTL2.bit.RXBKINTENA = 1;
		PieCtrlRegs.PIEIER9.bit.INTx1 = 1;
		if( pConf->sciId == SCIID_A )
			PieVectTable.RXAINT = _scia_RxInt;
	}
	if( pConf->intterupt & SCIINT_TX )
	{
		pDev->pSci->SCICTL2.bit.TXINTENA = 1;
		PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
		if( pConf->sciId == SCIID_A )
			PieVectTable.TXAINT = _scia_TxInt;
	}
	EDIS;
	if( pConf->intterupt )
		IER |= M_INT9;

	//fifo
	if( pConf->fifo )
	{
		pDev->pSci->SCIFFTX.all=0xE040;
		pDev->pSci->SCIFFRX.all=0x6060;
		pDev->pSci->SCIFFTX.bit.TXFFILIL = pConf->szFFTx;
		if( pConf->szFFRx == 0 )
			pConf->szFFRx = 15;
		pDev->pSci->SCIFFRX.bit.RXFFIL = pConf->szFFRx;
		pDev->pSci->SCIFFCT.all=0x0;
	}
	
	//circleBuf
	if( pConf->pTxBuf )
		CBuf_init( &pDev->TxBuf, (void*)pConf->pTxBuf->buf, sizeof(Uint16), pConf->pTxBuf->sz );
	else
		CBuf_unInit( &pDev->TxBuf );
	if( pConf->pRxBuf )
		CBuf_init( &pDev->RxBuf, (void*)pConf->pRxBuf->buf, sizeof(Uint16), pConf->pRxBuf->sz );
	else
		CBuf_unInit( &pDev->RxBuf );

	//callback
	pDev->pCallBack = pConf->pCallBack;

	//enable configure
	pDev->pSci->SCICTL1.all |= 0x20;

	return 1;
}

int Sci_DbufSend( LPDEVICE_SCI pDev, unsigned int* data, unsigned int sz )
{
	unsigned int* pEnd = data + sz;
	//return 0;
	while( data < pEnd && !CBuf_IsFull( &pDev->RxBuf ) )
	{
		CBuf_Enqueue( &pDev->TxBuf, *data++ );
	}
	SciaRegs.SCIFFTX.bit.TXFFIENA = 1;
	return ( sz - ( pEnd  - data ) );
}
//===========================================================================
// No more.
//===========================================================================
