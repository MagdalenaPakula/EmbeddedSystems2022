#include "spi.h"
#include "integer.h"

// we set SPI to generate clock for data transfer with frequency 15MHZ

void SPI_Init(void) {
    // setup GPIO (general purpose input-output)
    SPI_IODIR |= (1 << SPI_SCK_PIN) | (1 << SPI_MOSI_PIN) | (1 << SPI_SS_PIN); // all of them are from master
    SPI_IODIR &= ~(1 << SPI_MISO_PIN); // MISO is from card to master, so here we have negation (remember that IODIR is used to set direction of pins)

    // pin-functions reset
    SPI_PINSEL &= ~((3 << SPI_SCK_FUNCBIT) | (3 << SPI_MISO_FUNCBIT) | (3 << SPI_MOSI_FUNCBIT) | (3 << SPI_SS_FUNCBIT));
    SPI_PINSEL |= (1 << SPI_SCK_FUNCBIT) | (1 << SPI_MOSI_FUNCBIT) | (1 << SPI_MISO_FUNCBIT);

    // enabling SPI Master
    S0SPCR = (0 << CPOL) | (1 << MSTR); // Clock Polarity and Master Mode Select in SPI0 Control Registers

    // setting SPI speed - during initialization it should be low (low means around 400kHZ)
    setSPIspeed(254);
}

void setSPIspeed(BYTE speed) {
    speed = 0xFE; // 0xFE = 254 in decimal which we wrote above

    if (speed < SPI_PRESCALE_MIN) {
        speed = SPI_PRESCALE_MIN;
    }

    SPI_PRESCALE_REG = speed;
}

BYTE in;

BYTE sendToSPI(BYTE toSend) {
    S0SPDR = toSend; // SPI0 Data Register - transmit data is provided to the SPI by writing to this register

    while (!(S0SPSR & (1 << SPIF))) //S0SPSR - SPI0 Status Register (read only), SPIF (SPI Transfer Complete Flag) {
        in = S0SPSR;
    return in;
}


