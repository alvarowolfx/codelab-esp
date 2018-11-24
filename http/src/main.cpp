#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <String.h>
#include "DHT.h"

#define wifi_ssid "SSID"
#define wifi_password "PASS"

#define softap_ssid "seunome"
#define softap_pass "suasenha"

#define led1_pin D2
#define led2_pin D4
DHT dht;

ESP8266WebServer server(80);
float humidity;
float temperature;
int sensorValue;

void handleRoot() {
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  sensorValue = analogRead(A0);   // read the input on analog pin 0
  float luminosidade = map(sensorValue, 0, 1023, 100, 0);
  
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.print(temperature, 1);
  Serial.print("\t\t");
  Serial.println(dht.toFahrenheit(temperature), 1);

  String esp = "<!DOCTYPE html>";
  esp += "<html>";
  esp += "  <head>";
  esp += "    <meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\">";
  esp += "    <meta charset=\"UTF-8\">";
  esp += "    <title>Web Server ESP8266</title>";
  esp += "  </head>";
  esp += "  <body>";
  esp += "    <h1>Você está conectado!</h1>";
  esp += "    <br></br>";
  esp += "    Temperatura: " + String(temperature) + " Celsius";
  esp += "    <br></br>";
  esp += "    Humidade: " + String(humidity);
  esp += "    <br></br>";
  esp += "    Luminosidade: " + String(luminosidade);
  esp += "    <br></br>";
  esp += "<table style=\"width:50%\">";
  esp += "  <tr>";
  esp += "    <th><h3>LED 1</h3></th>";
  esp += "    <th><h3>LED 2</h3></th>";
  esp += "  </tr>";
  esp += "  <tr>";
  esp += "    <th><a href=\"LED1On\"><button>ON</button></a></th>";
  esp += "    <th><a href=\"LED2On\"><button>ON</button></a></th>";
  esp += "  </tr>";
  esp += "  <tr>";
  esp += "    <th><a href=\"LED1Off\"><button>OFF</button></a></th>";
  esp += "    <th><a href=\"LED2Off\"><button>OFF</button></a></th>";
  esp += "  </tr>";
  esp += "</table>";
  esp += "  </body>";
  esp += "</html>";
  
  server.send(200, "text/html", esp);
}
void handleLed1On() {
  digitalWrite(led1_pin, HIGH);
  handleRoot();
}

void handleLed1Off() {
  digitalWrite(led1_pin, LOW);
  handleRoot();
}

void handleLed2On() {
  digitalWrite(led2_pin, HIGH);
  handleRoot();
}

void handleLed2Off() {
  digitalWrite(led2_pin, LOW);
  handleRoot();
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

void setup_softap(){   
  WiFi.softAP(softap_ssid, softap_pass);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
}

void setup() {
  Serial.begin(115200);
  dht.setup(D5); // data pin D5  
  
  // setup_wifi();
  setup_softap();

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);

  server.on("/", handleRoot);
  server.on("/LED1On", handleLed1On);
  server.on("/LED1Off", handleLed1Off);
  server.on("/LED2On", handleLed2On);
  server.on("/LED2Off", handleLed2Off);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
