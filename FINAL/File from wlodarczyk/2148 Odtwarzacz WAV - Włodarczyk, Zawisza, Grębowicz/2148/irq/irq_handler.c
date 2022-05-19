/*************************************************************************************
 * @Change log:
 *           func:
 *           1+2.mc card + spi
 *           3+4. timer + iterrupt
 *           5. dac dzwiek
 *           6. hitachi
 *           7. gpio
 *           8+9. eeprom + i2c
 *
 *
 ******************************************************************************/

#include "../general.h"
#include <lpc2xxx.h>
#include "irq_handler.h"
#include "../timer.h"
#include "../types.h"
#include "../Common_Def.h"

extern char soundBufor0[10240];
extern char soundBufor1[10240];
BYTE read_wynik[512];
extern int buforFlag;
int fillBufor = 0;
int cnt = -1;
//int iterator = 2;

void IRQ_Test(void)
{
	tS32 val;
	if(buforFlag%2 == 0)	val = soundBufor0[cnt] - 128;
	else					val = soundBufor1[cnt] - 128;

	val = val * 2;
	if (val > 127) val = 127;
	else if (val < -127) val = -127;

	DACR = ((val+128) << 8) | (1 << 16);         //BIAS = 1, 2.5uS settling time
	cnt++;
	if(cnt == 10240 || cnt == 0)
	{
		cnt = 0;
	    if(buforFlag%2 == 0)	buforFlag = 1;
	  	else					buforFlag = 0;

	  	fillBufor = 1;
	}

	T1IR   = TIMER_MR0_INT;
    VICVectAddr = 0x00;              // End of Interrupt -> informacja dla
                                      // kontrolera przerwań


}
