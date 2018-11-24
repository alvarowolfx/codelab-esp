#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <String.h>
#include "DHT.h"


#define led1_pin D2
#define button_pin D3

DHT dht;

long lastMsg = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(led1_pin, OUTPUT);  
  dht.setup(D5); // data pin D5
}

void loop() {    
  long now = millis();
  button.tick();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float temp = dht.getTemperature();
    float hum = dht.getHumidity();
    
    Serial.print("New temperature:");
    Serial.println(String(temp).c_str());    
      
    Serial.print("New humidity:");
    Serial.println(String(hum).c_str());        
  }  
}
