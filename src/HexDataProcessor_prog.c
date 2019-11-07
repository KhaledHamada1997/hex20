/*
 * HexDataProcessor_prog.c
 *
 *  Created on: Oct 5, 2019
 *      Author: sondo
 */

#include "stdio.h"
#include "util.h"
#include "STD_Types.h"
#include "FLASH_int.h"
#include "HexDataProcessor_priv.h"
#include "HexDataProcessor_int.h"



/*****************************************/
/***********Public Functions**************/
/*****************************************/

/****************************************************************************************/
/* Description: Log the Driver Version													*/
/* Input      : Void																	*/
/* Output     : Void                                                                   	*/
/* Scope      : Public                                                                 	*/
/****************************************************************************************/
void HexDataProcessor_vidGetVersionInfo(void)
{

}



/****************************************************************************************/
/* Description: Get the Hex Data from the hex line and return it in the form of hex data*/
/* 				structure																*/
/* Input      : u8* u8HexLineArray														*/
/*				Description: The Array containing the Hex Line to extract the data from	*/
/*				HexData *pstrHexExtractedData											*/
/*				Description: Structure to put the extracted data in						*/
/* Output     : Void                                                                   	*/
/* Scope      : Public                                                                 	*/
/****************************************************************************************/
void HexDataProcessor_vidGetHexData(u8* au8HexLineArray, HexData *pstrHexExtractedData)
{
	u8 u8CharacterCount = 0;
	u8 u8ByteCounter = 0;
	u8 au8HexData[u16MAXFRAMESIZE];
	u8 FaultFlag = FAULTFLAGFALSE;
	u32 temp;
	u8 u8Counter = 0;
	/* Check the array is not null */
	if (au8HexLineArray != NULL)
	{
		/* Check the pointer is not null */
		if (pstrHexExtractedData != NULL)
		{
			/* Check the first element of the line is ':' */
			if (au8HexLineArray[0] == ':')
			{
				/* Loop on the array elements and change them from Ascii to hex */
				for (u8CharacterCount = 1; ((au8HexLineArray[u8CharacterCount] != '\n') && (au8HexLineArray[u8CharacterCount] != ':') && (FaultFlag == FAULTFLAGFALSE) ) ; u8CharacterCount++)
				{
					au8HexLineArray[u8CharacterCount] = vidCharacterToHex(au8HexLineArray[u8CharacterCount]);
					if (au8HexLineArray[u8CharacterCount] == DEF_u8OXFF)
					{
						FaultFlag = FAULTFLAGTRUE;
					}
				}
				/* Decrement the number of characters to evaluate the final number of characters */
				u8CharacterCount--;
				/* Check the number of characters is an even number to check the hex file isn't corrupted */
				if (u8CharacterCount % 2 != 0)
				{
					FaultFlag = FAULTFLAGTRUE;
				}
				/* Check no error occured in the file  */
				if (FaultFlag == FAULTFLAGFALSE)
				{
					/* Convert from the ascii-hex format to hex format. EX from 0A0A to AA*/
					for (u8ByteCounter = 0; u8ByteCounter < u8CharacterCount/2 ; u8ByteCounter++)
					{
						au8HexData[u8ByteCounter] = (au8HexLineArray[(u8ByteCounter*2) + 1] << u8HALFBYTEOFFSET) \
								| (au8HexLineArray[ (u8ByteCounter*2) + 2] & u8HALFBYTEMASK) ;
					}

					/* Divide the bytes to their corresponding elements in the structure */
					pstrHexExtractedData->u8DataLength = au8HexData[BYTECOUNTSTARTINDEX];
					pstrHexExtractedData->enuDataRecord = (RecordType)au8HexData[RECORDTYPESTARTINDEX];
					pstrHexExtractedData->u16DataAddress = (u16)((((u16)(au8HexData[ADDRESSSTARTINDEX] << BYTEOFFSET))) | (u16)(au8HexData[ADDRESSSTARTINDEX + 1]));
					pstrHexExtractedData->u8CheckSum = au8HexData[DATASTARTINDEX + pstrHexExtractedData->u8DataLength];

					for (u8Counter = 0; u8Counter < ( (pstrHexExtractedData->u8DataLength / 4) + 1) ; u8Counter++)
					{
						/* Take every 4 bytes and put them in the form of u32 in the data array of the structure */
						pstrHexExtractedData->au32Data[u8Counter] = (u32) ( (au8HexData[DATASTARTINDEX + (u8Counter*4)] ) \
																		| (au8HexData[DATASTARTINDEX + (u8Counter*4) + 1] << u8SECONDBYTEOFFSET) \
																		| (au8HexData[DATASTARTINDEX + (u8Counter*4) + 2] << u8THIRDBYTEOFFSET)  \
																		| (au8HexData[DATASTARTINDEX + (u8Counter*4) + 3]) << u8FOURTHBYTEOFFSET);
					}

					/* The last element of the array may have garbage because the data length may not be divisible by 4 */
					/* so we should replace the garbage by 0xFF if they are present										*/
					if (pstrHexExtractedData->u8DataLength % 4 != 0)
					{
						/* Shift the data to the most significant bits to add FF in the rest of them */
						//wrong according to flasher sequence, just add FF in the MSBs without shifting
//						pstrHexExtractedData->au32Data[(pstrHexExtractedData->u8DataLength / 4)] = pstrHexExtractedData->au32Data[(pstrHexExtractedData->u8DataLength / 4)] << ((4 - (pstrHexExtractedData->u8DataLength % 4)) * BYTEOFFSET);;
						for (u8Counter = (pstrHexExtractedData->u8DataLength % 4) ; u8Counter < 4; u8Counter++)
						{
							pstrHexExtractedData->au32Data[(pstrHexExtractedData->u8DataLength /4)] |= (DEF_u8OXFF << ( u8Counter * BYTEOFFSET) );
						}
						/* Add the number of bytes added to the data length */
						pstrHexExtractedData->u8DataLength += (4 - (pstrHexExtractedData->u8DataLength % 4));
					}
					else
					{
						/* Do Nothing */
					}
					/* Convert the data length to the data length in u32 form */
					pstrHexExtractedData->u8DataLength = (pstrHexExtractedData->u8DataLength / 4);
				}
			}
			else
			{
				/* Do Nothing */
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
	else
	{
		/* Do Nothing */
	}
}




/****************************************************************************************/
/* Description: Store Hex Data in the Flash												*/
/* Input      : HexData *astrHexDataArray												*/
/*				Description: Array of hex data structures to be stored in the flash		*/
/*				u8 u8HexSize															*/
/*				Description: Structure Array length										*/
/* Output     : u32                                                                  	*/
/*				Description: Address of the startup code of the hex file				*/
/* Scope      : Public                                                                 	*/
/****************************************************************************************/
u32 HexDataProcessor_u32StoreHexInFlash(HexData *astrHexDataArray, u16 u16HexSize)
{
	static u16 u16AddressOffset = 0;
	u8 u8Counter = 0;
	u32 u32ReturnAddress = 0;
	u32 u32TempAddress = 0;
	u8 u8ExitFlag = FAULTFLAGFALSE;
	/* Check array is not equal NULL */
	if (astrHexDataArray != NULL)
	{
		/* Loop on the structure array */
		for (u8Counter = 0; u8Counter < u16HexSize; u8Counter++)
		{
			if (u8ExitFlag == FAULTFLAGFALSE)
			{
				switch(astrHexDataArray[u8Counter].enuDataRecord)
				{
				case Data:
					/* Check the offset Address was already initialized by an Extended Linear Address */
					if (u16AddressOffset != DEF_u8ZERO)
					{
						/* Add the address of the data to the data address offset */
						u32TempAddress = ((u32)(u16AddressOffset << u8HALFWORDOFFSET)) | (astrHexDataArray[u8Counter].u16DataAddress);

						/* Write the Data in Flash */
						FLASH_vidWriteArray((u32*)u32TempAddress, astrHexDataArray[u8Counter].au32Data, astrHexDataArray[u8Counter].u8DataLength);
					}
					else
					{
						/* Do Nothing */
					}
					break;

				case EndOfLine:
					/* Set The Exit Flag */
					u8ExitFlag = FAULTFLAGTRUE;
					break;

				case ExtendedSegmentAddress:
					break;

				case StartSegmentAddress:
					break;

				case ExtendedLinearAddress:
					/*  Check the address is zero and the data length is 1 */
					if (astrHexDataArray[u8Counter].u8DataLength == DEF_u8ONE)
					{
						if (astrHexDataArray[u8Counter].u16DataAddress == DEF_u8ZERO)
						{
							/* Put the address offset in the static variable */
							u16AddressOffset = (u16)( astrHexDataArray[u8Counter].au32Data[DEF_u8ZERO]  & u32HALFWORDMASK );
//							u16AddressOffset = ( (astrHexDataArray[u8Counter].au32Data[DEF_u8ZERO] >> u8HALFWORDOFFSET) & u32HALFWORDMASK );
							u16AddressOffset = (u16AddressOffset << BYTEOFFSET ) | (u16AddressOffset >> BYTEOFFSET);
						}
						else
						{
							/* Do Nothing */
						}
					}
					else
					{
						/* Do Nothing */
					}
					break;

				case StartLinearAddress:
					/*  Check the address is zero and the data length is 1 */
					if (astrHexDataArray[u8Counter].u8DataLength == DEF_u8ONE)
					{
						if (astrHexDataArray[u8Counter].u16DataAddress == DEF_u8ZERO)
						{
							/* Put the address in the return variable */
							u32ReturnAddress = astrHexDataArray[u8Counter].au32Data[DEF_u8ZERO];
							u32ReturnAddress = (u32ReturnAddress << u8FOURTHBYTEOFFSET) \
												| (u32ReturnAddress >> u8FOURTHBYTEOFFSET) \
												| (((u32ReturnAddress >> u8THIRDBYTEOFFSET) & u32BYTEMASK) << u8SECONDBYTEOFFSET) \
												| (((u32ReturnAddress >> u8SECONDBYTEOFFSET) & u32BYTEMASK) << u8THIRDBYTEOFFSET);
						}
						else
						{
							/* Do Nothing */
						}
					}
					else
					{
						/* Do Nothing */
					}
					break;

				default:
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{

	}
	return u32ReturnAddress;
}




/*****************************************/
/***********Private Functions**************/
/*****************************************/
u8 vidCharacterToHex(u8 u8Character)
{
	u8 u8Return = 0xFF;
	if ( (u8Character >= '0') && (u8Character <= '9') )
	{
		u8Return = u8Character - '0';
	}
	else if ( (u8Character >= 'A') && (u8Character <= 'F') )
	{
		u8Return = u8Character - 'A' + 0x0A;
	}
	else
	{

	}
	return u8Return;
}
