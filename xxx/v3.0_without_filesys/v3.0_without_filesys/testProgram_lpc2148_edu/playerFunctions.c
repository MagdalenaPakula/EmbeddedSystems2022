#include <lpc2xxx.h>
#include <general.h>
#include <lpc2xxx.h>
#include "lcd.h"

extern char soundBufor0[10240];
extern char soundBufor1[10240];
extern int buforFlag;
extern FATFS fatfs; /* File system object */
extern DWORD rc;
extern WORD br;
extern char data_buffer_32[512];
extern char nazwa[12];
extern char pliki[256*12];
extern int32 titleIndex;
extern uint32 filesCount;

void songNameOnLCD(char *nazwa, int32 isMusicPlaying);

void init()
{
	rc = pf_mount(&fatfs);
  	if (rc){
  		//simplePrintf("blad po mount %x \n", rc);
  		LcdText("Blad montowania");
  		if( FR_DISK_ERR == rc || FR_NOT_READY == rc ){
  			//simplePrintf("Blad interfejsu");
  			LcdText("Blad interfejsu");
  		}else if( FR_NO_FILESYSTEM == rc ){
  			//simplePrintf("Nieprawidlowy system plikow na karcie pamieci");
  			LcdText("Blad systemu\nplikow");
  		}
  	}


	rc = listDir("/", TRUE); //poka¿ dok³adn¹ zawartoœæ karty
  	if (rc){
  		simplePrintf("listDir, rc=%x\n", rc);
  	}


	filesCount = filesList("/", pliki) ; // stwórz liste plików
	if( filesCount == 0){
		simplePrintf("fileList => brak plikow, lub nie mozna ich odczytac");
	}

	simplePrintf("Liczba plikow WAV: %d\n", filesCount);


	setSongName(nazwa, pliki, titleIndex);


	rc = pf_open(nazwa);

	  initSpi();
	  sdInit();
	  adjustMotor();
}

static void init_irq(tU32 period, tU8 duty_cycle) {
  //Zainicjuj VIC dla przerwań od Timera #1
  VICIntSelect &= ~TIMER_1_IRQ; //Przerwanie od Timera #1 przypisane do IRQ (nie do FIQ)
  VICVectAddr5 = (tU32) IRQ_Test; //adres procedury przerwania
  VICVectCntl5 = VIC_ENABLE_SLOT | TIMER_1_IRQ_NO;
  VICIntEnable = TIMER_1_IRQ; // Przypisanie i odblokowanie slotu w VIC od Timera #1

  T1TCR = TIMER_RESET; //Zatrzymaj i zresetuj
  T1PR = 0; //Preskaler nieużywany
  T1MR0 = ((tU64) period) * ((tU64) PERIPHERAL_CLOCK) / 1000000;
  T1MR1 = (tU64) T1MR0 * duty_cycle / 100; //Wypełnienie
  T1IR = TIMER_ALL_INT; //Resetowanie flag przerwań
  T1MCR = MR0_I | MR0_R; //Generuj okresowe przerwania dla MR0 i dodatkowo dla MR1
  T1TCR = TIMER_RUN; //Uruchom timer
}

static void delay(tU32 seconds) {
  T0TCR = TIMER_RESET; //Zatrzymaj i zresetuj
  T0PR = PERIPHERAL_CLOCK - 1; //jednostka w preskalerze
  T0MR0 = seconds;
  T0IR = TIMER_ALL_INT; //Resetowanie flag przerwaĹ„
  T0MCR = MR0_S; //Licz do wartości w MR0 i zatrzymaj się
  T0TCR = TIMER_RUN; //Uruchom timer

  // sprawdź czy timer działa
  // nie ma wpisanego ogranicznika liczby pętli, ze względu na charakter procedury
  while (T0TCR & TIMER_RUN) {}
}

void setSongName(char *nazwa, char *pliki, int titleIndex)
{
	for(int m = 0; m < 8; m++)
	{
		nazwa[m] = pliki[titleIndex*12+m];
	}
	//LcdText(nazwa, TRUE);
	rc = pf_open(nazwa);

}

void fillBuforWav()
{
    for (int k = 0; k < 20; k++) {
    	rc = pf_read(data_buffer_32, sizeof(data_buffer_32), &br);

    	if (buforFlag % 2 == 0) {
        for (int i = 0; i < 512; i++) {
          soundBufor0[512 * k + i] = data_buffer_32[i];
        }
      } else {
        for (int i = 0; i < 512; i++) {
          soundBufor1[512 * k + i] = data_buffer_32[i];
        }
      }
    }

}

void songNameOnLCD(char *nazwa, int32 isMusicPlaying)
{
	char message[16];

	if(isMusicPlaying == 1)
	{
		message[0] = '|';
		message[1] = '>';
	}
	else
	{
		message[0] = 'I';
		message[1] = 'I';
	}

	message[2] = ' ';

	for(int i = 0; i < 12; i++)
	{
		message[3+i] = nazwa[i];
	}

	message[15] = ' ';

	LcdText(message);
}
