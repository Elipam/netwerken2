#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "secret.h"

// Pin configuratie
#define DHTPIN D1 // DHT11 data pin
#define DHTTYPE DHT11
#define LED_BUILTIN_PIN LED_BUILTIN

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

// WiFi en MQTT clients
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Functie om verbinding te maken met Wi-Fi
void connectWiFi()
{
  Serial.print("Verbinden met Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n🟢 Verbonden met Wi-Fi");
}

// Functie om verbinding te maken met de MQTT broker
void connectMQTT()
{
  // Gebruik BearSSL om het certificaat in te stellen
  BearSSL::X509List cert(local_root_ca);
  espClient.setTrustAnchors(&cert); // Stel het certificaat in

  client.setServer(MQTT_HOST, MQTT_PORT);
  while (!client.connected())
  {
    Serial.print("Verbinden met MQTT broker...");
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS))
    {
      Serial.println("🟢 Verbonden met MQTT broker");
      client.subscribe("chat/message"); // Abonneer op het juiste chat topic
      Serial.println("📩 Geabonneerd op topic: chat/message");

      // Publiceer een verbindingsbericht
      client.publish("chat/message", "ESP8266 is online!");
    }
    else
    {
      Serial.print("🔴 Verbinding mislukt, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// Callback functie voor inkomende MQTT berichten
void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.print("📩 Ontvangen bericht op topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  // Verwerk commando's
  if (message == "lichtAan")
  {
    digitalWrite(LED_BUILTIN_PIN, LOW); // LED aan
    Serial.println("💡 LED aangezet");
  }
  else if (message == "lichtUit")
  {
    digitalWrite(LED_BUILTIN_PIN, HIGH); // LED uit
    Serial.println("💡 LED uitgezet");
  }
  else if (message == "temp")
  {
    float temperature = dht.readTemperature();
    if (isnan(temperature))
    {
      Serial.println("❌ Fout bij het lezen van temperatuur");
    }
    else
    {
      String tempMessage = "Temperatuur: " + String(temperature) + "°C";
      client.publish("chat/message", tempMessage.c_str()); // Gebruik het chat topic
      Serial.println(tempMessage);
    }
  }
  else if (message == "hum")
  {
    float humidity = dht.readHumidity();
    if (isnan(humidity))
    {
      Serial.println("❌ Fout bij het lezen van luchtvochtigheid");
    }
    else
    {
      String humMessage = "Luchtvochtigheid: " + String(humidity) + "%";
      client.publish("chat/message", humMessage.c_str()); // Gebruik het chat topic
      Serial.println(humMessage);
    }
  }
}

void setup()
{
  // Start seriële communicatie
  Serial.begin(115200);

  // Configureer LED pin
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN_PIN, HIGH); // LED uit

  // Start DHT sensor
  dht.begin();

  // Verbind met Wi-Fi en MQTT
  connectWiFi();
  client.setCallback(callback);
  connectMQTT();
}

void loop()
{
  // Houd de MQTT verbinding actief
  if (!client.connected())
  {
    connectMQTT();
  }
  client.loop();

  // Publiceer temperatuur en luchtvochtigheid periodiek
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 60000)
  { // Elke 60 seconden
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temperature))
    {
      String tempMessage = "Temperatuur: " + String(temperature) + "°C";
      client.publish("chat/message", tempMessage.c_str()); // Gebruik het chat topic
      Serial.println(tempMessage);
    }

    if (!isnan(humidity))
    {
      String humMessage = "Luchtvochtigheid: " + String(humidity) + "%";
      client.publish("chat/message", humMessage.c_str()); // Gebruik het chat topic
      Serial.println(humMessage);
    }

    lastPublish = millis();
  }
}
