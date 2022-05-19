static void delay(tU32 seconds) {
  T0TCR = TIMER_RESET; //Zatrzymaj i zresetuj
  T0PR = PERIPHERAL_CLOCK - 1; //jednostka w preskalerze
  T0MR0 = seconds;
  T0IR = TIMER_ALL_INT; //Resetowanie flag przerwaĹ„
  T0MCR = MR0_S; //Licz do wartości w MR0 i zatrzymaj się
  T0TCR = TIMER_RUN; //Uruchom timer

  // sprawdź czy timer działa
  // nie ma wpisanego ogranicznika liczby pętli, ze względu na charakter procedury
  while (T0TCR & TIMER_RUN) {}
}

static void sdelay(tU32 seconds) {
  T0TCR = TIMER_RESET; //Zatrzymaj i zresetuj
  T0PR = PERIPHERAL_CLOCK - 1; //jednostka w preskalerze
  T0MR0 = seconds;
  T0IR = TIMER_ALL_INT; //Resetowanie flag przerwaĹ„
  T0MCR = MR0_S; //Licz do wartości w MR0 i zatrzymaj się
  T0TCR = TIMER_RUN; //Uruchom timer

  // sprawdź czy timer działa
  // nie ma wpisanego ogranicznika liczby pętli, ze względu na charakter procedury
  while (T0TCR & TIMER_RUN) {}
}
