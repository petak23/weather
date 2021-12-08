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
 * 
 * Ma se pouzivat stavova dioda pro signalizaci stavu aplikace?
 * Defaultne rozlisuje:
 * 1) spusten konfiguracni portal - rychle blikani
 * 2) aplikace bezi, WiFi vypnute - blik jednou za 2 sec - vypnuto pokud je definovano BLINKER_ULTRA_LOW_POWER nebo BLINKER_LOW_POWER
 * 3) aplikace bezi, WiFi zapnute - dvoublik jednou za 2 sec  - vypnuto pokud je definovano BLINKER_ULTRA_LOW_POWER nebo BLINKER_LOW_POWER
 * 4) cekame na pripojeni na WiFi - blik 2x za sekundu  - vypnuto pokud je definovano BLINKER_ULTRA_LOW_POWER 
 */
  #define USE_BLINKER
  /** (pokud je USE_BLINKER) pin, na kterem je stavova LED (muzete zadat LED_BUILTIN, pokud to na vasem zarizeni funguje) */
  #define BLINKER_PIN 2
  /** (pokud je USE_BLINKER) kdy je LED vypnuta? (ESP32 - nejcasteji LOW, ESP8266 - nejcasteji HIGH ) */
  #define BLINKER_LED_OFF LOW
  /** (pokud je USE_BLINKER) je-li definovano, blika se jen pro konfiguracni portal a pro situaci, kdy je zaple wifi ale neni pripojene k AP */
  // #define BLINKER_LOW_POWER
  /** (pokud je USE_BLINKER) je-li definovano, blika se jen pro konfiguracni portal */
  // #define BLINKER_ULTRA_LOW_POWER

  /** definice blikani pro jednotlive rezimy blinkeru (popis cisel je v Blinker.h !) */
  #define BLINKER_MODE_PORTAL { 15, 200, 0 }
  #if defined(BLINKER_ULTRA_LOW_POWER)
    #define BLINKER_MODE_SEARCHING { -1 }
    #define BLINKER_MODE_RUNNING { -1 }
    #define BLINKER_MODE_RUNNING_WIFI { -1 }
  #elif defined(BLINKER_LOW_POWER)
    #define BLINKER_MODE_SEARCHING { 15, 500, 0 }
    #define BLINKER_MODE_RUNNING { -1 }
    #define BLINKER_MODE_RUNNING_WIFI { -1 }
  #else
    #define BLINKER_MODE_SEARCHING { 15, 500, 0 }
    #define BLINKER_MODE_RUNNING { 15, 2000, 0 }
    #define BLINKER_MODE_RUNNING_WIFI { 15, 200, 15, 2000, 0 }
  #endif