#ifndef _SPI_H
#define _SPI_H

#include "integer.h"
#include "../startup/lpc2xxx.h" // this is from test program


// S0SPCR definitions - SPI0 Control Registers
#define CPHA 3 // Clock Phase: on which edge of serial clock (SCK) - leading(0) or trailing(1) - data are sampled
#define CPOL 4 // Clock Polarity
#define MSTR 5 // Master Mode Select - (0 - SPI operates in Slave Mode, 1 - SPI operates in Master Mode)
// S0SPSR definitions - SPI0 Status Register
#define SPIF 7 // SPI Transfer Complete Flag (0 - transfer not completed, 1 - transfer completed)

#define SPI_IODIR IODIR0 // IODIR is used to set direction of pins
#define SPI_SCK_PIN 4 // Serial Clock
#define SPI_MISO_PIN 5 // Master In Slave Out -> from card through pin to master
#define SPI_MOSI_PIN 6 // Master Out Slave In -> to card
#define SPI_SS_PIN 11 // Slave Select - pin used by master to select slave

#define SPI_PINSEL PINSEL0 // PINSEL - pin select register
#define SPI_SCK_FUNCBIT 8
#define SPI_MISO_FUNCBIT 10
#define SPI_MOSI_FUNCBIT 12
#define SPI_SS_FUNCBIT 14

#define SPI_PRESCALE_REG S0SPCR
#define SPI_PRESCALE_MIN 64

#define SELECT_CARD() IOCLR0 = (1<<SPI_SS_PIN) // we are using IOCLR0 to select slave // clears GPIO pins
#define UNSELECT_CARD() IOSET0 = (1<<SPI_SS_PIN) // IOSET disables communication with a slave // it is used to set GPIO pins

// initializing SPI
void SPI_Init(void);
// setting SPI speed
void setSPIspeed(BYTE speed);
// sending something to SPI
BYTE sendToSPI(BYTE toSend);



#endif