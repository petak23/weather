
/* 
 * Program pre meteorologickú stanicu pomocou ESP8266 a MQTT pre IoT
 *
 * Posledna zmena(last change): 25.05.2021
 * @author Ing. Peter VOJTECH ml. <petak23@gmail.com>
 * @copyright  Copyright (c) 2016 - 2021 Ing. Peter VOJTECH ml.
 * @license
 * @link       http://petak23.echo-msz.eu
 * @version 1.0.3
 */

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
#include "DHT.h"
#include "definitions.h"

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

AsyncWebServer server(80);

// DHT Sensor - GPIO 5 = D1 on ESP-12E NodeMCU board
const int DHTPin = 5;

// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {

  dht.begin();
  
  initLittleFS();

  Serial.begin(115200);

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
  server.begin();                                         // Start server
  Serial.println("HTTP server started");
}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  AsyncElegantOTA.loop();

  now = millis();
  // Publishes new temperature and humidity every 15 seconds
  if (now - lastMeasure > 15000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Computes temperature values in Celsius
    //float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Publishes Temperature and Humidity values
    //uint16_t packetIdPub1 = 
    mqttClient.publish(topic_temperature, 0, true, String(t).c_str());                            
    //Serial.printf("Publishing on topic %s at QoS 0, packetId: %i ", topic_temperature, packetIdPub1);
    //Serial.printf("Teplota: %s°C \n", temperatureTemp);

    //uint16_t packetIdPub2 = 
    mqttClient.publish(topic_humidity, 0, true, String(h).c_str());                            
    //Serial.printf("Publishing on topic %s at QoS 0, packetId: %i ", topic_humidity, packetIdPub2);
    //Serial.printf("Vlhkosť: %s%% \n", humidityTemp);
  }
}