#ifndef RA_BLINKER_H
#define RA_BLINKER_H

#include <Ticker.h>
#include "Blinker.h"

/**
 * Prevzaté z: https://github.com/petrbrouzda/RatatoskrIoT
 * 
 * Posledná zmena(Last change): 08.12.2021
 * 
 * @authors Petr Brouzda <petr.brouzda@gmail.com>; Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Petr Brouzda, Ing. Peter VOJTECH ml.
 * @license Apache License 2.0
 * @link       http://petak23.echo-msz.eu
 * @version 1.0.0
 * 
 * raBlinker - zajistuje blikani LEDkou
 * 
 * Zalozim si objekt
 *    raBlinker blinker( LED_PIN );
 * a prikaz:   
 *    int sekvence1[] = { 50, 500, 50, 2000, 0 };
 *    int sekvence2[] = { 500, 500, 500, 10, -1 };
 * a spustim:
 *    blinker.setCode( sekvence1 );
 * Zmena sekvence:
 *    blinker.setCode( sekvence2 );
 * Zastaveni:   
 *    blinker.off();
 * 
 * Sekvence je:
 *    - cas HIGH v msec
 *    - cas LOW v msec
 *    - cas HIGH v msec
 *    - cas LOW v msec
 *    ....
 *    - 0 nebo -1
 * 0 = opakovani (jede donekonecna stejnou sekvenci)
 * -1 = konec (zablika jednou urcenou sekvenci a skonci)
 */

class raBlinker
{
  public:
    raBlinker( int pin );
    void off();
    void setCode( int * code );
    void changeState();
    
  private:
    int * code;
    int position;
    int state = BLINKER_LED_OFF;
    int pin;
    Ticker ticker;
};

#endif
