/** 
 * Vzorový súbor a popis nastavení v súbore definitions.h 
 * Pre reálnu aplikáciu je potrebné premenovať na: deffinitions.h
 * 
 * Posledná zmena(Last change): 08.12.2021
 * 
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.0.0
 */

/** Nastavenia Wifi */
#define WIFI_SSID "my-ssid-name"    
#define WIFI_PASSWORD "my-extra-strong-password"    
#define WIFI_HOST "my-host-name"   

/** ------ Nastavenia MQTT ------- */
// Nastavenia broker-a
#define MQTT_ENBLED true    // Celkové povolenie MQTT
#define MQTT_HOST IPAddress(xxx, xxx, xxx, xxx)
#define MQTT_PORT 1883
#define MQTT_USER "my-mqtt-broker-name"
#define MQTT_PASSWORD "my-mqtt-broker-password"

// Nastavenie topic
const char* topic_temperature = "weather/temperature"; 
const char* topic_humidity = "weather/humidity"; 
/* ------- ------- */

/** Prihlasovanie do OTA(AsyncElegantOTA) pre update firmware */
#define OTA_USER "ota-username";
#define OTA_PASSWORD "ota-password";

/** Nastavenie a definície senzorov */
#define DHTTYPE DHT22   // Definovanie typov pripojených senzorov
#define DHTPIN 5        // Pin, na ktorý je pripojený senzor

/** Ostaté Nastavenia */
#define PUBLISH_TIME 15000        // Perioda publikácie nameraných dát. Napr.(15000/1000) = 15 sec.
#define SERIAL_PORT_ENABLED true  // Povolenie výstupu na sériový port - logovanie