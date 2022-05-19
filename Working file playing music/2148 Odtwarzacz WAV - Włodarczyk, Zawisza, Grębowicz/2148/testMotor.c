/******************************************************************************
 *
 * Copyright:
 *    (C) 2000 - 2005 Embedded Artists AB
 *
 *****************************************************************************/


#include <general.h>
#include <lpc2xxx.h>

/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/
void
testMotor(void)
{
  static tU32 stepmotorSteps[2] = {0x00201000, 0x00200000};//, 0x00000000, 0x00001000};  //P0.21 and P0.12 are used to contol the stepper motor
  static tU8 stepmotorIndex = 0;
  tU8 i;
  
  
  IODIR0 |= 0x00201000;
  IOCLR0  = stepmotorSteps[0];

    stepmotorIndex = (stepmotorIndex + 1);// & 0x03;

    IOCLR0 = stepmotorSteps[0];
    IOSET0 = stepmotorSteps[stepmotorIndex];

    if(stepmotorIndex == 2) stepmotorIndex = 0;

}
void
adjustMotor(void)
{
  static tU32 stepmotorSteps[2] = {0x00201000, 0x00200000};//, 0x00000000, 0x00001000};  //P0.21 and P0.12 are used to contol the stepper motor
  static tU8 stepmotorIndex = 0;
  tU8 i;


  IODIR0 |= 0x00201000;
  IOCLR0  = stepmotorSteps[0];

    IOCLR0 = stepmotorSteps[0];
    IOSET0 = stepmotorSteps[stepmotorIndex];

    if(stepmotorIndex == 2) stepmotorIndex = 0;

}

