# Projekt esp8266-weadher

Meteorologická stanica pomocou esp8266 a MQTT 

## Špecifikácia 

pre verziu 1.0:

Informácia o teplote a vlhkosti je publikovaná každých 15s.

**mqtt client name** = "ESP8266-weadher"

**topic** = "room/temperature"   
**message** = "\<temperature\>"

**topic** = "room/humidity"    
**message** = "\<humidity\>"