# Popis nastavení v definitions.h

## Nastavenia Wifi

```c

#define WIFI_SSID "my-ssid-name"
#define WIFI_PASSWORD "my-extra-strong-password"    
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
const char* topic_temperature = "weather/temperature"; 
const char* topic_humidity = "weather/humidity"; 
```

## Senzory

```c
#define DHTTYPE DHT22   // Definovanie typov pripojených senzorov
#define DHTPIN 5        // Pin, na ktorý je pripojený senzor
```

## Ostaté konštanty

```c
#define PUBLISH_TIME 15000 // Perioda publikácie nameraných dát. (15000/1000)sec.
#define SERIAL_PORT_ENABLED true  // Povolenie výstupu na sériový port - logovanie
```
