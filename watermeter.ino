/*
   MQTT Binary Sensor - Water Count for Node-Red - NodeMCU (ESP8266)
   https://home-assistant.io/components/binary_sensor.mqtt/
   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient
   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266
    - https://learn.adafruit.com/pir-passive-infrared-proximity-motion-sensor/using-a-pir
   Schematic :
    - https://github.com/mertenats/open-home-automation/blob/master/ha_mqtt_binary_sensor_pir/Schematic.png
    - PIR leg 1 - VCC
    - PIR leg 2 - D0 (or nr.16)
    - PIR leg 3 - GND
    
   Configuration (HA) : 
     binary_sensor:
      platform: mqtt
      state_topic: 'home/motion/water/status'
      name: 'Motion'
      sensor_class: motion
      
   Versions:
   Samuel M. - v1.1 - 08.2016
   TT        - v1.2 - added fixed ip
   TT        - v1.3 - 06.2018 > adapted to value 0 and 1 for watermeter with sensor TCRT5000 

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const PROGMEM char* WIFI_SSID = "Bulldog";
const PROGMEM char* WIFI_PASSWORD = "Theodoor01";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "office_motion";
const PROGMEM char* MQTT_SERVER_IP = "192.168.2.150";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

// MQTT: topic
const PROGMEM char* MQTT_MOTION_STATUS_TOPIC = "home/motion/water/status";

// default payload
//const PROGMEM char* PULSE_ON = "0";
const PROGMEM char* PULSE_OFF = "1";

// PIR : D1/GPIO5
const PROGMEM uint8_t PIR_PIN = 16;
uint8_t m_pir_state = LOW; // no motion detected
uint8_t m_pir_value = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to publish the state of the pir sensor
void publishPirSensorState() {
  if (m_pir_state) {
    client.publish(MQTT_MOTION_STATUS_TOPIC, PULSE_OFF, true);
  //} else {                                                            > Not needed as we only want 1 reply
  //  client.publish(MQTT_MOTION_STATUS_TOPIC, PULSE_ON, true);
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  IPAddress ip(192,168,2,204);   
  IPAddress gateway(192,168,2,254);   
  IPAddress subnet(255,255,255,0);   
  WiFi.config(ip, gateway, subnet);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // read sensor for serial monitor
  m_pir_value = digitalRead(PIR_PIN);
  if (m_pir_value == HIGH) {
   if (m_pir_state == LOW) {
    // a motion is detected
     Serial.println("No Motion");
     publishPirSensorState();
     m_pir_state = HIGH;
   }
 
    
 } else {
  if (m_pir_state == HIGH) {
    publishPirSensorState();
     Serial.println("Motion Detected");
    m_pir_state = LOW;
    }
     delay(1000); >small delay for when te sensor stops at metal
 }
}
