# Projekt esp8266-weather

Meteorologická stanica pomocou esp8266 a MQTT

## Špecifikácia

Informácia o teplote a vlhkosti je publikovaná každých 60s.

**mqtt client name** = "ESP8266-weather"

**topic** = "weather/temperature"  
**message** = "\<temperature\>"

**topic** = "weather/humidity"  
**message** = "\<humidity\>"

## Odkazy na použité knižnice

- **Async MQTT client for ESP8266 and ESP32**: [https://github.com/marvinroger/async-mqtt-client](https://github.com/marvinroger/async-mqtt-client)
- **ESPAsyncTCP**: [https://github.com/me-no-dev/ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
- **DHT-sensor-library**: [https://github.com/adafruit/DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)
- **Adafruit Unified Sensor**: [https://github.com/adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)
- **AsyncElegantOTA**: [https://github.com/ayushsharma82/AsyncElegantOTA](https://github.com/ayushsharma82/AsyncElegantOTA)
- **AsyncTCP**: [https://github.com/me-no-dev/AsyncTCP](https://github.com/me-no-dev/AsyncTCP)
- **ESPAsyncWebServer**: [https://github.com/me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

## Zdroje

- [ESP32 MQTT - Publish BME280 Sensor Readings (Arduino IDE)](https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/)
- [ESP8266 NodeMCU OTA (Over-the-Air) Updates – AsyncElegantOTA (VS Code + PlatformIO)](https://randomnerdtutorials.com/esp8266-nodemcu-ota-over-the-air-vs-code/)
- [RatatoskrIoT](https://github.com/petrbrouzda/RatatoskrIoT) alebo môj fork [petak23_RatatoskrIoT](https://github.com/petak23/RatatoskrIoT)
