#ifndef _SD_H
#define _SD_H

#define CMDREAD     17
#define CMDWRITE    24
#define CMDREADCSD  9

// initialise SD card - initialise reader
CHAR sdInit(void);

// check SD state - checking file system
CHAR sdState(void);

// sending command
void sdCommand(BYTE cmd, WORD parX, WORD parY);

// 8bit response
BYTE sd8bResp(void);

// if error occurs in 8bit response
void sd8bRespError(BYTE value);

// 16bit response
WORD sd16bResp(void);

//reading sector from SD card - we are providing address, buffor and length
esint8 sd_readSector(euint32 address, euint8* buffor, euint16 length);

//writing into SD card sector - providing address and buffor
esint8 sd_writeSector(euint32 address, BYTE* buffor);

#endif // _SD_H
