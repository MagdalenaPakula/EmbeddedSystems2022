#include <lpc2xxx.h>
#include "LCD.h"

static void delay37us(void) {
    volatile tU32 i;

    //Temp test for 140 uS delay
    for (i = 0; i < 6 * 2500; i++)
    {
    	asm volatile ("nop");
    }
    //delay 15 ns x 2500 = about 37 us delay
}

void
LcdText(char *str)
{
    char *ptr;
  
	initLCD();
    lcdBacklight(TRUE);
    delay37us();

    //function set
    writeLCD(0, 0x30);
    delay37us();
    writeLCD(0, 0x30);
    delay37us();
    writeLCD(0, 0x30);
    delay37us();

    //function set
    writeLCD(0, 0x38);
    delay37us();

    //display off
    writeLCD(0, 0x08);
    delay37us();

    //display clear
    writeLCD(0, 0x01);
    delay37us(); //actually only 1.52 mS needed

    //display control set
    writeLCD(0, 0x06);
    delay37us();

    //display control set
    writeLCD(0, 0x0c);
    delay37us();

    //cursor home
    writeLCD(0, 0x02);
    delay37us();

    for(ptr = str; *ptr; ptr++)
    {
        if(*ptr != '\n')
        {
            writeLCD(1, *ptr);
        }
        else
        {
            writeLCD(0, 0x80 | 0x40);
        }
        delay37us();
    }

    delay37us();

}

void
writeLCD(tU8 reg, tU8 data)
{
    volatile tU8 i;

    if (reg == 0)
    {
    	IOCLR1 = LCD_RS;
    }
    else
    {
    	IOSET1 = LCD_RS;
    }


    IOCLR0 = LCD_RW;
    IOCLR1 = LCD_DATA;
    IOSET1 = ((tU32)data << 16) & LCD_DATA;

    IOSET1 = LCD_E;
    for(i=0; i<16; i++)
    {
    	asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
    }
    IOCLR1 = LCD_E;
    for(i=0; i<16; i++)
    {
    	asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
    }
}


void
lcdBacklight(tU8 onOff)
{
    if (onOff == TRUE)
    {
    	IOSET0 = LCD_BACKLIGHT;
    }
    else
    {
    	IOCLR0 = LCD_BACKLIGHT;
    }
}

void
initLCD(void)
{
    IODIR1 |= (LCD_DATA | LCD_E | LCD_RS);
    IOCLR1 = (LCD_DATA | LCD_E | LCD_RS);

    IODIR0 |= LCD_RW;
    IOCLR0 = LCD_RW;

    IODIR0 |= LCD_BACKLIGHT;
    IOCLR0 = LCD_BACKLIGHT;
}
