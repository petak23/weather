#include <Arduino.h>
#include <Ticker.h>
#include "ra_blinker.h"

raBlinker::raBlinker( int pin ) : pin(pin) {}

void raBlinker::changeState() {

  // Ak nemam code skonc
  if ( this->code==NULL ) return;

  // Ak pozicia = 0 a zároven code[0] je -1 alebo 0 tak vypni a skonč
  if ( this->position == 0 && 
      ( this->code[0]==-1 || this->code[0]==0 ) ) {
    digitalWrite( this->pin, BLINKER_LED_OFF );
    return;     
  }

  this->state = 1 - this->state; // Nahrada predosleho a inverzia stavu

  int pos = this->position;      // Zapametanie aktualnej pozicie
  digitalWrite( this->pin, this->state ); // praca s LED
  
  this->position++;              // Nasledujúca pozicia
  if( this->code[this->position] == 0 ) { // Ak kod na nasl. poz. je 0 vynuluj pozíciu
    this->position = 0;                   // ...
  }
  if( this->code[this->position] != -1 ) {  // Ak kod na pozicii != 1
    this->ticker.once_ms( this->code[pos], std::bind(&raBlinker::changeState, this)); // https://github.com/esp8266/Arduino/blob/master/libraries/Ticker/examples/TickerFunctional/TickerFunctional.ino
  }
}

void raBlinker::off() {
  noInterrupts();
    this->code = NULL;
    this->position = 0;
    this->ticker.detach();
  interrupts();

  this->state = BLINKER_LED_OFF;
  digitalWrite( this->pin, BLINKER_LED_OFF );
}

void raBlinker::setCode( int * code ) {
  noInterrupts();
    this->code = code;
    this->position = 0;
    this->state = BLINKER_LED_OFF;
  interrupts();

  this->changeState();
}