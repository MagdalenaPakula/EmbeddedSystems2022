#include "sd.h"

// for reading SD card we have to initalise SPI port (frequency of SPI: 15MHz)
CHAR sdInit(void) {
    SHORT i = 100;
    BYTE response;

    // 'i' defines how many times we will send command to the initiator (SPI port)
    do {
        sdCommand(0, 0, 0); // we are sending command 0
        response = sd8bResp(); // reading the value of SPI0 Data Register
    } while(response != 1 && i--);

    // return of appropriate errors when loop exits because of sending 'i' commands, not receiving the answer
    if (response != 1) {
        if (response == 0xFF) { // 0xFF = 255 in decimal
            return (-1);
        } else { // here we are checking error code
            sd8bRespError(response);
            return (-2);
        }
    }

    // we are waiting for information that the device has been initialised correctly (return 0)
    i = 32000;
    do {
        sdCommand(1, 0, 0);
        response = sd8bResp();

        if (response != 0) {
            sd8bRespError(response)
        }
    } while (response == 1 && i--);

    if (response != 0) {
        sd8bRespError(response);
        return (-3);
    }
    return (0);
}

void sdCommand(BYTE cmd, WORD parX, WORD parY) {
    SELECT_CARD();

    sendToSPI(0xFF); // 255
    sendToSPI(0x40 | cmd); // 64 in decimal
    sendToSPI((BYTE) (parX >> 8)); // sending most significant bit of parameter x
    sendToSPI((BYTE) (parX)); // sending least significant bit of parameter x
    sendToSPI((BYTE) (parY >> 8)); // sending most significant bit of parameter y
    sendToSPI((BYTE) (parY)); // sending least significant bit of parameter y
    sendToSPI(0x95); // decimal: 149, sending checksum - valid only for first command (0)
    sendToSPI(0xFF); // sending empty command
}

BYTE i;
BYTE response8b;

BYTE sd8bResp(void) {
    SELECT_CARD();

    // we will receive the reponse after 1-8 pings
    for (i = 0; i < 8; i++) {
        response8b = sendToSPI(0xFF);
        if(response8b != 0xFF) {
            return (response8b);
        }
    }
    return (response8b);
}

WORD response16b;
WORD sd16bResp(void) {
    SELECT_CARD();


}
