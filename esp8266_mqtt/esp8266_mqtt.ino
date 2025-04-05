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
  Serial.println("\nVerbonden met Wi-Fi");
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
    Serial.print("Verbinden met MQTT broker (poging ");
    Serial.print(retries + 1);
    Serial.print(")...");

    // Genereer een unieke client ID met timestamp om connectieproblemen te vermijden
    String clientId = String(MQTT_CLIENT_ID) + "-" + String(millis());

    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
      Serial.println("Verbonden met MQTT broker");
      client.subscribe("chat/message"); // Abonneer op het juiste chat topic
      Serial.println("Geabonneerd op topic: chat/message");

      // Publiceer een verbindingsbericht met client ID
      String connectMessage = String(MQTT_CLIENT_ID) + ": ESP8266 is online!";
      client.publish("chat/message", connectMessage.c_str());
      return; // Succesvol verbonden
    }
    else
    {
      Serial.print("Verbinding mislukt, rc=");
      Serial.print(client.state());
      Serial.println(", nieuwe poging over 5 seconden");
      retries++;
      delay(5000);
    }
  }

  if (retries >= 5)
  {
    Serial.println("Kon niet verbinden met MQTT broker na meerdere pogingen");
    Serial.println("Controleer de broker instellingen en netwerk verbinding");
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
  Serial.print("Ontvangen bericht op topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  // Verwerk commando's - controleert of het commando ergens in het bericht staat
  if (message.indexOf("lichtAan") != -1)
  {
    digitalWrite(LED_BUILTIN_PIN, LOW); // LED aan
    Serial.println("LED aangezet");
    // Bevestig de actie met client ID
    String confirmMessage = String(MQTT_CLIENT_ID) + ": LED is aangezet";
    client.publish("chat/message", confirmMessage.c_str());
  }
  else if (message.indexOf("lichtUit") != -1)
  {
    digitalWrite(LED_BUILTIN_PIN, HIGH); // LED uit
    Serial.println("LED uitgezet");
    // Bevestig de actie met client ID
    String confirmMessage = String(MQTT_CLIENT_ID) + ": LED is uitgezet";
    client.publish("chat/message", confirmMessage.c_str());
  }
  else if (message.indexOf("temp") != -1)
  {
    float temperature = dht.readTemperature();
    if (isnan(temperature))
    {
      Serial.println("Fout bij het lezen van temperatuur");
      client.publish("chat/message", String(String(MQTT_CLIENT_ID) + ": Fout bij het lezen van temperatuur").c_str());
    }
    else
    {
      String tempMessage = String(MQTT_CLIENT_ID) + ": Temperatuur: " + String(temperature) + "°C";
      client.publish("chat/message", tempMessage.c_str()); // Gebruik het chat topic
      Serial.println(tempMessage);
    }
  }
  else if (message.indexOf("hum") != -1)
  {
    float humidity = dht.readHumidity();
    if (isnan(humidity))
    {
      Serial.println("Fout bij het lezen van luchtvochtigheid");
      client.publish("chat/message", String(String(MQTT_CLIENT_ID) + ": Fout bij het lezen van luchtvochtigheid").c_str());
    }
    else
    {
      String humMessage = String(MQTT_CLIENT_ID) + ": Luchtvochtigheid: " + String(humidity) + "%";
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
  // Houd de Wi-Fi verbinding actief
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Wi-Fi verbinding verloren, opnieuw verbinden...");
    connectWiFi();
  }

  // Houd de MQTT verbinding actief
  if (!client.connected())
  {
    connectMQTT();
  }
  client.loop();
}
