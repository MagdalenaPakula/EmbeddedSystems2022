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
 *    585510
 *
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/

#include "timer.h"
#include "general.h"

/******************************************************************************
 * Defines, macros, and typedefs
 *****************************************************************************/


/******************************************************************************
 * Local variables
 *****************************************************************************/

static tTimer *pTimerList = NULL;
static tTimer *pFiredListLast = NULL;
static tTimer *pFiredListFirst = NULL;
static tCntSem timerSem;
static tU8 pTimerStack[TIMERSTACK_SIZE];

/******************************************************************************
 * Local functions
 *****************************************************************************/

static void timerProcess(void* arg);

static void addToTimerList(tTimer* pTimer);

/******************************************************************************
 * Implementation of public functions
 *****************************************************************************/


/*****************************************************************************
 *
 * Description:
 *    This function checks if any timer has expired. This function is called 
 *    from the system tick function. 
 *
 ****************************************************************************/
void
timerTick(void)
{
  volatile tSR localSR;  /* declare temporary local space for status word */
  tTimer * pLast = NULL;
  tTimer * pTmp = NULL;
  tU8 error;

  m_os_dis_int();

  if(pTimerList != NULL)
  {
    pTimerList->delta--;
    if(pTimerList->delta == 0)
    {
      /* find last delta zero timer in list */
      pTmp = pTimerList;
      while(pTmp->delta == 0)
      {
        pLast = pTmp;
        pTmp = pTmp->next;
        osSemGive(&timerSem, &error);
        if(pTmp == NULL)
          break;
      }

      if(pFiredListFirst == NULL)
      {
        pFiredListFirst = pTimerList;
        pFiredListLast = pLast;
      }
      else
      {
        pFiredListLast->next = pTimerList;
        pFiredListLast = pLast;
      }
      pTimerList = pLast->next;
      if(pTimerList != NULL)
        pTimerList->previous = NULL;
      pFiredListLast->next = NULL;
    }
  }  
  m_os_ena_int();
}


/*****************************************************************************
 *
 * Description:
 *    This function initializes a timer. A timer is initialized with a timer 
 *    value, specified in timer ticks. When the specified time has elapsed the 
 *    timer is said to fire. When a timer fire the callback function of the 
 *    timer is executed. A timer can be set to be repeatable. A repeatable 
 *    timer is reactivated once the callback function has returned. The timer 
 *    structure must be allocated, statically or dynamically, by the user 
 *    before this function is used. osCreateTimer does not allocate the 
 *    structure, it initializes and queues the timer. 
 *
 * Params:
 *    [in] pTimer   - A pointer to an allocated timer structure. 
 *    [in] callback - The callback to be used when the timer fires. 
 *    [in] repeat   - If TRUE the timer will be reactivated as soon as the 
 *                    callback function has returned. 
 *    [in] time     - The initial timer value, specified in system ticks. 
 *
 ****************************************************************************/
void
osCreateTimer(tTimer* pTimer,
              void    (*callback) (void),
              tBool   repeat,
              tU32    time)
{
  volatile tSR localSR;  /* declare temporary local space for status word */
  pTimer->time = time;
  pTimer->delta = time;
  pTimer->callback = callback;
  pTimer->repeat = repeat;

  m_os_dis_int();
  addToTimerList(pTimer);
  m_os_ena_int();    
}


/*****************************************************************************
 *
 * Description:
 *    This function deletes a timer. The timer structure is not de-allocated, 
 *    only removed from the timer queue. If the timer has already fired and is 
 *    not repeatable there is no need to call this function. It is only 
 *    meaningful to call this function on a timer that is armed but not fired. 
 *
 * Params:
 *    [in]  pTimer - A pointer to the timer to delete. 
 *    [out] pError - The return status of the function. 
 *
 * Error codes:
 *    OS_OK         - The function completed successfully. 
 *    OS_ERROR_NULL - A NULL pointer was supplied to the function where it was 
 *                    not allowed. 
 *
 ****************************************************************************/
void
osDeleteTimer(tTimer* pTimer,
              tU8*    pError)
{
  volatile tSR localSR;  /* declare temporary local space for status word */
  tTimer * pTmp;

  *pError = OS_OK;
  if(pTimer == NULL)
  {
    *pError = OS_ERROR_NULL;

    return;
  }

  m_os_dis_int();

  /* check if timer is in timer list */
  for(pTmp = pTimerList; pTmp != NULL; pTmp = pTmp->next)
  {
    if(pTmp == pTimer)
    {
      if(pTmp->previous != NULL)
      {
        pTmp->previous->next = pTmp->next;
      }
      if(pTmp->next != NULL)
      {
             pTmp->next->previous = pTmp->previous;
             pTmp->next->delta += pTmp->delta;
      }
      if(pTmp == pTimerList)
      {
        pTimerList = pTmp->next;
      }
      m_os_ena_int();
      return;
    }
  }
  m_os_ena_int();
}


/*****************************************************************************
 *
 * Description:
 *    This function initializes the timer process. The timer process runs on 
 *    the highest priority, i.e. 0, and executes the timer callback function 
 *    when a timer expires. No other process should be run on this priority. 
 *
 * Params:
 *    [out] pError - The return status of the function. 
 *
 * Error codes:
 *    OS_OK             - The function completed successfully. 
 *    OS_ERROR_ALLOCATE - The timer process could not be created since there 
 *                        was no free process control blocks available. The 
 *                        number of process control blocks is specified during 
 *                        operating system configuration (maximum number of 
 *                        processes). 
 *
 ****************************************************************************/
void
osInitTimers(tU8* pError)
{
  tU8 pid;
  osSemInit(&timerSem, 0);
  osCreateProcess(timerProcess, pTimerStack, TIMERSTACK_SIZE, &pid, 0, NULL, pError);
  osStartProcess(pid, pError);
}

/******************************************************************************
 * Implementation of local functions
 *****************************************************************************/


/*****************************************************************************
 *
 * Description:
 *    The timer process runs on the highest priority, e.g. 0, and is 
 *    responsible for invoking the callback functions of expired timers. 
 *
 * Params:
 *    [in] arg - This parameter is not used. 
 *
 ****************************************************************************/
static void
timerProcess(void* arg)
{
  volatile tSR localSR;  /* declare temporary local space for status word */
  tTimer * pTimer;
  tU8 error;
  void (*callback)(void);

  while(TRUE)
  {
    osSemTake(&timerSem, 0, &error);
    m_os_dis_int();
    if(pFiredListFirst != NULL)
    {
      pTimer = pFiredListFirst;
      pFiredListFirst = pFiredListFirst->next;
      if(pFiredListFirst == NULL)
      {
        pFiredListLast = NULL;
      }    
      callback = pTimer->callback;
      if(pTimer->repeat == TRUE)
      {
        pTimer->delta = pTimer->time;
        addToTimerList(pTimer);
      }
      m_os_ena_int();
      callback();
    }
    else
    {
      m_os_ena_int();
    }
  }
}


/*****************************************************************************
 *
 * Description:
 *    Add a timer to the timer-list. 
 *
 * Params:
 *    [in] pTimer - The timer to add. 
 *
 ****************************************************************************/
static void
addToTimerList(tTimer* pTimer)
{
  tTimer * pTmp;
  tTimer * pPrev = NULL;

  if(pTimerList == NULL)
  {
    pTimerList = pTimer;
    pTimer->next = NULL;
    pTimer->previous = NULL;
  }
  else
  {
    for(pTmp = pTimerList; pTmp != NULL; pTmp = pTmp->next)
    {
      if(pTimer->delta <= pTmp->delta)
      {
        break;
      }
      pTimer->delta -= pTmp->delta;
      pPrev = pTmp;
    }
    if(pPrev == NULL) /* put first in list */
    {
      pTimer->next = pTimerList;
      pTimerList->previous = pTimer;
      pTimer->previous = NULL;
      pTimerList = pTimer;
    }
    else
    {
      pTimer->previous = pPrev;
      pTimer->next = pPrev->next;
      if(pPrev->next != NULL)
      {
        pPrev->next->previous = pTimer;
      }
      pPrev->next = pTimer;
    }
    /* update relative sleep time of next task */
    if(pTmp != NULL)
      pTmp->delta -= pTimer->delta;
  }
}
