# Popis nastavení v definitions.h

## Nastavenia Wifi

```c

#define WIFI_SSID "my-ssid-name"
#define WIFI_PASSWORD "my-extra-stronf-password"    
#define WIFI_HOST "my-host-name"   
```

## Nastavenia MQTT broker-a

```c
#define MQTT_HOST IPAddress(xxx, xxx, xxx, xxx)
#define MQTT_PORT 1883
#define MQTT_USER "my-mqtt-broker-name"
#define MQTT_PASSWORD "my-mqtt-broker-password"

```

## Prihlasovanie do OTA pre update firmware

```c
// AsyncElegantOTA prihlasovanie
#define OTA_USER "ota-username";
#define OTA_PASSWORD "ota-password";
```

## Topic

```c
const char* main_topic = "main/topic/";
```
