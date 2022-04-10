/******************************************************************************
 *
 * Copyright:
 *    (C) 2000 - 2005 Embedded Artists AB
 *
 * Description:
 *
 * ESIC:
 *    pre_emptive_os
 *
 * Version:
 *    1.4.0
 *
 * Generate date:
 *    2005-03-15 at 20:27:15
 *
 * NOTE:
 *    DO NOT EDIT THIS FILE. IT IS AUTO GENERATED.
 *    CHANGES TO THIS FILE WILL BE LOST IF THE FILE IS RE-GENERATED
 *
 * Signature:
 *   7072655F656D70746976655F6F73,312E342E302E30,020235
 *   ,35,10104021013134373435363030,07323838,3732,01013
 *   830,0101013138303030303030,3135,33,3135,0232323530
 *   ,01020130,0231343734353539,3238313831,020101100302
 *   103030310010133,0163130,3230,3330,3430,3530,3630,3
 *   730,3830,3930,313030,313130,313230,313330,313430,3
 *   13530,313630,,35,35,35,35,35,35,35,35,35,35,35,35,
 *   35,35,35,35,,,,1001001100011000000000]484152445741
 *   5245,4C5043323130365F32,545538,756E7369676E6564206
 *   3686172,414C49474E4D454E54,34,54424F4F4C,756E73696
 *   76E65642063686172,54553332,756E7369676E656420696E7
 *   4,544D505F46494C4553,2A2E656C663B2A2E6C73743B2A2E6
 *   D61703B2A2E6F3B2A2E6F626A3B2A2E64,454E4449414E,4C4
 *   954544C45,54533332,7369676E656420696E74,545338,736
 *   9676E65642063686172,54553136,756E7369676E656420736
 *   86F7274,54533136,7369676E65642073686F7274,44455343
 *   52495054494F4E,,44454255475F4C4556454C,30,434F4445
 *   5F524F4F54,,47454E5F52554C4553,,4C494E455F5445524D
 *   ,43524C46,4252414345,,43524541544F52,416E646572732
 *   0526F7376616C6C,4352454154494F4E5F44415445,3230303
 *   52D30332D31352032303A31373A3432,524F4F54,433A2F446
 *   F63756D656E747320616E642053657474696E67732F416E646
 *   5727320526F7376616C6C2F4D696E6120646F6B756D656E742
 *   F456D62656464656420417274697374732F50726F647563747
 *   32F4C50433231303620525332333220517569636B537461727
 *   420426F6172642F72746F732F]505245464958,,4445425547
 *   5F4C4556454C,30,555345525F434F4D4D454E54,]64656661
 *   756C74,
 *
 * Checksum:
 *    278932
 *
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/

#include "kernel.h"
#include "event.h"
#include "pcb.h"
#include "hal.h"
#include "error.h"
#include "prioqueue.h"

/******************************************************************************
 * Local functions
 *****************************************************************************/

/******************************************************************************
 * Implementation of public functions
 *****************************************************************************/


/*****************************************************************************
 *
 * Description:
 *    Initializes an event structure. 
 *
 * Params:
 *    [in] pEvent - A pointer to an allocated event structure. 
 *
 ****************************************************************************/
void
eventInit(tOSEvent* pEvent)
{
  initPrioQueue(&(pEvent->waitQ), processControlBlocks);
}



/*****************************************************************************
 *
 * Description:
 *    This function moves the caller from the ready-list to the event queue. 
 *    The event queue is sorted by priority. If a timeout greater than zero is 
 *    specified the function will timeout after the specified time. The time 
 *    is specified in system ticks. 
 *
 * Params:
 *    [in] pEvent  - A pointer to an initialized event structure. 
 *    [in] timeout - The timeout time in system ticks. 
 *
 ****************************************************************************/
void
eventWait(tOSEvent* pEvent,
          tU32      timeout)
{
  pRunProc->flag |= (tU8)PROC_EVENT_WAIT;
  
  rmvFromRdyList();
  addToPrioQueue(&(pEvent->waitQ), pRunProc, EVENT_QUEUE);
  
  if(timeout > 0)
  {
    pRunProc->sleep = timeout;
    addToTimeList();
  } 
}


/*****************************************************************************
 *
 * Description:
 *    
 *
 * Params:
 *    [in] pEvent - A pointer to an initialized event structure. 
 *
 * Returns:
 *    The return status, see error codes below. 
 *
 * Error codes:
 *    OS_OK            - The function completed successfully. 
 *    OS_ERROR_TIMEOUT - The function returned due to a timeout. 
 *
 ****************************************************************************/
tU8
eventWaitReturn(tOSEvent* pEvent)
{
  tU8 ret = OS_OK;
  
  /* We will return to this point when we aquire the event or 
     we have timed out */
  if((pRunProc->flag & PROC_EVENT_WAIT) != 0)
  {
    pRunProc->flag &= ~(tU8)PROC_EVENT_WAIT;
    rmvFromPrioQueue(&(pEvent->waitQ), pRunProc, EVENT_QUEUE);
    ret = OS_ERROR_TIMEOUT;
  }
    
  return ret;
}


/*****************************************************************************
 *
 * Description:
 *    This function signals an event structure. When an event structure is 
 *    signaled the first process in the event queue will be inserted in the 
 *    ready-list. The event queue is sorted by priority. 
 *
 * Params:
 *    [in] pEvent - A pointer to an initialized event structure. 
 *
 ****************************************************************************/
void
eventSignal(tOSEvent* pEvent)
{
  if(eventIsEmpty(pEvent) == FALSE)
  {
    tOSPCB * pPCB;

    /* remove highest prio process from wait queue */
    pPCB = getHighPrioQueue(&(pEvent->waitQ), EVENT_QUEUE);
    rmvFromPrioQueue(&(pEvent->waitQ), pPCB, EVENT_QUEUE);
    pPCB->flag &= ~(tU8)PROC_EVENT_WAIT;

    /* process might be in time list, waiting for a timeout -> remove it */
    if((pPCB->flag & PROC_SLEEP) != 0)
      rmvFromTimeList(pPCB);

    /* insert process in ready list */
    addToRdyList(pPCB);

    if(isrNesting == 0) /* do not rechedule when called from an ISR */
    {
      schedule();
    }
  }
}


/*****************************************************************************
 *
 * Description:
 *    
 *
 * Params:
 *    [in] pEvent - A pointer to an initialized event structure. 
 *
 * Returns:
 *    
 *
 ****************************************************************************/
tBool
eventIsEmpty(tOSEvent* pEvent)
{
  return isEmptyPrioQueue(&(pEvent->waitQ));
}

/******************************************************************************
 * Implementation of local functions
 *****************************************************************************/
