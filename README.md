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

   Zorg ervoor dat Docker en Docker Compose geïnstalleerd zijn op je systeem. Voer dan het volgende commando uit in de hoofdmap van het project:

   ```sh
   docker-compose up --build
   ```

   Dit commando zal:

   - De Docker image bouwen volgens de instructies in de Dockerfile
   - De containers starten zoals gedefinieerd in docker-compose.yml
   - De HTTPS server (poort 443) en MQTT broker (poort 8883) beschikbaar maken

   Om de applicatie op de achtergrond te draaien, gebruik:

   ```sh
   docker-compose up -d --build
   ```

   Om de containers te stoppen:

   ```sh
   docker-compose down
   ```

5. **Open de clientapplicatie:**

   Open je webbrowser en navigeer naar https://localhost:443. Je zult waarschijnlijk een waarschuwing krijgen over een onveilig certificaat (omdat het zelf-ondertekend is). Kies de optie om door te gaan naar de website.

   Voer je naam in en authenticeer met een van de volgende inloggegevens:

   - Username: "student", Password: "welkom01"

## Projectstructuur

```
netwerken2/
├── app/                  # Client-side web applicatie
├── server/               # Server-side code
│   ├── index.js          # HTTPS en WebSocket server
│   ├── mqtt-server.js    # MQTT broker server
│   └── config.js         # Configuratie instellingen
├── ssl/                  # SSL certificaten
├── Dockerfile            # Instructies voor Docker image
└── docker-compose.yml    # Docker Compose configuratie
```

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
