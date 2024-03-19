#ifndef __TC6014_H
#define __TC6014_H

#include "sys.h"
#include "sram.h"
#include "usart.h"

#define TC6014A_BASE_ADDR Bank1_SRAM1_ADDR
#define TC6014B_BASE_ADDR Bank1_SRAM2_ADDR

#define	TC6014A 0
#define	TC6014B 1

#define outpw(port, data) (*(volatile unsigned short *)(port)) = data
#define inpw(port) (*(volatile unsigned short *)(port))

//-----tc6014 ¼Ä´æÆ÷µØÖ·¶¨Òå
#define  wr0  0x0  //ÃüÁî¼Ä´æÆ÷
#define  wr1  0x2  //Ä£Ê½¼Ä´æÆ÷1
#define  wr2  0x4  //Ä£Ê½¼Ä´æÆ÷2
#define  wr3  0x6  //Ä£Ê½¼Ä´æÆ÷3 
#define  wr4  0x8  //Êä³ö¼Ä´æÆ÷
#define  wr5  0xa  //²å²¹Ä£Ê½¼Ä´æÆ÷
#define  wr6  0xc  //µÍÎ»Ğ´ÈëÊı¾İ¼Ä´æÆ÷ 
#define  wr7  0xe  //¸ßÎ»Ğ´ÈëÊı¾İ¼Ä´æÆ÷

#define  rr0  0x0 //Ö÷×´Ì¬¼Ä´æÆ÷
#define  rr1  0x2 //×´Ì¬¼Ä´æÆ÷1  
#define  rr2  0x4 //×´Ì¬¼Ä´æÆ÷2 
#define  rr3  0x6 //×´Ì¬¼Ä´æÆ÷3  
#define  rr4  0x8 //ÊäÈë¼Ä´æÆ÷1  
#define  rr5  0xa //ÊäÈë¼Ä´æÆ÷2  
#define  rr6  0xc //µÍÎ»¶Á³öÊı¾İ¼Ä´æÆ÷ 
#define  rr7  0xe //¸ßÎ»¶Á³öÊı¾İ¼Ä´æÆ÷ 

#define  bp1p  0x4  //BPµÚ1 Öá+·½ÏòÊı¾İ¼Ä´æÆ÷ 
#define  bp1m  0x6 //BPµÚ1 Öá-·½ÏòÊı¾İ¼Ä´æÆ÷  
#define  bp2p  0x8  //BPµÚ2 Öá+·½ÏòÊı¾İ¼Ä´æÆ÷
#define  bp2m  0xa //BPµÚ2 Öá-·½ÏòÊı¾İ¼Ä´æÆ÷  
#define  bp3p  0xc  //BPµÚ3 Öá+·½ÏòÊı¾İ¼Ä´æÆ÷
#define  bp3m  0xe //BPµÚ3 Öá-·½ÏòÊı¾İ¼Ä´æÆ÷

//typedef enum
//{
//	TC6014A = 0,
//	TC6014B
//}ChipEnum;


/*-----------------------wr0------------------------------------------------*/
struct WR0_BITS
{
	uint16_t CMD:7;	//bit0~6 		ÃüÁîÂë
	uint16_t rsvd0:1;	//bit7 			±£Áô  	ĞèÉèÖÃÎª0
	uint16_t X_AXIS_SEL:1;	//bit8		ÖáXµÄÑ¡ÔñÎ»
	uint16_t Y_AXIS_SEL:1;	//bit9		ÖáYµÄÑ¡ÔñÎ»
	uint16_t Z_AXIS_SEL:1;	//bit10		ÖáZµÄÑ¡ÔñÎ»
	uint16_t U_AXIS_SEL:1;	//bit11		ÖáUµÄÑ¡ÔñÎ»
	uint16_t rsvd1:3;	//bit12~14 	±£Áô  	ĞèÉèÖÃÎª0
	uint16_t RESET:1;	//bit15 		¸´Î»Î»  Ğ´0x80ÊµÏÖĞ¾Æ¬¸´Î»£¬×¢ÒâÆäËûÇé¿öÏÂÒªÉèÎª0
};

union WR0
{
	struct WR0_BITS bit;
	uint16_t all;
};

/*-----------------------wr1------------------------------------------------*/
struct WR1_BITS
{
	uint16_t IN0_L:1;	//bit0 		IN0Òı½ÅÓĞĞ§µçÆ½Î» 0£ºµÍµçÆ½ÓĞĞ§    1£º¸ßµçÆ½ÓĞĞ§
	uint16_t IN0_E:1;	//bit1 		IN0Òı½ÅÊ¹ÄÜÎ»     0£º²»Ê¹ÄÜ        1£ºÊ¹ÄÜ  
	uint16_t IN1_L:1;	//bit2 		IN1Òı½ÅÓĞĞ§µçÆ½Î» 0£ºµÍµçÆ½ÓĞĞ§    1£º¸ßµçÆ½ÓĞĞ§
	uint16_t IN1_E:1;	//bit3 		IN1Òı½ÅÊ¹ÄÜÎ»     0£º²»Ê¹ÄÜ        1£ºÊ¹ÄÜ 
	uint16_t IN2_L:1;	//bit4 		IN2Òı½ÅÓĞĞ§µçÆ½Î» 0£ºµÍµçÆ½ÓĞĞ§    1£º¸ßµçÆ½ÓĞĞ§
	uint16_t IN2_E:1;	//bit5 		IN2Òı½ÅÊ¹ÄÜÎ»     0£º²»Ê¹ÄÜ        1£ºÊ¹ÄÜ 
	uint16_t IN3_L:1;	//bit6 		IN3Òı½ÅÓĞĞ§µçÆ½Î» 0£ºµÍµçÆ½ÓĞĞ§    1£º¸ßµçÆ½ÓĞĞ§
	uint16_t IN3_E:1;	//bit7 		IN3Òı½ÅÊ¹ÄÜÎ»     0£º²»Ê¹ÄÜ        1£ºÊ¹ÄÜ 
	
	//ÒÔÏÂÊÇÖĞ¶ÏÔÊĞí/½ûÖ¹Î»  1£ºÔÊĞí   0£º½ûÖ¹Ï
	uint16_t PLUSE:1;	//bit8 		Ã¿Ò»¸öÇı¶¯Âö³åÉÏÉıÑØ£¬ÖĞ¶Ï½«·¢Éú£¨Çı¶¯Âö³åÓÃÕıÂß¼­£©
	uint16_t P_MTE_CN:1;	//bit9  Âß¼­/ÊµÎ»¼ÆÊıÆ÷µÄÊıÖµ >= COMP-¼Ä´æÆ÷µÄÊıÖµÊ±£¬ÖĞ¶Ï·¢Éú
	uint16_t P_LT_CN:1;	//bit10 Âß¼­/ÊµÎ»¼ÆÊıÆ÷µÄÊıÖµ < COMP-¼Ä´æÆ÷µÄÊıÖµÊ±£¬ÖĞ¶Ï·¢Éú
	uint16_t P_LT_CP:1;	//bit11 Âß¼­/ÊµÎ»¼ÆÊıÆ÷µÄÊıÖµ >= COMP+¼Ä´æÆ÷µÄÊıÖµÊ±£¬ÖĞ¶Ï·¢Éú
	uint16_t P_MTE_CP:1;	//bit12 Âß¼­/ÊµÎ»¼ÆÊıÆ÷µÄÊıÖµ < COMP+¼Ä´æÆ÷µÄÊıÖµÊ±£¬ÖĞ¶Ï·¢Éú
	uint16_t C_END:1;	//bit13   ¼Ó¼õËÙÇı¶¯Ê±£¬ÔÚ¶¨ËÙÓò½áÊøÂö³åÊä³öºó£¬ÖĞ¶Ï·¢Éú
	uint16_t C_STA:1;	//bit14   ¼Ó¼õËÙÇı¶¯Ê±£¬ÔÚ¶¨ËÙÓò¿ªÊ¼Âö³åÊä³öºó£¬ÖĞ¶Ï·¢Éú
	uint16_t E_END:1;	//bit15   Çı¶¯½áÊøÊ±£¬ÖĞ¶Ï·¢Éú
};

union WR1
{
	struct WR1_BITS bit;
	uint16_t all;
};

/*-----------------------wr2------------------------------------------------*/
struct WR2_BITS
{
	uint16_t SLMTP:1;	//bit0 	  ÊÇ·ñ°ÑCOMP+¼Ä´æÆ÷×÷ÎªÈí¼şÏŞÖÆ 0£º·ñ   1£ºÊÇ
	uint16_t SLMTN:1;	//bit1 		ÊÇ·ñ°ÑCOMP-¼Ä´æÆ÷×÷ÎªÈí¼şÏŞÖÆ 0£º·ñ   1£ºÊÇ
	uint16_t LMTMD:1;	//bit2 		Ó²¼şÏŞÖÆÓĞĞ§Ê±µÄÂö³åÍ£Ö¹·½Ê½  0£ºÁ¢¼´Í£Ö¹  1£º¼õËÙÍ£Ö¹
	uint16_t HLMTP:1;	//bit3 		Õı·½ÏòÏŞÖÆÊäÈëĞÅºÅ(nLMTP)µÄÂß¼­µçÆ½  0£ºµÍÓĞĞ§   1£º¸ßÓĞĞ§
	uint16_t HLMTN:1;	//bit4 		¸º·½ÏòÏŞÖÆÊäÈëĞÅºÅ(nLMTN)µÄÂß¼­µçÆ½  0£ºµÍÓĞĞ§   1£º¸ßÓĞĞ§
	uint16_t CMPSL:1;	//bit5 		°ÑCOMP+/-¼Ä´æÆ÷µÄ±È½Ï¶ÔÏóÉè¶¨ÎªÂß¼­Î»ÖÃ¼ÆÊıÆ÷»òÕßÊµ¼ÊÎ»ÖÃ¼ÆÊıÆ÷
	uint16_t PLSMD:1;	//bit6 		Çı¶¯Âö³åµÄÊä³ö·½Ê½  0£º¶ÀÁ¢2Âö³å·½Ê½   1£ºÂö³å+·½Ïò·½Ê½
	uint16_t PLS_L:1;	//bit7 		Çı¶¯Âö³åµÄÂß¼­µçÆ½  0£ºÕıÂß¼­    1£º¸ºÂß¼­
	uint16_t DIR_L:1; //bit8 		·½ÏòÒı½ÅÂß¼­µçÆ½  0£ºÕı·½Ïò(L) ¸º·½Ïò(H)  1£ºÕı·½Ïò(H) ¸º·½Ïò(L) 
	uint16_t PINMD:1;	//bit9    ±àÂëÆ÷ÊäÈëĞÅºÅÄ£Ê½ 0£º2ÏàÂö³åÊäÈë   1£ºÉÏ/ÏÂÂö³åÊäÈë
	uint16_t PIND:2;	//bit10~11 	2ÏàÂö³åÊäÈë·ÖÖÜ±È  00£º1/1  01£º1/2  10£º1/4  11£ºÎŞĞ§
	uint16_t ALM_L:1;	//bit12   ÊäÈëĞÅºÅ(nALARM)µÄÂß¼­µçÆ½ 0£ºµÍÓĞĞ§   1£º¸ßÓĞĞ§
	uint16_t ALM_E:1;	//bit13   ÊÇ·ñÊ¹ÄÜ½ÓÊÕËÅ·şÇı¶¯Æ÷µÄnALARMĞÅºÅ  0£º·ñ   1£ºÊÇ
	uint16_t INP_L:1;	//bit14   ÊäÈëĞÅºÅ(nINPOS)µÄÂß¼­µçÆ½ 0£ºµÍÓĞĞ§   1£º¸ßÓĞĞ§
	uint16_t INP_E:1;	//bit15   ÊÇ·ñÊ¹ÄÜ½ÓÊÕËÅ·şÇı¶¯Æ÷µÄnINPOSĞÅºÅ  0£º·ñ   1£ºÊÇ
};

union WR2
{
	struct WR2_BITS bit;
	uint16_t all;
};

/*-----------------------wr3------------------------------------------------*/
struct WR3_BITS
{
	uint16_t MANLD:1;	//bit0 	  °Ñ¼Ó/¼õËÙ¶¨Á¿Çı¶¯µÄ¼õËÙÉè¶¨Îª×Ô¶¯¼õËÙ»òÊÖ¶¯¼õËÙ 0£º×Ô¶¯¼õËÙ   1£ºÊÖ¶¯¼õËÙ
	uint16_t DSNDE:1;	//bit1 		°Ñ¼Ó/¼õËÙÇı¶¯Ê±µÄ¼õËÙËÙ¶ÈÉè¶¨Îª¼ÓËÙ¶ÈµÄÊıÖµ»òÕß¶ÀÁ¢µÄ¼õËÙ¶ÈµÄÊıÖµ 0£ºÊ¹ÓÃ¼ÓËÙ¶ÈµÄÊıÖµ   1£ºÊ¹ÓÃ¼õËÙ¶ÈµÄÊıÖµ
	uint16_t SACC:1;	//bit2 		Éè¶¨Ö±Ïß¼Ó/¼õËÙ»òÕßSÇúÏß¼Ó/¼õËÙ 0£ºÖ±Ïß¼Ó/¼õËÙ  1£ºSÇúÏß¼Ó/¼õËÙ£¨Ê¹ÓÃÇ°Ò»¶¨ÒªÉèºÃ¼Ó/¼õËÙ¶ÈµÄ±ä»¯ÂÊ£©  
	uint16_t EXOP0:2;	//bit3~4 		¸ù¾İÍâ²¿ÊäÈëĞÅºÅ(nEXPP,nEXPM)Éè¶¨Çı¶¯²Ù×÷ 00/11£ºÎŞĞ§   01£ºÁ¬ĞøÂö³åÇı¶¯  10£º¶¨Á¿Âö³åÇı¶¯
	uint16_t rsvd0:2;	//bit5~6 	±£Áô  	ĞèÉèÖÃÎª0	
	uint16_t OUTSL:1;	//bit7 		Ñ¡ÔñÊä³öĞÅºÅnOUT7~4ÎªÍ¨ÓÃÊä³ö»¹ÊÇÊä³öÇı¶¯×´Ì¬  0£ºÍ¨ÓÃ  1£º×´Ì¬
													/*
													nOUT4/CMPP   µ±Âß¼­/Êµ¼Ê¼ÆÊıÆ÷µÄÖµ >= COMP+¼Ä´æÆ÷Ê±£¬Êä³ö¸ßµçÆ½£¬·ñÔòÊä³öµÍµçÆ
													nOUT5/CMPM   µ±Âß¼­/Êµ¼Ê¼ÆÊıÆ÷µÄÖµ >= COMP-¼Ä´æÆ÷Ê±£¬Êä³ö¸ßµçÆ½£¬·ñÔòÊä³öµÍµçÆ½
													nOUT6/ASND   Çı¶¯ÃüÁîÕıÔÚÖ´ĞĞ£¬²¢´¦ÓÚ¼ÓËÙ×´Ì¬Ê±£¬Êä³ö¸ßµçÆ½£¬·ñÔòÊä³öµÍµçÆ½
													nOUT7/DSND   Çı¶¯ÃüÁîÕıÔÚÖ´ĞĞ£¬²¢´¦ÓÚ¼õËÙ×´Ì¬Ê±£¬Êä³ö¸ßµçÆ½£¬·ñÔòÊä³öµÍµçÆ½½
													*/
	uint16_t OUT4:1;	//bit8 		°ÑÊä³öĞÅºÅnOUT4×÷ÎªÍ¨ÓÃÊä³öÊ¹ÓÃÊ±µÄÊıÖµ  0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t OUT5:1;	//bit9 		°ÑÊä³öĞÅºÅnOUT5×÷ÎªÍ¨ÓÃÊä³öÊ¹ÓÃÊ±µÄÊıÖµ  0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t OUT6:1;	//bit10 		°ÑÊä³öĞÅºÅnOUT6×÷ÎªÍ¨ÓÃÊä³öÊ¹ÓÃÊ±µÄÊıÖµ  0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t OUT7:1;	//bit11 		°ÑÊä³öĞÅºÅnOUT7×÷ÎªÍ¨ÓÃÊä³öÊ¹ÓÃÊ±µÄÊıÖµ  0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t rsvd1:4;	//bit12~15 	±£Áô  	ĞèÉèÖÃÎª0	
};

union WR3
{
	struct WR3_BITS bit;
	uint16_t all;
};

/*-----------------------wr4------------------------------------------------*/
struct WR4_BITS
{
	uint16_t XOUT0:1;	//bit0 	  XOUT0Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t XOUT1:1;	//bit1 	  XOUT1Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t XOUT2:1;	//bit2 	  XOUT2Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t XOUT3:1;	//bit3 	  XOUT3Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	
	uint16_t YOUT0:1;	//bit4 	  YOUT0Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t YOUT1:1;	//bit5 	  YOUT1Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t YOUT2:1;	//bit6 	  YOUT2Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t YOUT3:1;	//bit7 	  YOUT3Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	
	uint16_t ZOUT0:1;	//bit8 	  ZOUT0Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t ZOUT1:1;	//bit9 	  ZOUT1Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t ZOUT2:1;	//bit10 	  ZOUT2Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t ZOUT3:1;	//bit11 	  ZOUT3Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö

	uint16_t UOUT0:1;	//bit12 	  UOUT0Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³
	uint16_t UOUT1:1;	//bit13 	  UOUT1Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t UOUT2:1;	//bit14 	  UOUT2Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³ö
	uint16_t UOUT3:1;	//bit15 	  UOUT3Í¨ÓÃÊä³ö 0£ºµÍµçÆ½Êä³ö   1£º¸ßµçÆ½Êä³öö
};

union WR4
{
	struct WR4_BITS bit;
	uint16_t all;
};

/*-----------------------wr5------------------------------------------------*/
struct WR5_BITS
{
	uint16_t AX1:2;	//bit0~1 	  Ö¸¶¨ÔËĞĞ²å²¹Çı¶¯µÄµÚÒ»Öá£¨Ö÷Öá£©
	uint16_t AX2:2;	//bit2~3 	  Ö¸¶¨ÔËĞĞ²å²¹Çı¶¯µÄµÚ¶şÖá
	uint16_t AX3:2;	//bit4~5 	  Ö¸¶¨ÔËĞĞ²å²¹Çı¶¯µÄµÚÈıÖá
							/*
							Öá			±àÂë
							X   		00
							Y				01
							Z				10
							U				11
							*/
	uint16_t rsvd0:2;	//bit6~7 	  ±£Áô  	ĞèÉèÖÃÎª0	
	
	uint16_t LSPD:2;	//bit8~9 	  Éè¶¨²å²¹Çı¶¯µÄ¹Ì¶¨ÏßËÙ¶ÈÄ£Ê½
							/*
					bit9:bit8		²Ù×÷Ä£Ê½
							00		ÏßËÙ¶Èºã¶¨ÎŞĞ§
							01		2ÖáÏßËÙ¶Èºã¶¨
							10		£¨²»ÄÜÉè¶¨£©
							11		3ÖáÏßËÙ¶Èºã¶¨
							*/

	uint16_t rsvd1:1;	//bit10 	  ±£Áô  	ĞèÉèÖÃÎª0	
	
	uint16_t EXPLS:1;	//bit11 	  ÊÇ·ñ¸ù¾İÍâ²¿ĞÅºÅ(EXPLSN)½øĞĞ²½½ø²å²¹Çı¶¯ 0£º·ñ   1£ºÊÇö
	uint16_t CMPLS:1;	//bit12 	  ÊÇ·ñ¸ù¾İÃüÁî½øĞĞ²½½ø²å²¹Çı¶¯ 0£º·ñ   1£ºÊÇö
	uint16_t rsvd2:1;	//bit13 	  ±£Áô  	ĞèÉèÖÃÎª0
	uint16_t CIINT:1;	//bit14 	  ÊÇ·ñÊ¹ÄÜÁ¬Ğø²å²¹ÖĞ¶Ï 0£º·ñ   1£ºÊÇö
	uint16_t BPINT:1;	//bit15 	  ÊÇ·ñÊ¹ÄÜÎ»Ä£Ê½²å²¹ÖĞ¶Ï 0£º·ñ   1£ºÊÇö
};

union WR5
{
	struct WR5_BITS bit;
	uint16_t all;
};

/*-----------------------tc6014¼Ä´æÆ÷------------------------------------------------*/
typedef struct TC6014_Wregs
{
	//TC6014µÄ¼Ä´æÆ÷
	union WR0 _wr0;
	union WR1 _wr1[4];
	union WR2 _wr2[5];
	union WR3 _wr3[5];
	union WR4 _wr4[2];
	union WR5 _wr5[2];
	uint16_t _wr6[2];
	uint16_t _wr7[2];
	
	//TC6014µÄ²ÎÊı
	uint32_t R; //range  ·¶Î§
	uint32_t SV; //start velocity ³õÊ¼ËÙ¶È
	uint32_t V;  //drive velocity Çı¶¯ËÙ¶È
	uint32_t A;  //acceleration ¼ÓËÙ¶È
	uint32_t D;  //deceleration ¼ÓËÙ¶È
	uint32_t K;  //jerk  ¼ÓËÙ¶È±ä»¯ÂÊ
	uint32_t L;  //deceleration increase rate ¼õËÙ¶È±ä»¯ÂÊ
	
	uint8_t clock; //Ê±ÖÓ=fclk/16000000£¬fclk(Íâ²¿¾§ÕñÆµÂÊ)Ò»°ãÎª16MHz£¬¼´clock = 1
	uint16_t multiple; //±¶ÂÊ=8000000/R£¬×î´óÎª500£¬¼´R=16000Ê±
	uint32_t initial_speed; //³õÊ¼ËÙ¶È(pps)=SV*multiple*clock
	uint32_t drive_speed; //Çı¶¯ËÙ¶È(pps)=V*multiple*clock
	uint32_t acceleration; //¼ÓËÙ¶È(pps/sec)=A*125*multiple*clock*clock
	uint32_t deceleration; //¼õËÙ¶È(pps/sec)=D*125*multiple*clock*clock
	uint32_t jerk; //¼ÓËÙ¶È±ä»¯ÂÊ(pps/sec/sec/sec)=62.5*1000000*multiple*clock*clock*clock/K
	uint32_t dec_inc_tate; //¼õËÙ¶È±ä»¯ÂÊ(pps/sec/sec/sec)=62.5*1000000*multiple*clock*clock*clock/L
}TC6014_TypeDef;

extern TC6014_TypeDef* ptc6014_wregs;
extern uint32_t adr;

void select_chip(int chip);
int which_chip(void);
void wreg1(int axis,int wdata);
void wreg2(int axis,int wdata);
void wreg3(int axis,int wdata);
void command(int axis,int cmd);
void range(int axis,long wdata);
void acac(int axis,int wdata);
void dcac(int axis,int wdata);
void acc(int axis,int wdata);
void dec(int axis,int wdata);
void startv(int axis,int wdata);
void speed(int axis,int wdata);
void pulse(int axis,long wdata);
void decp(int axis,long wdata);
void center(int axis,long wdata);
void lp(int axis,long wdata);
void ep(int axis,long wdata);
void compp(int axis,long wdata);
void compm(int axis,long wdata);
void accofst(int axis,long wdata);
void hsspeed(int axis,int wdata);
void expmode(int axis,int em6data,int em7data);
void syncmode(int axis,int sm6data,int sm7data);
long readlp(int axis);
long readep(int axis);
void waitaxis(int axis);
void next_wait(void);
void bp_wait(void);
void homesrch(void);


void test(char flag);
	
#endif
