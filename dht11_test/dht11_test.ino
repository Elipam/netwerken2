#include <DHT.h>
#include <ESP8266WiFi.h>
#include "secret.h"

// Pin configuratie
#define DHTPIN D1 // DHT11 data pin
#define DHTTYPE DHT11

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void connectWiFi()
{
  Serial.print("Verbinden met Wi-Fi: ");
  WiFi.begin("CodeCast", "CoCaWiFi!");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n🟢 Verbonden met Wi-Fi");
}

void setup()
{
  // Start seriële communicatie
  Serial.begin(115200);
  Serial.println("DHT11 Testcode met Wi-Fi");

  // Verbind met Wi-Fi
  connectWiFi();

  // Start DHT sensor
  dht.begin();
}

void loop()
{
  // Lees temperatuur en luchtvochtigheid
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Controleer op fouten
  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("❌ Fout bij het lezen van de sensor");
  }
  else
  {
    Serial.print("🌡️ Temperatuur: ");
    Serial.print(temperature);
    Serial.println("°C");

    Serial.print("💧 Luchtvochtigheid: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  // Wacht 2 seconden voor de volgende meting
  delay(2000);
}
