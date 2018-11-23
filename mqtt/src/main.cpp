#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <String.h>
#include "DHT.h"

DHT dht;

#define wifi_ssid "SSID"
#define wifi_password "SSID"

#define mqtt_server "iot.eclipse.org"
#define mqtt_user ""
#define mqtt_password ""

#define clientId "{seu_nome}"
#define humidity_topic "{seu_nome}/sensor/humidity"
#define temperature_topic "{seu_nome}/sensor/temperature"
#define led1_topic "{seu_nome}/led1"
#define led1_pin D2
#define luminosidade_topic "{seu_nome}/sensor/luminosidade"

WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi();
ESP8266WebServer server(80);


void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  String message;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));

  if(String(topic) == led1_topic)
  {
    if(String(message) == "Liga")
      digitalWrite(led1_pin, HIGH);
    else
      digitalWrite(led1_pin, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  dht.setup(D5); // data pin D5  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);
  pinMode(led1_pin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");    
    if (client.connect(clientId)) {
      Serial.println("connected");
      client.subscribe(led1_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 1.0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.getTemperature();
    float newHum = dht.getHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
    }
    int sensorValue = analogRead(A0);   // read the input on analog pin 0
    float luminosidade = map(sensorValue, 0, 1023, 100, 0);
    client.publish(luminosidade_topic, String(luminosidade).c_str(), true);
  }
}
