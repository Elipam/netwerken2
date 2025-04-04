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
   cd /mnt/c/Users/eliam/OneDrive/Documenten/Technische informatica 2/netwerkenVervolg/netwerken
   ```

3. **Genereer een SSL certificaat en sleutel:**

   Als je dit van github hebt, gebruik dan OpenSSL om een zelf-ondertekend certificaat en sleutel te genereren:

   ```sh
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

   - Username: "chatter1", Password: "chatter1password"
   - Username: "chatter2", Password: "chatter2password"
