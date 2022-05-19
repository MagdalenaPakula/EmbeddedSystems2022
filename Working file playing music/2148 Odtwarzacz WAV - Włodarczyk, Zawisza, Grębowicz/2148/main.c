#include <lpc2xxx.h>
#include "irq/irq_handler.h"
#include "timer.h"
#include "VIC.h"
#include "sd.h"
#include "pff.h"
#include <printf_P.h>
#include <printf_init.h>
#include <string.h>
#include "lcd.h"
#include "spi.h"
#include "testMotor.c"
#include "wavFunctions.c"
#include "lcd_hw.h"
#include "lcd2.h"


FATFS fatfs; /* File system object */
DWORD rc;
char data_buffer_32[512];
WORD br;

char soundBufor0[10240];
char soundBufor1[10240];
char pliki[256*12];
extern int fillBufor;
int buforFlag = 0;
char nazwa[] = "TEMPLATE.wav";
int32 titleIndex = 0;
int32 isMusicPlaying = 0;
uint32 filesCount;

void main(void) {

	PINSEL1 &= ~0x000C0000;
	PINSEL1 |= 0x00080000;
	drawMenu();
	printf_init();
	lcdInit(); //Ekran expansion
	drawMenu();
	initLCD();//Ekran glowny
	LcdText("Systemy\nWbudowane");
	delay(2);
	LcdText("Odtwarzacz MP3");
	delay(2);
	LcdText("Inicjalizacja...");

	init();

	songNameOnLCD(nazwa, isMusicPlaying);

	/**************
	 * główna pętla
	 * P0.14 - odtwórz/zatrzymaj
	 * P0.17 - reset utworu
	 * P0.18 - następny utwór
	 * P0.19 - poprzedni utwór
	 * P0.20 - powrót do pierwszego utworu
	 *************/
	while (TRUE) {

		//odtwarzanie/zatrzymywanie P0.14
		if ((IOPIN0 & 0x00004000) == 0) {
			if(isMusicPlaying == 0) {
				init_irq(125, 20);
				isMusicPlaying = 1;
			}
			else
			{
				init_irq(0,0);
				isMusicPlaying = 0;
			}
			songNameOnLCD(nazwa, isMusicPlaying);
			delay(1);
		}

		//następny utwór P0.18
		if ((IOPIN0 & 0x00040000) == 0){
			titleIndex++;

			if(titleIndex == filesCount)
				titleIndex = 0;

			setSongName(nazwa, pliki, titleIndex);
			songNameOnLCD(nazwa, isMusicPlaying);
			testMotor();
			delay(1);
		}

		//poprzedni utwór P0.19
		if ((IOPIN0 & 0x00080000) == 0){
			titleIndex--;

			if(titleIndex == -1)
				titleIndex = filesCount-1;

			setSongName(nazwa, pliki, titleIndex);
			songNameOnLCD(nazwa, isMusicPlaying);
			testMotor();
			delay(1);
		}

		//reset P0.20
		if ((IOPIN0 & 0x00100000) == 0){
			titleIndex = 0;

			setSongName(nazwa, pliki, titleIndex);
			songNameOnLCD(nazwa, isMusicPlaying);
			delay(1);
		}

		//song to 0:00 P0.17
		if ((IOPIN0 & 0x00020000) == 0){
			setSongName(nazwa, pliki, titleIndex);
			songNameOnLCD(nazwa, isMusicPlaying);
			delay(1);
		}



		//sprawdź czy trzeba uzupełnić bufor
		if (fillBufor == 1) {
		  fillBufor = 0;

		  fillBuforWav();
		}
	}
}


void openFile(char *file){
	 simplePrintf("\nMontuje wolumin...\n");

	  rc = pf_mount( & fatfs);

	  if (FR_DISK_ERR == rc || FR_NOT_READY == rc) {
	    simplePrintf("Nie udalo sie zamontowac karty SD.\n");
	  } else if (FR_NO_FILESYSTEM == rc) {
	    simplePrintf("Nieprawidlowy system plikow na karcie pamieci.\n");
	  } else if (!rc) {

	    simplePrintf("Karta SD podlaczona poprawnie.\n");

	    rc = pf_open(file);
	  }
}








