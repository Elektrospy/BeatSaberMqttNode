#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "FastLED.h"

#include <ArduinoOTA.h>

/************ WIFI and MQTT Information (CHANGE THESE FOR YOUR SETUP) ******************/
const char* ssid = "foobar"; //type your WIFI information inside the quotes
const char* password = "";
const char* mqtt_server = "10.42.0.244";
const char* mqtt_username = "";
const char* mqtt_password = "";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

/**************************** FOR OTA **************************************************/
#define NODENAME "left_controller" //change this to whatever you want to call your device
#define OTApassword "ThisIsABadPassword" //the password you will need to enter to upload remotely via the ArduinoIDE
int OTAport = 8266;

/************* MQTT TOPICS (change these topics as you wish)  **************************/
const char* light_set_topic = "beatsaber/saber/a";

/*********************************** FastLED Defintions ********************************/
#define NUM_LEDS    12
#define DATA_PIN    4
//#define CLOCK_PIN 5
#define CHIPSET     WS2812
#define COLOR_ORDER GRB
struct CRGB leds[NUM_LEDS];

#define MAX_BRIGHTNESS 32

uint8_t current_state = 0;
bool dimming_in_progress = false;
uint8_t dimming_counter = 0;

void reconnect();
void setup_wifi();
void update_animation();
void callback(char* topic, byte* payload, unsigned int length);

/********************************** START SETUP*****************************************/
void setup() {
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  //OTA SETUP
  ArduinoOTA.setPort(OTAport);
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(NODENAME);

  // No authentication by default
  ArduinoOTA.setPassword((const char *)OTApassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  FastLED.clear();
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.show();

  Serial.println("Ready.");
  Serial.printf("IP Address: %s \n", WiFi.localIP().toString().c_str());
}


/********************************** START SETUP WIFI*****************************************/
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/********************************** START CALLBACK*****************************************/
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.printf("Message arrived [%s]", topic);

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  current_state = atoi(message);
  dimming_in_progress = false;
  
}

/********************************** UPDATE ANIMATION ***************************************/
void setColor(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }
}

void update_animation()
{
  
  switch (current_state)
  {
    case 0:
      FastLED.clear();
      break;

    case 1:
      setColor(CRGB::Blue);
      break;
    case 2:
      setColor(CRGB::Blue);
      break;

    case 3:
      if(!dimming_in_progress){
        setColor(CRGB::Blue);
        dimming_in_progress = true;
      }
      else{
        fadeToBlackBy(leds, NUM_LEDS, 40);
        dimming_counter++;
        if(dimming_counter > 20){
          dimming_counter = 0;
          dimming_in_progress = false;
          current_state = 0;
        }
      }
      break;

    case 5:
      setColor(CRGB::Red);
      break;
    case 6:
      setColor(CRGB::Red);
      break;

    case 7:
      if(!dimming_in_progress){
        setColor(CRGB::Red);
        dimming_in_progress = true;
      }
      else{
        fadeToBlackBy(leds, NUM_LEDS, 40);
        dimming_counter++;
        if(dimming_counter > 20){
          dimming_counter = 0;
          dimming_in_progress = false;
          current_state = 0;
        }
      }
  
    default:
      break;
  }

  FastLED.show();
}


/********************************** START RECONNECT*****************************************/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(NODENAME, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(light_set_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/********************************** START MAIN LOOP*****************************************/
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print("WIFI Disconnected. Attempting reconnection.");
    setup_wifi();
    return;
  }

  EVERY_N_MILLIS(50)
  {
    update_animation();
  }


  client.loop();

  ArduinoOTA.handle();

}