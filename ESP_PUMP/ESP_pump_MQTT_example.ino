#include "ESP8266WiFi.h"
#include <PubSubClient.h>

const char* ssid = "ssid";
const char* password = "password";

const char* mqttServer = "192.168.0.n";
const int mqttPort = 1883;

int relayPin = 2;
int current_state = 0;

WiFiClient espClient;
PubSubClient client(espClient);

IPAddress ip(192, 168, 0, 120);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void setup()
{
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    if (client.connect("pump")) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("garden/pump/log", "Garden pump reporting for duty");
  client.subscribe("garden/pump/status");
  client.subscribe("garden/pump/toggle");
  client.subscribe("garden/pump/on");
  client.subscribe("garden/pump/off");
 
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "garden/pump/on")==0){
    Serial.print("Toggle pump on");
    client.publish("garden/pump/log", "Garden pump ON");
    digitalWrite(relayPin, LOW);
  }else if(strcmp(topic, "garden/pump/off")==0){
    Serial.print("Toggle pump off");
    client.publish("garden/pump/log", "Garden pump OFF");
    digitalWrite(relayPin, HIGH);
  }else {
    current_state = digitalRead(relayPin);
    if(strcmp(topic, "garden/pump/status")==0){
      Serial.print("Current state:");
      Serial.print(current_state);
      if (current_state == 1){
        client.publish("garden/pump/log", "Pump off");
      }else{
        client.publish("garden/pump/log", "Pump on");
      }
    }else if(strcmp(topic, "garden/pump/toggle")==0){
      if (current_state == 1){
        client.publish("garden/pump/log", "Garden pump ON");
        digitalWrite(relayPin, LOW);
      }else{
        client.publish("garden/pump/log", "Garden pump OFF");
        digitalWrite(relayPin, HIGH);
      }
    }else{
      Serial.print("nothing found");
      Serial.print(topic);
    }
  }  
}


void loop() {
  client.loop();
}
