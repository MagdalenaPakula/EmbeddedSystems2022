static void delay(tU32 seconds) {
  T0TCR = TIMER_RESET; //Stop and reset
  T0PR = PERIPHERAL_CLOCK - 1; //Unit in the pre-scaler
  T0MR0 = seconds;
  T0IR = TIMER_ALL_INT; //Reset interrupt flags
  T0MCR = MR0_S; //Count to the value in MR0 and stop
  T0TCR = TIMER_RUN; //Start the timer

  // check if the timer is working
  // there is no loop count delimiter, due to the nature of the procedure
  while (T0TCR & TIMER_RUN) {}
}

static void sdelay(tU32 seconds) {
  T0TCR = TIMER_RESET;
  T0PR = PERIPHERAL_CLOCK - 1;
  T0MR0 = seconds;
  T0IR = TIMER_ALL_INT;
  T0MCR = MR0_S;
  T0TCR = TIMER_RUN;

  while (T0TCR & TIMER_RUN) {}
}
