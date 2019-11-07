/*
 * HexDataProcessor_priv.h
 *
 *  Created on: Oct 7, 2019
 *      Author: sondo
 */

#ifndef HEXDATAPROCESSOR_PRIV_H_
#define HEXDATAPROCESSOR_PRIV_H_


/* Maximum frame size in the hex line in u8 format */
#define u16MAXFRAMESIZE	261

/* Half u8 Definitions */
#define u8HALFBYTEOFFSET	4
#define u8HALFBYTEMASK	0x0F
#define u32BYTEMASK		0x000000FF
#define BYTEOFFSET		8


/*Half u32 Definitions*/
#define u8HALFWORDOFFSET	16
#define u32HALFWORDMASK		0x0000FFFF

/* u8 to u32 OFFSET Definitions */
#define u8FOURTHBYTEOFFSET		24
#define u8THIRDBYTEOFFSET		16
#define u8SECONDBYTEOFFSET		8

/* Fault Flag True/False Definitions */
#define FAULTFLAGTRUE	0
#define FAULTFLAGFALSE	1

/* Frame Start Indices */
#define BYTECOUNTSTARTINDEX		0
#define ADDRESSSTARTINDEX		1
#define RECORDTYPESTARTINDEX	3
#define DATASTARTINDEX			4


/* Frequently Used Numbers Defines */
#define DEF_u8ZERO		0
#define DEF_u8ONE		1
#define DEF_u8OXFF		0xFF




/*****************************************/
/***********Private Functions**************/
/*****************************************/

/****************************************************************************************/
/* Description: Convert Character form to Hex Form. EX. from 'A' to 0x0A				*/
/* Input      : u8 u8Character															*/
/*				Description: Character to be converted									*/
/* Output     : u8	                                                                 	*/
/*				Description: The Character after conversion to hex						*/
/*							 It returns 0xFF if the character is an invalid hex format	*/
/* Scope      : Private                                                                	*/
/****************************************************************************************/
u8 vidCharacterToHex(u8 u8Character);


#endif /* HEXDATAPROCESSOR_PRIV_H_ */
