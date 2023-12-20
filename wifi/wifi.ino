#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>
#include <Thread.h>
#define RXD2 33
#define TXD2 4

#define EAP_ANONYMOUS_IDENTITY "20220719anonymous@urjc.es" // leave as it is
#define EAP_IDENTITY "j.delatorre.2020@alumnos.urjc.es"    // Use your URJC email
#define EAP_PASSWORD "Pompas y yo 2002"            // User your URJC password
#define EAP_USERNAME "j.delatorre.2020@alumnos.urjc.es"    // Use your URJC email

//SSID NAME
const char* ssid = "eduroam"; // eduroam SSID


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
String buff;
int start_time;
// Thread myThread = Thread();
Thread sendThread = Thread();
// int ping_time;

void setupWifi()
{
  delay(10);
  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true); 

  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP()); //print LAN IP
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
  // buff = '-1';
  // Serial2.println(buff);
  start_time = millis();

  Serial.println("start_lap enviado");
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
  //Serial.println("entra en ping");
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
  Serial.println("Ping enviado");

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
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  setupWifi();

  connectToMQTTServer();
  
  start_time = 0;
  start_lap();
  Serial2.print("{ 'test': " + String(millis()) + " }");
  Serial.print("Messase sent! to Arduino");
  buff = '-1';

  Serial2.println(buff);
  buff = "";
  sendThread.enabled = true;
  sendThread.setInterval(4000);
  sendThread.onRun(send_ping);
  // ping_time = start_time;
  
}

void loop()
{
  if(sendThread.shouldRun())
    sendThread.run();

  if (Serial2.available()) {

    char c = Serial2.read();
    Serial.println(c);

    buff += c;
    if (c == '1')  {            
      //obstacle
      Serial.println("entra en el if");
      obstacle_detected(5);
      end_lap();
      Serial2.println(buff);
      buff = "";
      exit(0);
      
      
    } 
    // if (c == '3'){
      
    // }


  }

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
  yield();
}