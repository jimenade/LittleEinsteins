#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>
#include <Thread.h>
#define RXD2 33
#define TXD2 4

#define EAP_ANONYMOUS_IDENTITY "20220719anonymous@urjc.es" // leave as it is
#define EAP_IDENTITY "email@alumnos.urjc.es"    // Use your URJC email
#define EAP_PASSWORD "password"            // User your URJC password
#define EAP_USERNAME "email@alumnos.urjc.es"    // Use your URJC email

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
String buff = "";
int start_time;
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

  messages.publish(jsonString.c_str());
  start_time = millis();

  Serial.println("start_lap enviado");
}

void end_lap(){
  int end_time = millis();
  StaticJsonDocument<200> jsonDoc;
  float result = end_time - start_time;
  // Fill the JSON document with data
  jsonDoc["team_name"] = "LittleEinsteins";
  jsonDoc["id"] = "14";
  jsonDoc["action"] = "END_LAP";
  jsonDoc["time"] = result;
  
  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  messages.publish(jsonString.c_str());
  while(true){
    //Bucle para que no envie mas mensajes
    delay(2000); 
  }
}

void obstacle_detected(int distance){
  
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
  Serial.println("Ping enviado");
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
}

void loop()
{
  if(sendThread.shouldRun())
    sendThread.run();

  if (Serial2.available()) {

    char c = Serial2.read();
    int signal = atoi(&c);
    Serial.println(c);

    buff += c;
    if (signal == 1)  {            
      //obstacle

      char distance = Serial2.read();
      int dist_int = atoi(&distance);

      Serial.println("entra en el if");
      // int distance_int = atoi(&distance);
      obstacle_detected(dist_int);
      end_lap();
      Serial2.println(buff);
      buff = "";
    } 
  }
}