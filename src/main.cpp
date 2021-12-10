#include <Arduino.h>

/* 
 * Program pre meteorologickú stanicu pomocou ESP8266 a MQTT pre IoT
 *
 * Posledná zmena(last change): 06.12.2021
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.0.4
 */

//+++++ RatatoskrIoT +++++

  #include "AppConfig.h"

  // RA objects
  ratatoskr* ra;
  raLogger* logger;
  raConfig config;
  Tasker tasker;

  // stav WiFi - je pripojena?
  bool wifiOK = false;
  // cas, kdy byla nastartovana wifi
  long wifiStartTime = 0;
  // duvod posledniho startu, naplni se automaticky
  int wakeupReason;
  // je aktualni start probuzenim z deep sleep?
  bool deepSleepStart;

  #ifdef RA_STORAGE_RTC_RAM 
    RTC_DATA_ATTR unsigned char ra_storage[RA_STORAGE_SIZE];
  #endif

  #include <Blinker.h>

  #ifdef USE_BLINKER
    raBlinker blinker( BLINKER_PIN );
    int blinkerPortal[] = BLINKER_MODE_PORTAL;
    int blinkerSearching[]  = BLINKER_MODE_SEARCHING;
    int blinkerRunning[] = BLINKER_MODE_RUNNING;
    int blinkerRunningWifi[] = BLINKER_MODE_RUNNING_WIFI;;
  #endif  

//----- RatatoskrIoT ----

#include "DHT.h"
#include "definitions.h"


float humidity = 0;
float temperature = 0;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// ID kanalu pro posilani dat
int ch1;
int ch2;

/**
 * Nacti hodnotu a posli ji na server. Spousteno periodicky z Taskeru.
 */
void taskReadDHT() {
  //Nacitaj udaje z DHT
  humidity = dht.readHumidity();    // Načítanie vlhkosti
  temperature = dht.readTemperature(); // Načítanie teploty v °C

  if (isnan(humidity) || isnan(temperature)) { // Kontrola načítania dát zo senzora
    #if SERIAL_PORT_ENABLED
      Serial.println("Failed to read from DHT sensor!");
    #endif
    return;
  }
  
  ra->postData( ch1, 1, humidity );
  ra->postData( ch2, 1, temperature );
}



/*
 * Pokud user code oznaci posledni poslana data znackou 
 *    ra->setAllDataPreparedFlag();
 * automaticky se zavola, jakmile jsou vsechna data odeslana.
 * 
 * Typicke pouziti je ve scenari, kdy chceme po probuzeni odeslat balik dat a zase zarizeni uspat.
 * 
 * Pro pripad, ze se odeslani z nejakeho duvodu nezdari, doporucuji do setup() pridat zhruba toto:
 *    tasker.setTimeout( raAllWasSent, 60000 );
 * s nastavenym maximalnim casem, jak dlouho muze byt zarizeni probuzeno (zde 60 sec).
 * Tim se zajisti, ze dojde k deep sleepu i v pripade, ze z nejakeho duvodu nejde data odeslat.
 */
void raAllWasSent()
{
  // v teto aplikaci se nepouziva
}


/**
 * Vraci jmeno aplikace do alokovaneho bufferu.
 * Jmeno aplikace by nemelo obsahovat strednik.
 */
void raGetAppName( char * target, int size )
{
  snprintf( target, size, "%s, %s %s", 
            __FILE__, __DATE__, __TIME__ 
            );  
  target[size-1] = 0;
}


void setup() {
  //+++++ RatatoskrIoT +++++
    // Mel by byt jako prvni v setup().
    // Pokud je parametr true, zapne Serial pro rychlost 115200. Jinak musi byt Serial zapnuty uz drive, nez je tohle zavolano.
    ratatoskr_startup( true );
  //----- RatatoskrIoT ----

  //++++++ user code here +++++
  dht.begin();

  // nadefinujeme kanal
  ch1 = ra->defineChannel( DEVCLASS_CONTINUOUS_MINMAXAVG, 2, "humidity", 3600 );
  ch2 = ra->defineChannel( DEVCLASS_CONTINUOUS_MINMAXAVG, 1, "temperature", 3600 );

  // nastavime spousteni mereni jednou za minutu
  tasker.setInterval( taskReadDHT, 60000 );

  // nacteme jednou hodnotu, aby se prvni nacetla hned a ne az za 60 sekund
  taskReadDHT();
  
  //------ user code here -----
}


void loop() {
  //+++++ RatatoskrIoT +++++
    // do scheduled tasks
    tasker.loop();
  //----- RatatoskrIoT ----

  //++++++ user code here +++++
  //------ user code here ------
}


//------------------------------------ callbacks from WiFi +++


void wifiStatus_StartingConfigPortal(  char * apName, char *apPassword, char * ipAddress   )
{
  // +++ user code here +++
    logger->log( "Starting AP [%s], password [%s]. Server IP [%s].", apName, apPassword, ipAddress );
  // --- user code here ---
}

void wifiStatus_Connected(  int status, int waitTime, char * ipAddress  )
{
  // +++ user code here +++
  logger->log("* wifi [%s], %d dBm, %d s", ipAddress, WiFi.RSSI(), waitTime );
  // --- user code here ---
}

void wifiStatus_NotConnected( int status, long msecNoConnTime )
{
  // +++ user code here +++
    char desc[32];
    getWifiStatusText( status, desc );
    logger->log("* no wifi (%s), %d s", desc, (msecNoConnTime / 1000L) );
  // --- user code here ---
}

void wifiStatus_Starting()
{
  // +++ user code here +++
  // --- user code here ---
}

/**
   Je zavolano pri startu.
   - Pokud vrati TRUE, startuje se config portal.
   - Pokud FALSE, pripojuje se wifi.
   Pokud nejsou v poradku konfiguracni data, otevira se rovnou config portal a tato funkce se nezavola.
*/
bool wifiStatus_ShouldStartConfig()
{
  // +++ user code here +++
    pinMode(CONFIG_BUTTON, INPUT_PULLUP);

    // Pokud se pouziva FLASH button na NodeMCU, je treba zde dat pauzu, 
    // aby ho uzivatel stihl zmacknout (protoze ho nemuze drzet pri resetu),
    // jinak je mozne usporit cas a energii tim, ze se rovnou pojede.
    logger->log( "Sepni pin %d pro config portal", CONFIG_BUTTON );
    delay(3000);

    if ( digitalRead(CONFIG_BUTTON) == CONFIG_BUTTON_START_CONFIG ) {
      logger->log( "Budu spoustet config portal!" );
      return true;
    } else {
      return false;
    }
  // --- user code here ---
}