/**
 * Sleeper helper class
  * Daniel Milesson 2014
 */
 
#ifndef Sleepy_h
#define Sleepy_h

#include <avr/wdt.h>
#include <avr/sleep.h>

#ifdef __AVR_MEGA__
#define WDTCR_REG WDTCSR
#endif

class Sleepy
{
  public:
    void PowerNap();
    void SleepMillis(uint32_t millis);
    void SleepSeconds(uint8_t seconds);
    void SleepMinutes(uint8_t minutes);
    void SleepHours(uint8_t hours);
  
  private:
    void sleepMillis(long millis);
    void sleep(int time_to_sleep);
    void setup_watchdog(int ii);

};

void Sleepy::SleepHours(uint8_t hours) { 
  SleepMillis((long) hours * 1000 * 60 * 60);
}

void Sleepy::SleepMinutes(uint8_t minutes) { 
  SleepMillis((long) minutes * 1000 * 60);
}

void Sleepy::SleepSeconds(uint8_t seconds) { 
  SleepMillis(((long) seconds) * (long) 1000);
}

void Sleepy::SleepMillis(uint32_t millis) { 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleepMillis(millis);
}

// Unsigned long covers 2,147,483,647 milliseconds or 24.85 days
void Sleepy::sleepMillis(long ms) {
  while (ms >= 8000) { sleep(WDTO_8S); ms -= 8000; }
  if (ms >= 4000)    { sleep(WDTO_4S); ms -= 4000; }
  if (ms >= 2000)    { sleep(WDTO_2S); ms -= 2000; }
  if (ms >= 1000)    { sleep(WDTO_1S); ms -= 1000; }
  if (ms >= 500)     { sleep(WDTO_500MS); ms -= 500; }
  if (ms >= 250)     { sleep(WDTO_250MS); ms -= 250; }
  if (ms >= 125)     { sleep(WDTO_120MS); ms -= 120; }
  if (ms >= 64)      { sleep(WDTO_60MS); ms -= 60; }
  if (ms >= 32)      { sleep(WDTO_30MS); ms -= 30; }
  if (ms >= 16)      { sleep(WDTO_15MS); ms -= 15; }
}

void Sleepy::sleep(int time_to_sleep) { 
   setup_watchdog(time_to_sleep);
   sleep_mode();
}

ISR(WDT_vect) {
  // Watchdog callback...
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void Sleepy::setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR_REG |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR_REG = bb;
  WDTCR_REG |= _BV(WDIE);
}

#endif
