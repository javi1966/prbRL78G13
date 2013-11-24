#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "ior5f100le.h"
#include "ior5f100le_ext.h"

#include "lcd.h"
#include "spi.h"

extern volatile unsigned char G_SPI_SendingData; //spi busy flag
extern volatile unsigned char G_SPI_ReceivingData; //spi busy flag


/*-------------------------------------------------------------------------*
 * Globals
 *-------------------------------------------------------------------------*/

int G_CharWidth = 5u;
int G_CharHeight = 7u;
const unsigned char ** iFont;
int CharX_Position;
int CharY_Position;
uint32_t CharX2_Position;
uint32_t CharY2_Position;



//************************************************************************
void SPI_Send( unsigned char *aData, unsigned long aLength)
{
  unsigned char noRXData;
  
	G_SPI_SendingData = 1;
	G_SPI_ReceivingData = 0;
        
        P1  &= ~(1<<0);  //CS -> P10 low
	
	

        spi10_SendReceiveData(aData, aLength, &noRXData);
        while(G_SPI_SendingData);
        
        P1 |= (1<<0);   //CS -> P10 high
    
    
  
}
//************************************************************************
void SPI_SendReceive(uint8_t *aTXData, uint32_t aTXLength, uint8_t *aRXData)
{
  
        G_SPI_SendingData = 1;
	G_SPI_ReceivingData = 1;
	
	 P1  &= ~(1<<0);  //CS -> P10 low
	
    //RMM delay_ms(1);
    
        spi10_SendReceiveData(aTXData, aTXLength, aRXData);
        while(G_SPI_SendingData || G_SPI_ReceivingData);
    
	 P1 |= (1<<0);   //CS -> P10 high
} 
//***********************************************************************
void CommandSend( uint8_t cCommand)
{
   //P1 &= ~(1<<5); // Assert P15 (LCD RS)
   //SPI_Send((unsigned char *)&cCommand, 1);
   //P1 |= (1<<5); // Deassert P15 (LCD RS)
   
   
    uint8_t dummyRX;
    
    P1 &= ~(1<<5);      // Assert P15 (LCD RS)
    P1 &= ~(1<<0);      // Assert P10 (LCD CS)

    spi10_SendReceiveData( (uint8_t *)&cCommand, 1, &dummyRX);
    while(G_SPI_SendingData );
   // CSIMK10 = 1U;       // disable INTCSI10 interrupt
   // SIO10 = cCommand;   // started by writing data to SDR[7:0]
    
                        // Short Delay
   // for(delay=0; delay<5; delay++); 
    
                       
     
  //  while(SSR02 & 0x0040U); // Wait for transmission end
                        // Short Delay
    
    
                     
  
    
    P1 |= (1<<0);       // Deassert P10 (LCD CS)
    
   // SPI_SendReceive((uint8_t *)&cCommand,1, (uint8_t *)&dummyRX);
    
    P1 |= (1<<5);       // Deassert P15 (LCD RS)
  
}  
//************************************************************************
void DataSend(uint8_t cData)
{
  //  P1 |= (1<<5); // Deassert P15 (LCD RS)

  //  SPI_Send((unsigned char *)&cData, 1);

  //  P1 |= (1<<5); // Deassert P15 (LCD RS)
   
    uint8_t dummyRX;
    
    P1 &= ~(1<<0);  // Assert P10 (LCD CS)
    
    
    
    spi10_SendReceiveData( (uint8_t *)&cData, 1, &dummyRX);
    while(G_SPI_SendingData );

    //spi10_SendReceiveData((unsigned char *)&cData, 1, &dummyRX);
   
                        // Wait for transmission end
    //while(SSR02 & 0x0040U); 
    
   // CSIMK10 = 1U;       // disable INTCSI10 interrupt
   // SIO10 = cData;       // started by writing data to SDR[7:0]
    
                        // Short Delay
   // for(delay=0; delay<5; delay++); 
    
                        // Wait for transmission end
   // while(SSR02 & 0x0040); 
    
    
    
    P1 |= (1<<0);       // Deassert P10 (LCD CS)
    
    //SPI_SendReceive((uint8_t *)&cData,1, (uint8_t *)&dummyRX);
  
  
} 
//************************************************************************
void ST7579_SetVO_Range(int32_t nValue0to254)
{
   

    if (nValue0to254 <= 254) {
        char cByteToSend = 0 ;
        char cLowHigh = 0 ;

        if (nValue0to254 > 127) {
            cByteToSend = ((nValue0to254 - 127) | 0x80) & 0x00FF ;
            cLowHigh = 1 ;
        }
        else {
            cByteToSend = (nValue0to254 | 0x80) & 0x00FF ;
        }

        /* Use Function Set 0 H[1:0]=(0,0) */
        /* Original Development hardcoded this as CommandSend(0x020) */
        CommandSend(LCD_FUNCTION_ZERO);
       // p_gw->iCommAPI->iCommandSend(LCD_FUNCTION_ZERO) ;
        /* Set VO range to Low if 0x04 or High if 0x05 */
        /* here low would be PRS=0 and high would be PRS=1 */
        if (cLowHigh == 0) CommandSend(0x04) ;
        if (cLowHigh == 1) CommandSend(0x05) ;
    
        /* Use Function Set 1 H[1:0]=(0,1) */
        /* Original Development hardcoded this as CommandSend(0x021) */
        CommandSend(LCD_FUNCTION_ONE);
        //p_gw->iCommAPI->iCommandSend(LCD_FUNCTION_ONE) ;
        /* Set PRS=0 to 2.94 V or PRS=1 2.95V to 6.75V */
        /* the value here can be 0 to 127 or 0x00 to 0x7F */
        CommandSend(cByteToSend) ;
    }
}
//************************************************************************
void ST7579_SetSystemBiasBooster( int8_t cValue0To17)
{
   

    if (cValue0To17 <= 17) {
        /* Select bias and boost settings from value */
        char cBias = 0 ;
        char cBoost = 0 ;
        switch (cValue0To17) {
            case 0:
                /* Bias level 1/4 Boost Times 3. */
                cBias = 0x17 ;
                cBoost = 0x99 ;
                break ;
            case 3:
                /* Bias level 1/5 Boost Times 3. */
                cBias = 0x16 ;
                cBoost = 0x99 ;
                break ;
            case 6:
                /* Bias level 1/6 Boost Times 3. */
                cBias = 0x15 ;
                cBoost = 0x99 ;
                break ;
            case 9:
                /* Bias level 1/7 Boost Times 3. */
                cBias = 0x14 ;
                cBoost = 0x99 ;
                break ;
            case 1:
                /* Bias level 1/4 Boost Times 4. */
                cBias = 0x17 ;
                cBoost = 0x9A ;
                break ;
            case 4:
                /* Bias level 1/5 Boost Times 4. */
                cBias = 0x16 ;
                cBoost = 0x9A ;
                break ;
            case 7:
                /* Bias level 1/6 Boost Times 4. */
                cBias = 0x15 ;
                cBoost = 0x9A ;
                break ;
            case 10:
                /* Bias level 1/7 Boost Times 4. */
                cBias = 0x14 ;
                cBoost = 0x9A ;
                break ;
            case 12:
                /* Bias level 1/8 Boost Times 4. */
                cBias = 0x13 ;
                cBoost = 0x9A ;
                break ;
            case 14:
                /* Bias level 1/9 Boost Times 4. */
                cBias = 0x12 ;
                cBoost = 0x9A ;
                break ;
            case 2:
                /* Bias level 1/4 Boost Times 5. */
                cBias = 0x17 ;
                cBoost = 0x9B ;
                break ;
            case 5:
                /* Bias level 1/5 Boost Times 5. */
                cBias = 0x16 ;
                cBoost = 0x9B ;
                break ;
            case 8:
                /* Bias level 1/6 Boost Times 5. */
                cBias = 0x15 ;
                cBoost = 0x9B ;
                break ;
            case 11:
                /* Bias level 1/7 Boost Times 5. */
                cBias = 0x14 ;
                cBoost = 0x9B ;
                break ;
            case 13:
                /* Bias level 1/8 Boost Times 5. */
                cBias = 0x13 ;
                cBoost = 0x9B ;
                break ;
            case 15:
                /* Bias level 1/9 Boost Times 5. */
                cBias = 0x12 ;
                cBoost = 0x9B ;
                break ;
            case 16:
                /* Bias level 1/10 Boost Times 5. */
                cBias = 0x11 ;
                cBoost = 0x9B ;
                break ;
            case 17:
                /* Bias level 1/11 Boost Times 5. */
                cBias = 0x10 ;
                cBoost = 0x9B ;
                break ;
            default:
                /* Bias level 7 Boost Times 3. */
                cBias = 0x14 ;
                cBoost = 0x99 ;
                break ;
        }

        /* Set Bias */
        /* Use Function Set 1 H[1:0]=(0,1) */
        /* Original Development hardcoded this as CommandSend(0x021) */
        
        
        CommandSend(LCD_FUNCTION_ONE);
        CommandSend(cBias) ;
        /* Set Booster */
        /* Use Function Set 3 H[1:0]=(1,1) */
        /* Original Development hardcoded this as CommandSend(0x023) */
        CommandSend(LCD_FUNCTION_THREE) ;
        /* Set default efficiency and multiplyer on voltage */
        CommandSend(cBoost) ;
    }
}
//************************************************************************
void ST7579_SetFrameRate( int32_t nRate55To137)
{
  
    /* Use Function Set 3 H[1:0]=(1,1) */
    /* Original Development hardcoded this as CommandSend(0x023) */
    CommandSend(LCD_FUNCTION_THREE) ;

    switch(nRate55To137) {
        case 55:
            /* Set frame rate of 55 */
            CommandSend(0x08) ;
            break ;
        case 65:
            /* Set frame rate of 65 */
            CommandSend(0x09) ;
            break ;
        case 68:
            /* Set frame rate of 68 */
            CommandSend(0x0A) ;
            break ;
        case 70:
            /* Set frame rate of 70 */
            CommandSend(0x0B) ;
            break ;
        case 73:
            /* Set frame rate of 73(Default) */
            CommandSend(0x0C) ;
            break ;
        case 76:
            /* Set frame rate of 76 */
            CommandSend(0x0D) ;
            break ;
        case 80:
            /* Set frame rate of 80 */
            CommandSend(0x0E) ;
            break ;
        case 137:
            /* Set frame rate of 137 */
            CommandSend(0x0F) ;
            break ;
        default:
            /* Set frame rate of 73(Default) */
              CommandSend(0x0C) ;
    }
}
//*****************************************************************************
void ST7579_SetPage( uint8_t cValue0To9)
{
    
   char cValueToSend = cValue0To9 | 0x40 ;

    if (cValue0To9 <= 9) {
        cValueToSend &= 0x4F ;
    
        // Use Function Set 0 H[1:0]=(0,0) 
       // Original Development hardcoded this as CommandSend(0x020) 
        CommandSend(LCD_FUNCTION_ZERO) ;
       // Set Ram Page of 0 to 9 where 0x40 is 0 and 0x49 is nine 
        CommandSend(cValueToSend) ;
   }
   
   
  
  //  CommandSend(LCD_FUNCTION_ZERO);
   // CommandSend(0x40 |  cValue0To9) ;  
      
  
}
//******************************************************************************
void ST7579_SetChar(  int8_t cValue0To101)
{
  
    char cValueToSend = cValue0To101 | 0x80 ;
    
    if (cValue0To101 <= 101) {
        cValueToSend &= 0xFF ;
    
        // Use Function Set 0 H[1:0]=(0,0) 
       // Original Development hardcoded this as CommandSend(0x020)
       // p_gw->iCommAPI->iCommandSend(LCD_FUNCTION_ZERO) ;
        CommandSend(LCD_FUNCTION_ZERO);
        // Set Ram Page of 0 to 9 where 0x40 is 0 and 0x49 is nine 
        CommandSend(cValueToSend) ;
    }
           
  
   // CommandSend(LCD_FUNCTION_ZERO);
   // CommandSend(0x80 | cValue0To101) ;  
      
}
//***********************************************************************
void ST7579_SetLine(  int8_t cValue0To66)
{
   

    if (cValue0To66 <= 66) {
        int8_t cData[10] = "" ;
        int8_t cValueToSendLow = 0 ;
        int8_t cValueToSendHigh = 0 ;
    
        cData[0] = 0 ;
        cData[1] = 0 ;
        cData[2] = 0 ;
        cData[3] = 0 ;
        cData[4] = 0 ;
        cData[5] = 0 ;
        cData[6] = 0 ;
    
        /*  binary 1111 1111 becomes 0000 0001 */
        cData[0] = 0x01 & cValue0To66 ;
        /*  binary 1111 1111 becomes 0000 0010 */
        cData[1] = 0x02 & cValue0To66 ;
        /*  binary 1111 1111 becomes 0000 0100 */
        cData[2] = 0x04 & cValue0To66 ;
        /*  binary 1111 1111 becomes 0000 1000 */
        cData[3] = 0x08 & cValue0To66 ;
        /* binary 1111 1111 becomes 0001 0000 */
        cData[4] = 0x010 & cValue0To66 ;
        /* binary 1111 1111 becomes 0010 0000 */
        cData[5] = 0x020 & cValue0To66 ;
        /* binary 1111 1111 becomes 0100 0000 */
        cData[6] = 0x040 & cValue0To66 ;
    
        /* binary 10 becomes binary 1 */
        cData[1] = cData[1] >> 1 ;
        /* binary 100 becomes binary 1 */
        cData[2] = cData[2] >> 2 ;
        /* binary 1000 becomes binary 1 */
        cData[3] = cData[3] >> 3 ;
        /* binary 1 0000 becomes binary 1 */
        cData[4] = cData[4] >> 4 ;
        /* binary 10 0000 becomes binary 1 */
        cData[5] = cData[5] >> 5 ;
        /* binary 100 0000 becomes binary 1 */
        cData[6] = cData[6] >> 6 ;
    
        cValueToSendLow = cData[6] | 0x04 ;
        cValueToSendHigh = cData[0] | cData[1] | cData[2] | cData[3] | cData[4] | cData[5] | 0x40 ;

        /* Use Function Set 1 H[1:0]=(0,1) */
        /* Original Development hardcoded this as CommandSend(0x021) */
        CommandSend(LCD_FUNCTION_ONE) ;

        /* Set Ram Start Line of 0 to 66 using a high and low value and two command sends */
       CommandSend(cValueToSendHigh) ;
       CommandSend(cValueToSendLow) ;
    }
}
//********************************************************************
void ST7579_Send16bitsCommand(int32_t nCommand)
{
  
   int8_t cCommandOne = (int8_t)((nCommand >> 8) & 0x00FF) ;
   int8_t cCommandTwo = (int8_t)(nCommand & 0x00FF) ;

    CommandSend(cCommandOne) ;
    CommandSend(cCommandTwo) ;
}
//****************************************************************************
void ST7579_Config()
{
  
   
    int i = 0 ;
    
    // Inialize Display 
    ST7579_SetSystemBiasBooster(5) ; //5
    ST7579_SetVO_Range(95) ;  //95
    for (i=0;i<1000;i++){}
    ST7579_SetFrameRate(137) ; //137
    
    
     // set up the bias and boost
    /*
    CommandSend(LCD_FUNCTION_ONE);
    CommandSend(0x16); // bias
    CommandSend(LCD_FUNCTION_THREE);
    CommandSend(0x9B); // boost

    // set VO range
    CommandSend(LCD_FUNCTION_ZERO);
    CommandSend(0x4);
    CommandSend(LCD_FUNCTION_ONE);
    CommandSend(0x80 | 80);

    
    
    ST7579_SetPage(0) ;
    ST7579_SetChar(0) ;
    for (i =0 ; i < 808; i++)
       DataSend(0x00);
    
    CommandSend(LCD_FUNCTION_ZERO);
    CommandSend(0x8 | 0x4) ;  
    
    */
  
  

}

//************************************************************************

void LCDInit (void)
{
  
     // Setup output pins
    P1 = (1<<0) | (1<<5);
    PM1 &= (unsigned char)~(1<<0);  // Set P10 (LCD CS) to output
    PM1 &= (unsigned char)~(1<<5);  // Set P15 (LCD RS) to output
  
  
  
    spi10_Init();
    ST7579_Config();
    
    
    
    ST7579_Send16bitsCommand(LCD_DISPLAY_NORMAL);
    LCDPatron();
    LCDClear();
   
   
  
    
}

//****************************************************************************


void LCDClear (void)
{ 
 
    uint32_t pCounter;
    uint32_t column;

    ST7579_SetLine(0);
    for (pCounter=0;pCounter<8;pCounter++)
    {
        ST7579_SetPage(pCounter);
        ST7579_SetChar(0);
        
        for (column=0; column<128; column++)   {
                  DataSend(0x00);
        }              
    }
    
     ST7579_SetPage( 0);
     ST7579_SetChar( 0);  
}
//*************************************************************************************
void LCDPatron()
{
  uint32_t pCounter;
   uint32_t column;
  
   for (pCounter=0;pCounter<8;pCounter++)  {
                        ST7579_SetLine( 0);
                        ST7579_SetChar( 0);              
                        ST7579_SetPage( pCounter);
                        for (column=0;column<16;column++)   {
                           DataSend(0xFF);
                           DataSend(0x01);
                           DataSend(0x01);
                           DataSend(0x01);
                           DataSend(0x01);
                           DataSend(0x01);
                           DataSend(0x01);
                           DataSend(0x01);
                        }
      }                                   
  
}  
//***********************************************************************************
//void LCDString (const char *aStr, uint8_t aX, uint8_t aY)
//{
//    GlyphSetXY (G_lcd, aX, aY);
 //   GlyphString(G_lcd, (uint8_t *)aStr, strlen(aStr));
//}

//*************************************************************************************
void LCDFont (LCDFONT  font)
{
  
    
   /*         switch (font) {
            #ifdef USE_GLYPH_FONT_BITMAP
                case GLYPH_FONT_BITMAP:
                    p_gw->iLCDAPI->iFont = Bitmaps_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_HELVR10
                case GLYPH_FONT_HELVR10:
                    p_gw->iLCDAPI->iFont = FontHelvr10_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_8_BY_16
                case GLYPH_FONT_8_BY_16:
                    p_gw->iLCDAPI->iFont = Font8x16_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_8_BY_8
                case GLYPH_FONT_8_BY_8:
                    p_gw->iLCDAPI->iFont = Font8x8_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_WINFREE
                case GLYPH_FONT_WINFREESYSTEM14_BY_16:
                    p_gw->iLCDAPI->iFont = FontWinFreeSystem14x16_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_5_BY_7
                case GLYPH_FONT_5_BY_7:
                    p_gw->iLCDAPI->iFont = Fontx5x7_table ;
                    break ;
            #endif
            #ifdef USE_GLYPH_FONT_6_BY_13
                case GLYPH_FONT_6_BY_13:
                    p_gw->iLCDAPI->iFont = Fontx6x13_table ;
                    break ;
            #endif
			#ifdef USE_GLYPH_FONT_LOGOS
                case GLYPH_FONT_LOGOS:
                    p_gw->iLCDAPI->iFont = Logos_table ;
                    break ;
            #endif
            }
  
    */
  
    
    switch (font) {
        case FONT_LOGOS:
                         iFont=Logos_table;
			 G_CharWidth = 16u;
			 G_CharHeight = 16u;
             break;

        case FONT_LARGE:
                         iFont = Font8x8_table;
			 G_CharWidth = 8u;
			 G_CharHeight = 8u;
             break;

        case FONT_SMALL:
                         iFont = Fontx5x7_table;
			 G_CharWidth = 5u;
			 G_CharHeight = 7u;
             break;
             
             
    case  FONT_HELVR10:
                         iFont = FontHelvr10_table; // width=3, height=14
			 G_CharWidth =  3u;
			 G_CharHeight = 14u;
      
    }
}
//***************************************************************************
void LCDSetXY (uint8_t x, uint8_t y)
{
  
  CharX_Position = x;
  CharY_Position = y;
 
  
} 
 
//****************************************************************************
void LCD_Char(char aChar)
{
  
  
  /*  const uint8_t *data = Fontx5x7_table[(int)aChar];
    for (i = 0; i < 5; i++)
    {
         DataSend(data[i + 2]);
    }*/
  
    const uint8_t *p_char;
    const uint8_t *p_charData;
    uint32_t page;
    uint8_t width;
    uint8_t height;
    uint32_t column;
    uint8_t debug_i=0;
    
          
            p_char = iFont[aChar];
            width = p_char[0];
            height = p_char[1];
            p_charData = &p_char[2];
            for (page=0; page<height; page+=8)  {
                debug_i=((CharY_Position+page) >> 3);
                ST7579_SetPage(debug_i);
                ST7579_SetChar(CharX_Position);
                for (column=0; column<width; column++, p_charData++)    {
                       DataSend(*p_charData);
                }
            }
	 CharX_Position += width;  
 
  
}  
//****************************************************************************
void LCDString(const char* str,uint8_t x,uint8_t y)
{
  
  int nIndex = 0 ;

    LCDSetXY(x,y);
    while (nIndex < strlen(str))
    {
        LCD_Char( str[nIndex]) ;
        nIndex++ ;
    }

  
}
//****************************************************************************
void LCDStringPos (const char *aStr, uint8_t aPos, uint8_t aY)
{
   unsigned char  x = aPos  * G_CharWidth;
   LCDString(aStr, x, aY);
}
//****************************************************************************
void  LCDPrintf(uint8_t aLine, uint8_t aPos, char *aFormat, ...)
{
    unsigned char  y;
    char buffer[100];
    va_list marker;
    
	y = aLine * G_CharHeight;
	
    // Convert to a string for output
    va_start(marker, aFormat);
    vsprintf(buffer, aFormat, marker);
    va_end(marker);
    
    LCDString(buffer, aPos, y);
}
//****************************************************************************
void LCDStringLine (const char *aStr, uint8_t aX, uint8_t aLine)
{
	uint8_t y = aLine * G_CharHeight;
        LCDString(aStr, aX, y);
}

//*************************************************************
uint8_t LCDCenter(uint8_t aStrLen)
{
	return (LCD_W-G_CharWidth*aStrLen)/2;
}
//**********************************************************
uint8_t LCDRight(uint8_t aStrLen)
{
	return LCD_W - G_CharWidth * aStrLen;
}

//****************************************************************************
uint8_t No_Clobber_Axes(uint8_t cmd, uint32_t col, uint32_t line)
{
    uint8_t new_cmd = cmd;
    
    if (col == ((LCD_W/2)-4))
        new_cmd = 0xFF;     // Preserve the y-axis
    else if (line <= ((LCD_H/2)-4) && (line+8) >= ((LCD_H/2)-4))
        new_cmd |= (1 << ((LCD_H/2)-4) % 8);
    
    return new_cmd;
}



//*****************************************************************************
void LCDDrawBlock(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2)
{
  uint32_t line;
  uint32_t column;
  uint8_t cmd = 0xFF;
  
   CharX_Position = aX1;
   CharY_Position = aY1;
   CharX2_Position= aX2;
   CharY2_Position= aY2;
  
                    line = CharY_Position - (CharY_Position % 8);   // Gets the nearest block boundary
                    cmd = 0xFF;
                    
                    if (CharY_Position < (line + 8))
                            cmd &= (0xFF << (CharY_Position - line));
                    
                    for ( ; line<=CharY2_Position; line+=8) {
                        
                        ST7579_SetPage( line/8);
                        ST7579_SetChar(CharX_Position);
                       
                        if ((line+8) > CharY2_Position) /* If we're ending midway through a line */
                            cmd &= (0xFF >> ((line+7) - CharY2_Position));
                        
                        /* This code fills in the row column by column, one px at a time */
                        for (column=CharX_Position; column<=CharX2_Position; column++)    {
                            DataSend(No_Clobber_Axes(cmd, column, line));
                        }
                        
                        cmd = 0xFF;
                    }
  
  
}  

//*******************************************************************************************
void LCDEraseBlock(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2)
{
  uint32_t line;
  uint32_t column;
   uint8_t cmd = 0xFF;
  
  
    CharX_Position = aX1;
    CharY_Position = aY1;
    CharX2_Position= aX2;
    CharY2_Position= aY2;
  
  
                    line =CharY_Position - (CharY_Position % 8);
                    cmd = 0x00;
                    
                    if (CharY_Position < (line + 8))
                            cmd &= (0xFF << (CharY_Position - line));
                    
                    for ( ; line<=CharY2_Position; line+=8) {
                        ST7579_SetPage( line/8);
                        ST7579_SetChar(CharX_Position);
                        for (column=CharX_Position; column<=CharX2_Position; column++)    {
                            DataSend(No_Clobber_Axes(cmd, column, line));
                        }
                    }
  
   
} 
/*

void LCDClearLine (uint8_t aLine)
{
    uint8_t y = aLine * G_CharHeight;

    GlyphEraseBlock(G_lcd, 0u, y, 95u, y + 7u);
}



void LCDStringLine (const char *aStr, uint8_t aX, uint8_t aLine)
{
	uint8_t y = aLine * G_CharHeight;
        LCDString(aStr, aX, y);
}

void LCDStringPos (const char *aStr, uint8_t aPos, uint8_t aY)
{
    uint8_t  x = aPos  * G_CharWidth;
    LCDString(aStr, x, aY);
}

void LCDStringLinePos (const char *aStr, uint8_t aPos, uint8_t aLine)
{
    uint8_t  x = aPos  * G_CharWidth;
    LCDStringLine(aStr, x, aLine);
}

void LCDStringAlign(const char *aStr, T_LCD_ALIGN aAlign, uint8_t aY)
{
	uint8_t x;
	switch(aAlign)
	{
		case LEFT:
			x = 0;
		break;
		case RIGHT:
			x = LCD_W - strlen(aStr)*G_CharWidth;
		break;
		case CENTER:
			x = LCD_W/2 - strlen(aStr)*G_CharWidth/2;
		break;
		default:
		break;
	}
	
	LCDString(aStr, x, aY);
}

void LCDChar(char aChar, uint8_t aX, uint8_t aLine)
{
    uint8_t  y = aLine * G_CharHeight;
	
	GlyphSetXY (G_lcd, aX, y);
    GlyphChar(G_lcd, (uint32_t)aChar);  
}

void LCDCharPos(char aChar, uint8_t aPos, uint8_t aLine)
{
    uint8_t  x;
    uint8_t  y;

    x = aPos  * G_CharWidth;
    y = aLine * G_CharHeight;
	
	GlyphSetXY (G_lcd, x, y);
    GlyphChar(G_lcd, (uint32_t)aChar);  
}

void  LCDPrintf(uint8_t aLine, uint8_t aPos, char *aFormat, ...)
{
    uint8_t  y;
    char buffer[100];
    va_list marker;
    
	y = aLine * G_CharHeight;
	
    // Convert to a string for output
    va_start(marker, aFormat);
    vsprintf(buffer, aFormat, marker);
    va_end(marker);
    
    LCDStringPos(buffer, aPos, y);
}

void  LCDTest()
{
	GlyphClearScreen(G_lcd);
	GlyphDrawTestPattern(G_lcd);
}

void  LCDInvert()
{
	GlyphInvertScreen(G_lcd);
}

uint8_t LCDCenter(uint8_t aStrLen)
{
	return (LCD_W-G_CharWidth*aStrLen)/2;
}
uint8_t LCDRight(uint8_t aStrLen)
{
	return LCD_W - G_CharWidth * aStrLen;
}


  */
  
  



