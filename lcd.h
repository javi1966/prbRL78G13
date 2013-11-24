#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>

/*-------------------------------------------------------------------------*
 * Defines
 *-------------------------------------------------------------------------*/
#define LCD_H 		64
#define LCD_W		96

#if 1
// MY reverse direction
#define LCD_FUNCTION_ZERO    0x028
#define LCD_FUNCTION_ONE     0x029
#define LCD_FUNCTION_TWO     0x02A
#define LCD_FUNCTION_THREE   0x02B
     // command sets
#define LCD_DISPLAY_REVERSE  0x280D
#define LCD_DISPLAY_NORMAL   0x280C
#else
// MY normal direction
#define LCD_FUNCTION_ZERO    0X20
#define LCD_FUNCTION_ONE     0X21
#define LCD_FUNCTION_TWO     0X22
#define LCD_FUNCTION_THREE   0X23
     // command sets
#define LCD_DISPLAY_REVERSE  0x200D
#define LCD_DISPLAY_NORMAL   0x200C
#endif


/* Global Fonts to all users of API at all times                             */
extern const unsigned char * FontHelvr10_table[256];
extern const unsigned char * Bitmaps_table[256];
extern const unsigned char * Font8x16_table[256];
extern const unsigned char * Font8x8_table[256];
extern const unsigned char * FontWinFreeSystem14x16_table[256];
extern const unsigned char * Fontx5x7_table[256];
extern const unsigned char * Fontx6x13_table[256];
extern const unsigned char * Logos_table[256];




typedef enum {
        FONT_HELVR10 = 3u,
	FONT_LOGOS   = 2u,
	FONT_LARGE   = 1u,
	FONT_SMALL   = 0u
}LCDFONT;

unsigned char LCDCenter(unsigned char aStrLen);
unsigned char LCDRight(unsigned char aStrLen);

void LCDInit(void);
void LCDClear(void);
void LCDPatron(void);
void LCDFont(LCDFONT font);
void LCDClearLine(unsigned char line);
void LCDString(const char* str,uint8_t x,uint8_t y);
void LCDStringPos(const char *aStr, unsigned char aPos, unsigned char aY);
void LCDStringLine(const char *aStr, unsigned char aX, unsigned char aLine);
void LCDStringLinePos(const char *aStr, unsigned char aPos, unsigned char aLine);
//void LCDStringAlign(const char *aStr, T_LCD_ALIGN aAlign, uint8_t aY);
void LCD_Char(char aChar);
void LCDCharPos(char aChar, unsigned char aPos, unsigned char aLine);
void LCDPrintf(unsigned char aLine, unsigned char aPos, char *aFormat, ...);
void LCDTest();
void LCDInvert();
void LCDDrawBlock(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2);

#endif
