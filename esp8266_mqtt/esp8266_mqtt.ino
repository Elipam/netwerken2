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
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
}

// Functie om verbinding te maken met de MQTT broker
void connectMQTT()
{
  // Gebruik BearSSL om het certificaat in te stellen
  BearSSL::X509List cert(local_root_ca);
  espClient.setTrustAnchors(&cert); // Stel het certificaat in
  espClient.setInsecure();          // Voeg deze regel toe als backup als het certificaat problemen geeft

  // Verhoog de timeout voor betere verbindingskansen
  espClient.setTimeout(15000); // 15 seconden timeout

  client.setServer(MQTT_HOST, MQTT_PORT);
  int retries = 0;
  while (!client.connected() && retries < 5) // Maximum 5 pogingen
  {
    // Genereer een unieke client ID met timestamp om connectieproblemen te vermijden
    String clientId = String(MQTT_CLIENT_ID) + "-" + String(millis());

    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
      client.subscribe("chat/message"); // Abonneer op het juiste chat topic

      // Publiceer een verbindingsbericht met client ID
      String connectMessage = String(MQTT_CLIENT_ID) + ": ESP8266 is online!";
      client.publish("chat/message", connectMessage.c_str());
      return; // Succesvol verbonden
    }
    else
    {
      retries++;
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

  // Verwerk commando's - controleert of het commando ergens in het bericht staat
  if (message.indexOf("lichtAan") != -1)
  {
    digitalWrite(LED_BUILTIN_PIN, LOW); // LED aan
    // Bevestig de actie met client ID
    String confirmMessage = String(MQTT_CLIENT_ID) + ": LED is aangezet";
    client.publish("chat/message", confirmMessage.c_str());
  }
  else if (message.indexOf("lichtUit") != -1)
  {
    digitalWrite(LED_BUILTIN_PIN, HIGH); // LED uit
    // Bevestig de actie met client ID
    String confirmMessage = String(MQTT_CLIENT_ID) + ": LED is uitgezet";
    client.publish("chat/message", confirmMessage.c_str());
  }
  else if (message.indexOf("temp") != -1)
  {
    float temperature = dht.readTemperature();
    if (isnan(temperature))
    {
      client.publish("chat/message", String(String(MQTT_CLIENT_ID) + ": Fout bij het lezen van temperatuur").c_str());
    }
    else
    {
      String tempMessage = String(MQTT_CLIENT_ID) + ": Temperatuur: " + String(temperature) + "°C";
      client.publish("chat/message", tempMessage.c_str()); // Gebruik het chat topic
    }
  }
  else if (message.indexOf("hum") != -1)
  {
    float humidity = dht.readHumidity();
    if (isnan(humidity))
    {
      client.publish("chat/message", String(String(MQTT_CLIENT_ID) + ": Fout bij het lezen van luchtvochtigheid").c_str());
    }
    else
    {
      String humMessage = String(MQTT_CLIENT_ID) + ": Luchtvochtigheid: " + String(humidity) + "%";
      client.publish("chat/message", humMessage.c_str()); // Gebruik het chat topic
    }
  }
}

void setup()
{
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
  // Houd de Wi-Fi verbinding actief
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }

  // Houd de MQTT verbinding actief
  if (!client.connected())
  {
    connectMQTT();
  }
  client.loop();
}
