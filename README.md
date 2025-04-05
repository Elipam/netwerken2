## Overzicht

Dit project demonstreert een eenvoudige webapplicatie die WebSockets en MQTT gebruikt voor real-time communicatie. De server luistert naar WebSocket-verbindingen en MQTT-berichten en stuurt berichten door naar alle verbonden clients.

## Vereisten

Zorg ervoor dat je de volgende libraries en dependencies hebt geïnstalleerd:

- Node.js
- npm (Node Package Manager)
- Docker
- Docker Compose

## Hoe het project te draaien

### Stappen

1. **Start WSL (Windows Subsystem for Linux):**

   Open een terminal en start WSL:

   ```sh
   wsl
   ```

2. **Navigeer naar de projectdirectory:**

   ```sh
   cd /mnt/c/Users/eliam/OneDrive/Documenten/Technische informatica 2/netwerkenVervolg/netwerken2
   ```

3. **Genereer een SSL certificaat en sleutel:**

   Als je dit van github hebt, gebruik dan OpenSSL om een zelf-ondertekend certificaat en sleutel te genereren:

   ```sh
   mkdir -p ssl
   openssl genrsa -out ssl/1003233-key.pem 2048
   openssl req -new -key ssl/1003233-key.pem -out ssl/1003233-csr.pem
   openssl x509 -req -days 365 -in ssl/1003233-csr.pem -signkey ssl/1003233-key.pem -out ssl/1003233-cert.pem
   ```

4. **Start de services met Docker Compose:**

   ```sh
   docker-compose up --build
   ```

5. **Open de clientapplicatie:**

   Open je HTML file in je webbrowser, door https://localhost:443 te bezoeken en voer je naam in. Gebruik een van de volgende inloggegevens voor MQTT authenticatie:

   - Username: "student", Password: "welkom01"

## Beschikbare Endpoints

- **HTTPS Server**: https://localhost:443 - Voor de webinterface
- **Secure MQTT Server**: mqtts://localhost:8883 - Beveiligde MQTT poort met TLS

## Verbinden met ESP8266

De ESP8266 code maakt verbinding met de beveiligde MQTT broker op netwerkenbasis.com:8883. Zorg ervoor dat de ESP8266 de juiste certificaten heeft ingesteld in de `secret.h` file.

De ESP8266 kan de volgende commando's ontvangen via MQTT:

- `lichtAan` - Zet de ingebouwde LED aan
- `lichtUit` - Zet de ingebouwde LED uit
- `temp` - Stuurt de huidige temperatuur
- `hum` - Stuurt de huidige luchtvochtigheid
