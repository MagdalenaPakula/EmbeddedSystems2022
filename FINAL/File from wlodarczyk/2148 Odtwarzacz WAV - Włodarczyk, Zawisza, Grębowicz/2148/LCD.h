#include <general.h>

#ifndef LCD_H_
#define LCD_H_

#define LCD_DATA      0x00ff0000  //P1.16-P1.23
#define LCD_E         0x02000000  //P1.25
#define LCD_RW        0x00400000  //P0.22
#define LCD_RS        0x01000000  //P1.24
#define LCD_BACKLIGHT 0x40000000  //P0.30

void initLCD(void);
void lcdBacklight(tU8 onOff);
void writeLCD(tU8 reg, tU8 data);
void LcdText(char *str);

#endif /* LCD_H_ */
