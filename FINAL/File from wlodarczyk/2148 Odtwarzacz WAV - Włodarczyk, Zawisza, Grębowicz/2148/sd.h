/*
 * sd.h
 *
 *  Created on: 2014-12-22
 *      Author: embedded
 */


#include "types.h"
#include "pff.h"
#ifndef SD_H_
#define SD_H_

#define SPI_SS_PIN	11
#define SD_Disable() IO0SET = 1 << 11
#define SD_Enable() IO0CLR = 1 << 11
#include "integer.h"
#define	CMDREAD		17
#define	CMDWRITE	24
#define	CMDREADCSD	9


CHAR sdInit(void);
esint8 sd_readSector(euint32 address, euint8* buf, euint16 len);
esint8 sd_writeSector(euint32 address, BYTE* buf);
void sdCommand(BYTE cmd, WORD paramx, WORD paramy);
BYTE sdResp8b(void);
WORD sdResp16b(void);
void sdResp8bError(BYTE value);
CHAR sdState(void);
FRESULT listDir(char *path, uint8 first);
uint32 filesList(char* path, char* list);

#endif /* SD_H_ */
