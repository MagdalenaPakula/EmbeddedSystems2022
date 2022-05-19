/*
 * spi.h
 *
 *  Created on: 2014-12-22
 *      Author: embedded
 */

#include "types.h"
#include "integer.h"
#include "startup/lpc2xxx.h"

#ifndef SPI_H_
#define SPI_H_

#define SPIF	7

#define SPI_SCK_PIN    4   /* Clock*/
#define SPI_MISO_PIN   5   /* from Card*/
#define SPI_MOSI_PIN   6   /* to Card*/
#define SPI_SS_PIN	   11   /* Card-Select*/

#define SPI_PINSEL     PINSEL0
#define SPI_SCK_FUNCBIT   8
#define SPI_MISO_FUNCBIT  10
#define SPI_MOSI_FUNCBIT  12
#define SPI_SS_FUNCBIT    14

#define SPI_PRESCALE_REG  S0SPCCR
#define SPI_PRESCALE_MIN  8

#define  SPI_CS   0x00008000
#define SELECT_CARD()   IOCLR0 = (1<<SPI_SS_PIN) // IOCLR -> low = wlaczenie
#define UNSELECT_CARD()	IOSET0 = (1<<SPI_SS_PIN) // IOSET -> high = wylaczenie


#define CPHA 3
#define CPOL 4
#define MSTR 5
#define LSBF 6
#define SPIE 7

void setSpiSpeed(BYTE speed);
void initSpi(void);
BYTE my_spiSend(BYTE outgoing);
BYTE spiSend(BYTE toSend);
void send_SPI(unsigned char indata);
#endif /* SPI_H_ */
