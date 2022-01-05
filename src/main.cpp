#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <AsyncMqttClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "ra_blinker.h"
#include "pv_simpletime.h"
#include "DHT.h"
#include "Tasker.h"
#include "definitions.h"

/** 
 * Program pre meteorologickú stanicu pomocou ESP8266 a MQTT pre IoT
 *
 * Posledná zmena(last change): 05.01.2022
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2022 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.2.0
 */

AsyncMqttClient mqttClient;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

int mqtt_state = 0;          // Stav MQTT pripojenia podľa https://pubsubclient.knolleary.net/api#state
float humidity = 0;
float temperature = 0;

boolean timeNTPwait = false;
boolean firstRun = true;

AsyncWebServer server(80);

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

// Inicialize Tasker
Tasker tasker;

#ifdef USE_BLINKER
  raBlinker blinker( BLINKER_PIN );
  int blinkerPublishMQTT[] = { 50, 250, 200, 250, -1 };
#endif

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
pvSimpletime pvst;

// Initialize LittleFS
void initLittleFS() {
  
  #if SERIAL_PORT_ENABLED
    if (!LittleFS.begin()) {
      Serial.println("An error has occurred while mounting LittleFS");
    }
    Serial.println("LittleFS mounted successfully");
  #else
    LittleFS.begin();
  #endif
}

// Create a WebSocket object
AsyncWebSocket webs("/ws");

String getOutputStates(){
  JSONVar myArray;
  myArray["mqtt"] = String(mqtt_state);
  
  static char tTemp[7];
  dtostrf(temperature, 6, 2, tTemp);
  static char hTemp[7];
  dtostrf(humidity, 6, 2, hTemp);                           

  pvst.setTime(timeClient.getEpochTime());
  myArray["humidity"] = hTemp;
  myArray["temperature"] = tTemp;
  myArray["out_time"] = "Poslené meranie: " + pvst.getFormDT();
  myArray["logbook"] = "Poslené meranie: " + pvst.getFormDT() + " T: " + String(tTemp) + "°C; V: " + String(hTemp) + "%";

  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  webs.textAll(state);
}

void log(String logMessage) {
  pvst.setTime(timeClient.getEpochTime());
  String tmp = pvst.getFormDT() + " -> " + logMessage;
  #if SERIAL_PORT_ENABLED
    Serial.println(tmp);
  #else
    JSONVar myArray;
    myArray["logbook"] = tmp;
    webs.textAll(JSON.stringify(myArray));
  #endif
}

void connectToWifi() {
  #if SERIAL_PORT_ENABLED
    Serial.println("Connecting to Wi-Fi...");
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  log("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  #if SERIAL_PORT_ENABLED
    Serial.println("Connected to Wi-Fi.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  #endif
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  #if SERIAL_PORT_ENABLED
    Serial.println("Disconnected from Wi-Fi.");
  #endif
  tasker.setTimeout(connectToWifi, 2000); // Opakovaný pokus o pripojenie za 2s
  tasker.clearTimeout(connectToMqtt); // Ak nemám wifi, tak sa nepripájam ani na MQTT
}

void onMqttConnect(bool sessionPresent) {
  mqtt_state = 1;                   // Nastav príznak MQTT spojenia
  notifyClients(getOutputStates()); // Aktualizuj stavy webu
  log("Connected to MQTT.");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  log("Disconnected from MQTT.");
  mqtt_state = 0;                   // Nastav príznak chýbajúceho MQTT spojenia
  notifyClients(getOutputStates()); // Aktualizuj stavy webu
  if (WiFi.isConnected()) {
    tasker.setTimeout(connectToMqtt, 5000); // Opakovaný pokus o pripojenie MQTT o 5s
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      log("WebSocket client #" + String(client->id()) + " connected from: " + client->remoteIP().toString().c_str());
      notifyClients(getOutputStates());       // Updatuj web
      break;
    case WS_EVT_DISCONNECT:
      #if SERIAL_PORT_ENABLED
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
      #endif
      break;
    case WS_EVT_DATA:
      //handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  webs.onEvent(onEvent);
  server.addHandler(&webs);
}

void readDHT(boolean publishmqtt = true) {
  //Nacitaj udaje z DHT
  humidity = dht.readHumidity();    // Načítanie vlhkosti
  temperature = dht.readTemperature(); // Načítanie teploty v °C

  if (isnan(humidity) || isnan(temperature)) { // Kontrola načítania dát zo senzora
    log("Chyba načítania údajov z DHT senzora.");
    return;
  }
  notifyClients(getOutputStates());       // Updatuj web
}

void taskReadDHT() {
  readDHT();

  // Publikácia načítaných hodnôt 
  mqttClient.publish(topic_temperature, 0, true, String(temperature).c_str());
  mqttClient.publish(topic_humidity, 0, true, String(humidity).c_str()); 
  blinker.setCode( blinkerPublishMQTT );
}

void firstPublish() {
  log("Prvé plánované meranie");
  taskReadDHT();
  // Publikovanie nových hodnôt od teraz každých PUBLISH_TIME min.
  tasker.setInterval(taskReadDHT, (PUBLISH_TIME * 60000));
}

void readNTP() {
  time_t timeNTP = timeClient.getEpochTime();
  if (timeNTP > 10000) {
    log("Úspešné načítanie serverového času. Aktuálny: " + String(timeNTP));
    int put = PUBLISH_TIME;
    int min = minute(timeNTP);
    int sec = 60 - second(timeNTP);                   // Sekundy do celej minúty
    int min_to_publish = put - (min % put) - 1;       // Celé minúty do najbližšej publikácie
    long sec_to_publish = min_to_publish * 60 + sec;  // Sekundy do najbližšej publikácie
    log("put="+String(put)+" | min="+String(min)+" | sec="+String(sec)+" | min_to_publish="+String(min_to_publish));
    firstRun = false;
    tasker.setTimeout(firstPublish, (sec_to_publish * 1000));
    log("Nastavenie najbližšieho merania o "+ String(sec_to_publish) + "s");
  } else {
    log("Čakám na serverový čas..." + String(timeNTP));
    tasker.setTimeout(readNTP, 2000); // Čakanie na serverový čas
  }
}

void firstRunFunction() {
  if (!timeNTPwait) {
    timeNTPwait = true;
    tasker.setTimeout(readNTP, 2000); // Čakanie na serverový čas
  } 
}

void setup() {

  dht.begin();
  
  initLittleFS();
  initWebSocket();

  #if SERIAL_PORT_ENABLED
    Serial.begin(115200);
  #endif

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);

  connectToWifi();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });

  server.serveStatic("/", LittleFS, "/");

  AsyncElegantOTA.begin(&server, OTA_USER, OTA_PASSWORD); // Štart ElegantOTA s autentifikáciou https://github.com/ayushsharma82/AsyncElegantOTA
  server.begin();  // Start server
  
  timeClient.begin();                                       

  log("HTTP server started");

  readDHT(false);       // Updatuj web
}

void loop() {

  webs.cleanupClients();

  tasker.loop();

  timeClient.update();

  if (firstRun) {
    firstRunFunction();
  }
}