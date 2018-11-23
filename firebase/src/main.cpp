#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

#include "DHT.h"

DHT dht;


//Conexão com firebase
#define FIREBASE_HOST "{db_url}"
#define FIREBASE_AUTH "{database_key}"

//Conexão com a rede Wi-Fi
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASS"

#define LED 14
#define LEDPath "ledStatus"
#define PublishPath "publishNewState"
#define TemperaturePath "temperature"
#define HumidityPath "humidity"

void setup()
{
    dht.setup(4); // data pin 2
   

    // Iniciar Monitor Serial
    Serial.begin(115200);
    delay(200);


    // Iniciar ligação à rede
    Serial.println();
    Serial.println();
    Serial.print("A ligar à rede ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("");
        Serial.println("O seu nodeMCU está ligado!");
        Serial.print("Endereço IP: ");
        Serial.println(WiFi.localIP());
        Serial.println();
        Serial.println();
    }
    else
    {
        Serial.println("");
        Serial.println("Upsss...o seu nodeMCU não se consegue ligar...");
    }

    // Init firebase    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    
    // Setup 
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    Firebase.setInt(LEDPath, LOW);
    Firebase.setInt(PublishPath, true);

    Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F) ");
}

void loop()
{
    // DHT
  if(Firebase.getInt(PublishPath)){
    // Obtem os dados do sensor DHT 
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    
     Serial.print(dht.getStatusString());
     Serial.print("\t");
     Serial.print(humidity, 1);
     Serial.print("\t\t");
     Serial.print(temperature, 1);
     Serial.print("\t\t");
     Serial.print(dht.toFahrenheit(temperature), 1);
     Serial.print("\n");

 
    if(!isnan(humidity) && !isnan(temperature)){
      // Manda para o firebase
      Firebase.pushFloat(TemperaturePath, temperature);
      Firebase.pushFloat(HumidityPath, humidity);    
      //publishNewState = false;
    }else{
      Serial.println("Error Publishing");
    }
  }

    if (Firebase.getInt(LEDPath))
    {
        digitalWrite(LED, LOW);
    }

    else
    {
        digitalWrite(LED, HIGH);
    }

    if (Firebase.failed()) // Check for errors 
    {
        Serial.print("setting /number failed:");
        Serial.println(Firebase.error());
        return;
    }
    delay(1000);

}


// caso de erro https://www.grc.com/fingerprints.htm 