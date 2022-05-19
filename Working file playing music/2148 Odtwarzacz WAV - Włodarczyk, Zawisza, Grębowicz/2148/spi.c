#include <lpc2xxx.h>
#include "spi.h"

void setSpiSpeed(BYTE speed)
{
	speed &= 0xFE;
	if ( speed < SPI_PRESCALE_MIN  )
	{
		speed = SPI_PRESCALE_MIN;
	}

	SPI_PRESCALE_REG = speed;

}
void initSpi(void)
{
	// setup GPIO
	IODIR0 |= (1<<SPI_SCK_PIN)|(1<<SPI_MOSI_PIN)|(1<<SPI_SS_PIN); // |= - output, 1<< pin (np P0.11)
	IODIR0 &= ~(1<<SPI_MISO_PIN); // &= - input
									// &= (1<<10) | (1<<11)
	// set Chip-Select high - unselect card
	IOSET0 = (1<<SPI_SS_PIN); 

	// reset Pin-Functions
	SPI_PINSEL &= ~( (3<<SPI_SCK_FUNCBIT) | (3<<SPI_MISO_FUNCBIT) |
		(3<<SPI_MOSI_FUNCBIT) | (3<<SPI_SS_FUNCBIT) ); // Pin function Select register 0 (PINSEL1 - 0xE002 C000)

	SPI_PINSEL |= ( (1<<SPI_SCK_FUNCBIT) | (1<<SPI_MISO_FUNCBIT) |
		(1<<SPI_MOSI_FUNCBIT) );

	// enable SPI-Master
	S0SPCR = (1<<MSTR)|(0<<CPOL);
		// low speed during init 400kHz
	setSpiSpeed(150);

	CHAR i = 0;
	/* Send 20 spi commands with card not selected */
	for(i=0;i<21;i++)
	{
		my_spiSend(0xff);
	}
}

BYTE my_spiSend(BYTE outgoing)
{
	BYTE incoming;

	S0SPDR = outgoing;
	while( !(S0SPSR & (1<<SPIF)) ) ; // S0SPSR - SPI Status Register
	incoming = S0SPDR;				// SPI Data Register

	return(incoming);
}


BYTE spiSend(BYTE toSend)
{

	BYTE incoming;
	S0SPDR = toSend;
	while( !(S0SPSR & (1<<SPIF)) ) ;

	incoming = S0SPDR;

	return incoming;
}
void send_SPI(unsigned char indata)	// pierwzy bajt wiersz, drugi kolumna
{
  S0SPDR = indata;
  while(!(S0SPSR & 0x80));

}
