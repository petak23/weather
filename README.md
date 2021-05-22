# Projekt esp8266-weadher

Meteorologická stanica pomocou esp8266 a MQTT

## Špecifikácia

Informácia o teplote a vlhkosti je publikovaná každých 15s.

**mqtt client name** = "ESP8266-weadher"

**topic** = "room/temperature"  
**message** = "\<temperature\>"

**topic** = "room/humidity"  
**message** = "\<humidity\>"

## Odkazy na použité knižnice

- **Async MQTT client for ESP8266 and ESP32**: [https://github.com/marvinroger/async-mqtt-client](https://github.com/marvinroger/async-mqtt-client)
- **ESPAsyncTCP**: [https://github.com/me-no-dev/ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP)
- **DHT-sensor-library**: [https://github.com/adafruit/DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)
- **Adafruit Unified Sensor**: [https://github.com/adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor)

## Zdroje

- [ESP32 MQTT - Publish BME280 Sensor Readings (Arduino IDE)](https://randomnerdtutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/)
