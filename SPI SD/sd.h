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

#endif // _SD_H
