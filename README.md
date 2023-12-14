# LittleEinsteins

Hello! We are Teresa and Jimena, engineering students, and we're excited to present our project.
Project Overview

Our project centers around the development of a car designed to complete a lap in a circuit in less than 20 seconds. The programming aspect is implemented using Arduino. Below is an image showcasing our car:

![image](https://github.com/jimenade/LittleEinsteins/assets/102520569/9c37c49c-debd-4c01-af90-a26d0875e3e9)

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
