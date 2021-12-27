#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <SimpleTime.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "ra_blinker.h"
#include "DHT.h"
#include "Tasker.h"
#include "definitions.h"

/* 
 * Program pre meteorologickú stanicu pomocou ESP8266 a MQTT pre IoT
 *
 * Posledná zmena(last change): 27.12.2021
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.1.6
 */

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

int mqtt_state = 0;          // Stav MQTT pripojenia podľa https://pubsubclient.knolleary.net/api#state
float humidity = 0;
float temperature = 0;

AsyncWebServer server(80);

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

Tasker tasker;

#ifdef USE_BLINKER
  raBlinker blinker( BLINKER_PIN );
  /*int blinkerPortal[] = BLINKER_MODE_PORTAL;
  int blinkerSearching[]  = BLINKER_MODE_SEARCHING;
  int blinkerRunning[] = BLINKER_MODE_RUNNING;
  int blinkerRunningWifi[] = BLINKER_MODE_RUNNING_WIFI;*/
  int blinkerPublishMQTT[] = { 50, 250, 200, 250, -1 };
#endif

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
time_t utcCalc = 0;

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
  
  static char temperatureTemp[7];
  dtostrf(temperature, 6, 2, temperatureTemp);
  static char humidityTemp[7];
  dtostrf(humidity, 6, 2, humidityTemp);                           

  utcCalc = timeClient.getEpochTime();
  myArray["humidity"] = humidityTemp;
  myArray["temperature"] = temperatureTemp;
  myArray["out_time"] = "Poslené meranie: "+String(day(utcCalc))+"."+String(month(utcCalc))+"."+String(year(utcCalc))+" "+String(hour(utcCalc))+":"+String(minute(utcCalc))+":"+String(second(utcCalc));
  myArray["logbook"] = " EpochTime: "+String(timeClient.getEpochTime());

  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  webs.textAll(state);
}

void log(String logMessage) {
  JSONVar myArray;
  utcCalc = timeClient.getEpochTime();
  myArray["logbook"] = String(timeClient.getEpochTime())+" -> "+logMessage;
  webs.textAll(JSON.stringify(myArray));
}

void connectToWifi() {
  #if SERIAL_PORT_ENABLED
    Serial.println("Connecting to Wi-Fi...");
  #endif
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  #if SERIAL_PORT_ENABLED
    Serial.println("Connecting to MQTT...");
  #endif
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
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  mqtt_state = 1;                   // Nastav príznak MQTT spojenia
  notifyClients(getOutputStates()); // Aktualizuj stavy webu
  log("Connected to MQTT.");
  #if SERIAL_PORT_ENABLED
    Serial.println("Connected to MQTT.");
    Serial.print("Session present: ");
    Serial.println(sessionPresent);
  #endif
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  #if SERIAL_PORT_ENABLED
    Serial.println("Disconnected from MQTT.");
  #endif
  log("Disconnected from MQTT.");
  mqtt_state = 0;                   // Nastav príznak chýbajúceho MQTT spojenia
  notifyClients(getOutputStates()); // Aktualizuj stavy webu
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      #if SERIAL_PORT_ENABLED
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      #endif
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

void taskReadDHT() {
  //Nacitaj udaje z DHT
  humidity = dht.readHumidity();    // Načítanie vlhkosti
  temperature = dht.readTemperature(); // Načítanie teploty v °C
  utcCalc = timeClient.getEpochTime();

  #if SERIAL_PORT_ENABLED
  // Výstup na sériový port
    static char temperatureTemp[7];
    dtostrf(temperature, 6, 2, temperatureTemp);
    static char humidityTemp[7];
    dtostrf(humidity, 6, 2, humidityTemp);
    Serial.print(timeClient.getFormattedTime());
    Serial.printf(" DAY: %u (%lu)[%u.%u.%u]", timeClient.getDay(), timeClient.getEpochTime(),  day(utcCalc), month(utcCalc), year(utcCalc));
    Serial.printf(" Teplota: %s°C | Vlhkosť: %s%% \n", temperatureTemp, humidityTemp);
    
  #endif

  if (isnan(humidity) || isnan(temperature)) { // Kontrola načítania dát zo senzora
    #if SERIAL_PORT_ENABLED
      Serial.println("Failed to read from DHT sensor!");
    #endif
    log("Chyba načítania údajov z DHT senzora.");
    return;
  }

  // Publikácia načítaných hodnôt 
  mqttClient.publish(topic_temperature, 0, true, String(temperature).c_str());
  mqttClient.publish(topic_humidity, 0, true, String(humidity).c_str()); 
  blinker.setCode( blinkerPublishMQTT );
  notifyClients(getOutputStates());       // Updatuj web
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

  #if SERIAL_PORT_ENABLED
    Serial.println("HTTP server started");
  #endif

  // Publikovanie nových hodnôt sa deje každých PUBLISH_TIME/1000 sec.
  tasker.setInterval(taskReadDHT, PUBLISH_TIME);

  taskReadDHT();       // Updatuj web
}

void loop() {

  webs.cleanupClients();

  tasker.loop();

  timeClient.update();

}