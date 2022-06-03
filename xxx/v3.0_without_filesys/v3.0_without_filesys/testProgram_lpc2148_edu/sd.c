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
    response16b = (sd8bResp() << 8) & 0xFF00;
    response16b |= sendToSPI(0xFF);
    UNSELECT_CARD();
    return (response16b);
}

void sdResp8bError(BYTE value) {
    switch (value) {
        case 0x40: // it is 64 in decimal
            simplePrintf("The argument is out of bounds.");
            break;

        case 0x20: // it is 32 in decimal
            simplePrintf("The address is out of bounds.");
            break;

        case 0x10: // it is 16 in decimal
            simplePrintf("ERROR: during erase sequence error appeard.");
            break;

        case 0x08: // it is 8 in decimal
            simplePrintf("Cyclic Redundancy Check failed.");
            // CRC is a code with a predifined length used, among others, to check the correctness of stored data
            // the process writing the data calculates the CRC for the message and appends it to the data
            // then the process reading the data from the disk also calculates the CRC value
            // for the message and compares it with the received one
            // if the results are different, there has been a misrepresentation in data
            break;

        case 0x04: // also 4 in decimal
            simplePrintf("This command is illegal.");
            break;

        case 0x02: // also 2 in decimal
            simplePrintf("Erase disk.");
            break;

        case 0x01: // also 1 in decimal
            simplePrintf("Wait, card is initialising.");
            break;

        default:
            simplePrintf("Unknown error with error code: ", value);
            break;
    }
}

CHAR sdState(void) {
     WORD value;
     sdCommand(13, 0, 0);
     value = sd16bResp();

     switch(value) {
         case 0x000: // this is 0 in decimal of course
            return (1);
            break;
         case 0x001:
            simplePrintf("Card is locked.");
            break;
         case 0x002:
             simplePrintf("Lock/Unlock command failed.");
             break;
         case 0x004:
            simplePrintf("Unknown error. Card may be damaged.");
            break;
         case 0x008:
            simplePrintf("Error: problem with internal card controller.");
            break;
         case 0x0010:
            simplePrintf("Error-correction performance failed to correct data.");
            break;
         case 0x0020:
             simplePrintf("Violation of write protect.");
             break;
         case 0x0040:
            simplePrintf("Invalid sector selection.") // for erasing
            break;
         case 0x0080:
             simplePrintf("Out of range.");
             break;
         default:
            if (value > 0x00FF) {
                sd8bResp((BYTE)(value >> 8));
            } else {
                simplePrintf("Unknown error with error code: ", value);
            } break;
    }
return (-1);

}

esint8 sd_readSector(euint32 address, euint8* buffor, euint16 length) {
    euint8 cardResponse = 0x00;
    euint8 firstBlock = 0x00;
    euint8 c = 0x00;
    euint16 timeout = 0xFFFF; // 65 535 in decimal; first block?
    euint32 i = 0x00;
    euint32 place = 0x00;
    euint16 t =0;

    place = 512 * addres; // providing place from which we want to read

    sdCommand((BYTE)17, (euint16)(place >> 16), (euint16) place);

    cardResponse =sd8bResp();

// waiting for start block
    do {
        firstBlock = sd8bResp();
    } while(firstBlock == 0xFF && timeout--);

    if (cardResponse != 0x00 || firstBlock != 0xFE) {
        sdResp8bError(firstBlock);
        return (-1);
    }

    for (i = 0; i < 512; i++) {
        c = sendToSPI(0xFF);
        if (i < length) {
            buffor[i] = c;
        }
    }

    sendToSPI(0xFF);

    while(sendToSPI(0xFF) != 0xFF) {
        t++;
    }
    UNSELECT_CARD();
    return(0);
}

