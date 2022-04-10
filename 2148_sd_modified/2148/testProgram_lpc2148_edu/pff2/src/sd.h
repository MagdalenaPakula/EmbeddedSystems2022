#ifndef _SD_H
#define _SD_H

#include "integer.h"

#define	CMDREAD		17
#define	CMDWRITE	24
#define	CMDREADCSD	9

//initialise reader
CHAR sdInit(void);
//check the file system
CHAR sdState(void);
//reading the register from the card and its size
//may not work
//CHAR sdgetDriveSize(void);
//send command
void sdCommand(BYTE cmd, WORD paramx, WORD paramy);

//8bit response
BYTE sdResp8b(void);
//if an error occurs
void sdResp8bError(BYTE value);
//16bit response
WORD sdResp16b(void);


#endif //_SD_H
