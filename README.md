# LittleEinsteins

Hello! We are Teresa and Jimena, engineering students, and we're excited to present our project.
Project Overview

Our project centers around the development of a car designed to complete a lap in a circuit in less than 20 seconds. The programming aspect is implemented using Arduino. Below is an image showcasing our car:

![image](https://github.com/jimenade/LittleEinsteins/assets/102520569/9c37c49c-debd-4c01-af90-a26d0875e3e9)
## Moving the car:

To accomplish this aspect of the project, we implemented a Proportional-Integral-Derivative (PID) controller using Arduino. The PID controller is responsible for orchestrating the reading of infrared sensors, interpreting data from the ultrasound sensor to determine distance, and managing the transmission of messages corresponding to each action. This approach ensures precise control over the robot's movements and responses.

Specifically, the PID controller oversees the following functionalities:

- Infrared Sensor Reading:
  
  The PID controller tasks the reading of infrared sensors, which play a crucial role in line following and detection. By continuously monitoring the input from these sensors, the controller makes real-time decisions to keep the robot aligned with the desired path.

- Ultrasound Sensor for Distance Measurement:
  
  The ultrasound sensor is utilized to measure the distance between the robot and surrounding obstacles. The PID controller incorporates this distance data into its decision-making process, enabling the robot to navigate effectively and avoid collisions.
  
- Message Transmission:

  The PID controller is responsible for sending messages corresponding to each action undertaken by the robot. These messages include critical information about the robot's state and actions, such as starting and ending a lap, detecting obstacles, and other relevant events.

- Line Tracking Management:
  
  Notably, our robot is designed to operate without losing the tracking line. While we do not have a recovery plan for scenarios involving line loss, the PID controller is adept at minimizing the chances of such occurrences. Additionally, the controller handles messages related to the robot's line-tracking status, including detecting the line, losing the line, and reporting the percentage of the line visible.

By incorporating the PID controller into our project, we ensure a robust and adaptive control system that enhances the precision and reliability of our robot's navigation and interaction with its environment. This comprehensive approach contributes to the overall success and efficiency of our project.

## ESP-32:
We employ the ESP-32 microcontroller to establish a Wi-Fi connection between the robot and a server, facilitating MQTT communication. The network utilized for this connection is an educational network associated with one of our designated accounts. Below is an illustrative example of the code structure, with placeholder data for confidentiality:
```cpp
#include <WiFi.h>

#define EAP_ANONYMOUS_IDENTITY "20220719anonymous@urjc.es" // leave as it is
#define EAP_IDENTITY "name@alumnos.urjc.es"    // Use your URJC email
#define EAP_PASSWORD "password"            // User your URJC password
#define EAP_USERNAME "name@alumnos.urjc.es"    // Use your URJC email

//SSID NAME
const char* ssid = "eduroam"; // eduroam SSID

WiFiClient client;

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

```

## MQTT Communication:

To communicate with the MQTT server, we utilize topics, and our designated topic is /SETR/2023/14/. Messages are sent in JSON format, and here's an example message for starting the lap:

```cpp
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
```
Mandatory Messages

We need to send several mandatory messages, including:

    Start lap
    End lap
    Obstacle detected
    Line lost
    Ping (every 4 seconds)

We can only start sending messages when connected to the WiFi and the server. Once connected, we initiate the lap by sending the start lap message. The end of the lap is determined by detecting an obstacle. Accordingly, we first send the obstacle detected message, followed by the end lap message containing the lap time. Additionally, a ping message is sent every 4 seconds to maintain communication.

Feel free to explore our project and its code for a deeper understanding!
