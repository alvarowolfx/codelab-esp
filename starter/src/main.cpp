#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <String.h>
#include "DHT.h"

#define led1_pin D2
#define led2_pin D4

DHT dht;

long lastMsg = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(led1_pin, OUTPUT);  
  pinMode(led2_pin, OUTPUT);  
  dht.setup(D5); // data pin D5
}

void loop() {    
  long now = millis();  
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float temp = dht.getTemperature();
    float hum = dht.getHumidity();
    
    Serial.print("New temperature:");
    Serial.println(String(temp).c_str());    
      
    Serial.print("New humidity:");
    Serial.println(String(hum).c_str()); 

    digitalWrite(led1_pin, ledState ? HIGH : LOW );
    digitalWrite(led2_pin, ledState ? LOW : HIGH );
    ledState = !ledState;       
  }  
}
