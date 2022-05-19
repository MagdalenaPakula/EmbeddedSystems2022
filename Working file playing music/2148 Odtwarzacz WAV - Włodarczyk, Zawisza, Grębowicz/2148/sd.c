#include <lpc2xxx.h>
#include "types.h"
#include <printf_P.h>
#include "sd.h"
#include "spi.h"
#include <string.h>

CHAR sdInit(void)
{

    SHORT i;
    BYTE resp;

    spiSend(0xff); // dummy write aby "wyczyscic" rejestr po uzyciu matrycy diod
    /* Try to send reset command up to 100 times */
    i=100;
    do
    {
        sdCommand(0, 0, 0); // wyslanie komendy 0
        resp=sdResp8b(); // odczyt wartosci rejestru S0SPDR
    }
    while(resp!=1 && i--);

    if( resp != 1) // jesli odpowiedz jest rozna od 1
    {
        if(resp == 0xff)
        {
            return(-1);
        }
        else // sprawdzenie kodu bledu
        {
            sdResp8bError(resp);
            return(-2);
        }
    }

    i=32000;
    do
    {
        sdCommand(1, 0, 0);

        resp=sdResp8b();
        if(resp!=0)
        {
            sdResp8bError(resp);
        }
    }
    while(resp == 1 && i--);

    if( resp != 0)
    {
        sdResp8bError(resp);
        return(-3);
    }

    return(0);

}

void sdCommand(BYTE cmd, WORD paramx, WORD paramy)
{
    SELECT_CARD();

    // SD_card_IODIR = IODIR;
    spiSend(0xff);

    spiSend(0x40 | cmd);
    spiSend((BYTE) (paramx >> 8)); /* MSB of parameter x */
    spiSend((BYTE) (paramx)); /* LSB of parameter x */
    spiSend((BYTE) (paramy >> 8)); /* MSB of parameter y */
    spiSend((BYTE) (paramy)); /* LSB of parameter y */

    spiSend(0x95); /* Checksum (should be only valid for first command (0) */

    spiSend(0xff); /* eat empty command - response */

    UNSELECT_CARD();

}


BYTE sdResp8b(void)
{

    BYTE i;
    BYTE resp8b;
    SELECT_CARD();
    /* Respone will come after 1 - 8 pings */
    for(i=0;i<8;i++)
    {
        resp8b = spiSend(0xff);
        if(resp8b != 0xff)
        return(resp8b);
    }
    UNSELECT_CARD();
    return(resp8b);
}

WORD resp16;
WORD sdResp16b(void)
{

    SELECT_CARD();
    resp16 = ( sdResp8b() << 8 ) & 0xff00;
    resp16 |= spiSend(0xff);
    UNSELECT_CARD();
    return(resp16);
}

void sdResp8bError(BYTE value)
{
    switch(value)
    {
        case 0x40:
        	simplePrintf("Argument out of bounds.n");
        	break;
        case 0x20:
        	simplePrintf("Address out of bounds.n");
        	break;
        case 0x10:
        	simplePrintf("Error during erase sequence.n");
			break;
        case 0x08:
        	simplePrintf("CRC failed.n");
        	break;
        case 0x04:
        	simplePrintf("Illegal command.n");
        	break;
        case 0x02:
        	simplePrintf("Erase reset (see SanDisk docs p5-13).n");
        	break;
        case 0x01:
        	simplePrintf("Card is initialising.n");
        	break;
        default:
        	simplePrintf("Unknown error 0x%x (see SanDisk docs p5-13).n",value);
        	break;
    }

}

CHAR sdState(void){
    WORD value;

    sdCommand(13, 0, 0);
    value = sdResp16b();

    switch(value)
    {
        case 0x000:
        	return(1);
        	break;
        case 0x0001:
        	simplePrintf("Card is Locked.n");
        	break;
        case 0x0002:
        	simplePrintf("WP Erase Skip, Lock/Unlock Cmd Failed.n");
        	break;
        case 0x0004:
        	simplePrintf("General / Unknown error -- card broken?.n");
        	break;
        case 0x0008:
        	simplePrintf("Internal card controller error.n");
        	break;
        case 0x0010:
        	simplePrintf("Card internal ECC was applied, but failed to correct the data.n");
        	break;
        case 0x0020:
        	simplePrintf("Write protect violation.n");
        	break;
        case 0x0040:
        	simplePrintf("An invalid selection, sectors for erase.n");
        	break;
        case 0x0080:
        	simplePrintf("Out of Range, CSD_Overwrite.n");
        	break;
        default:
			if(value>0x00FF)
			{
				sdResp8bError((BYTE) (value>>8));
			}
			else
			{
				simplePrintf("Unknown error: 0x%x (see SanDisk docs).n",value);
			}
			break;
    }
    return(-1);
}
esint8 sd_readSector(euint32 address, euint8* buf, euint16 len)
{
    euint8 cardresp=0x00;
    euint8 firstblock=0x00;
    euint8 c=0x00;
    euint16 fb_timeout=0xffff;
    euint32 i=0x00;
    euint32 place=0x00;
    euint16 t=0;

    place=512*address;
    sdCommand((BYTE)17, (euint16) (place >> 16), (euint16) place);

    cardresp=sdResp8b(); /* Card response */

    /* Wait for startblock */
    do
    {
    	firstblock=sdResp8b();
    }
    while(firstblock==0xff && fb_timeout--);

    if(cardresp!=0x00 || firstblock!=0xfe)
    {
        sdResp8bError(firstblock);
        return(-1);
    }

    for(i=0;i<512;i++)
    {
        c = spiSend(0xff);
        if(i<len)
        {
        	buf[i] = c;
        }
    }

    /* Checksum (2 byte) - ignore for now */
    spiSend(0xff);
    spiSend(0xff);

    spiSend(0xff);
    while(spiSend(0xff)!=0xff)
    {
        t++;
    }
    UNSELECT_CARD();
    return(0);
}

esint8 sd_writeSector(euint32 address, BYTE* buf) //BYTE
{
    euint32 place; //DWORD
    euint16 i; //WORD
    euint16 t=0;

    place=512*address;
    sdCommand((BYTE) 24, (WORD) (place >> 16), (WORD) place);
    sdResp8b(); /* Card response */
    spiSend(0xfe); /* Start block */
    for(i=0;i<512;i++)
    {
    	spiSend(buf[i]); /* Send data */
    }
    spiSend(0xff); /* Checksum part 1 */
    spiSend(0xff); /* Checksum part 2 */

    spiSend(0xff);

    while(spiSend(0xff)!=0xff)
    {
        t++;
    }

    UNSELECT_CARD();
    return(0);
}
FRESULT listDir(char* path, uint8 first){
	FRESULT res;
	FILINFO fno;
	DIR dir;
	int i;

	if(first == TRUE){
		simplePrintf("D-Directory R-Read_Only H-Hidden S-System A-Archive\n");
		simplePrintf("DRHSA path/fileName\n");
		simplePrintf("-------------------------------------------------------\n");
	}

	res = pf_opendir(&dir, path);
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = pf_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			if (fno.fattrib & AM_DIR) {
				char tmp[strlen(path) + 1 + strlen(fno.fname)];
				strcpy(tmp, path);
				if( path[1]!= "/" && path[1]!= 0 )
					strncat(tmp, "/", 1);
				strncat(tmp, fno.fname, strlen(fno.fname));

				simplePrintf("D---- %s\n",tmp);
				res = listDir(tmp, FALSE); //rekursywne przeszukiwanie katalogów
				if (res != FR_OK) break;
			} else { //wypisz atrybuty
				simplePrintf("-");
				if(fno.fattrib & AM_RDO)
					simplePrintf("R");
				else
					simplePrintf("-");

				if(fno.fattrib & AM_HID)
					simplePrintf("H");
				else
					simplePrintf("-");

				if(fno.fattrib & AM_SYS)
					simplePrintf("S");
				else
					simplePrintf("-");

				if(fno.fattrib & AM_ARC)
					simplePrintf("A");
				else
					simplePrintf("-");

				if( path[0]!= "/" && i != 1)
					simplePrintf(" %s/%s\n", path, fno.fname);
				else
					simplePrintf(" %s%s\n", path, fno.fname);
			}
		}
	}

	return res;
}
uint32 filesList(char* path, char* list){
	FRESULT res;
	FILINFO fno;
	DIR dir;
	uint32 i=0;

	res = pf_opendir(&dir, path);
	if (res == FR_OK) {
		for (;;) {
			res = pf_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			if (fno.fattrib & AM_DIR) {
				continue;
			} else {
				if( fno.fname[9] != 'W' || fno.fname[10] != 'A' ||
					fno.fname[11] != 'V' )
					continue;
				if( i >= 256)
					break;
				char *p = &list[i * 12];
				strncpy(p, fno.fname,12);
				i++;
			}
		}
	}
	return i;
}

