# BeatSaberMqttNode

Esp8266 mqtt end node, for reacting to the mqtt messages created by the [BeatSaberMqttProxy
](https://github.com/Elektrospy/BeatSaberMqttProxy).
The esp controls a led strip with ws2812b led's, based on the mqtt topic your'e node is subscribed to and the payload received.

## Hardware used:
- ESP8266
  - Wemos D1 mini
- WS2812B RGB Led Strip
  - Signal is connected to the data pin 4 (printed as D2 on the Wemos D1 mini)
- Levelshifter or Voltage Divider for Logic Level
  - Needed for longer wire connections
  - The ESP is 3.3V and die LED's are 5V Logic
  
## Required Libraries:
- FastLED
  - https://github.com/FastLED/FastLED
- PubSubClient
  - https://github.com/knolleary/pubsubclient
- ESP8266WiFi & ArduinoOTA
  - Should be included if you installed the Arduino Framework for ESP's
  - https://github.com/esp8266/Arduino
