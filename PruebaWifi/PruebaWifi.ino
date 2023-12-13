#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>
#include <Thread.h>

/*
  Replace the SSID and Password according to your wifi
*/
const char *ssid = "wifieif";
const char *password = "Goox0sie_WZCGGh25680000";

// Your MQTT broker ID
const char *mqttBroker = "193.147.53.2";
const int mqttPort = 21883;

// MQTT topics
const char *publishTopic = "/SETR/2023/14/";

WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, mqttBroker, mqttPort);

// Setup a publisher
Adafruit_MQTT_Publish messages = Adafruit_MQTT_Publish(&mqtt, publishTopic, MQTT_QOS_1);

unsigned long lastMsg = 0;
const int READ_CYCLE_TIME = 3000;

int start_time;
Thread sendThread = Thread();
// int ping_time;

void setupWifi()
{
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void connectToMQTTServer()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    mqtt.disconnect();
    delay(10000); // wait 10 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void start_lap(){
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "START_LAP";

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Publish the JSON payload to an MQTT topic
  // client.publish(TOPIC, jsonString.c_str());
  

  messages.publish(jsonString.c_str());

  start_time = millis();
}

void end_lap(){
  int end_time = millis();
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "END_LAP";
  jsonDoc["time"] = end_time - start_time;


  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void obstacle_detected(float distance){
  
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "OBSTACLE_DETECTED";
  jsonDoc["distance"] = distance;


  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void line_lost(){  
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "LINE_LOST";
  // jsonDoc["distance"] = distance;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());

}

void send_ping(){
  int ping_time = millis();
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  int time = ping_time - start_time;
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "PING";
  jsonDoc["time"] = time;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());

}

void init_line_search(){
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "INIT_LINE_SEARCH";
  // jsonDoc["distance"] = distance;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void stop_line_search(){
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "STOP_LINE_SEARCH";
  // jsonDoc["distance"] = distance;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void line_found(){
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "LINE_FOUND";
  // jsonDoc["distance"] = distance;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void visible_line(float value){
  StaticJsonDocument<200> jsonDoc;

  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "VISIBE_LINE";
  jsonDoc["value"] = value;

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
}

void setup()
{
  // Set Serial Monitor Baud rate
  Serial.begin(115200);
  // Configure LED
  // pinMode(LED_BUILTIN, OUTPUT);
  // Connect to Wifi
  setupWifi();
  connectToMQTTServer();
  start_time = 0;
  sendThread.enabled = true;
  sendThread.setInterval(4000);
  sendThread.onRun(send_ping);
  start_lap();
  // ping_time = start_time;
  
}

void loop()
{
  // Connect to MQTT
  //connectToMQTTServer();

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  // if (!mqtt.ping())
  // {
  //   mqtt.disconnect();
  // }
}