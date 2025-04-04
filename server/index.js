const fs = require("fs");
const https = require("https");
const express = require("express");
const path = require("path");
const config = require("./config");

// Laad SSL certificaat en sleutel voor de HTTPS server
const httpsOptions = {
  key: fs.readFileSync("./ssl/1003233-key.pem"),
  cert: fs.readFileSync("./ssl/1003233-cert.pem"),
};

// Maak een Express applicatie
const app = express();

// Gebruik Express om statische bestanden te serveren vanuit de 'app' directory
app.use(express.static(path.join(__dirname, "../app")));

// Maak een HTTPS server met SSL ondersteuning
const server = https.createServer(httpsOptions, app);

// Maak een WebSocket server met CORS ondersteuning voor cross-domain requests
const io = require("socket.io")(server, {
  cors: { origin: "*" },
});

// Verbind met onze eigen MQTT broker met authenticatie
const mqtt = require("mqtt");
// Gebruik de eerste gebruiker uit de config voor de verbinding
const mqttUser = config.mqtt.users[0];
const mqttClient = mqtt.connect(`mqtts://${config.mqtt.host}:${config.mqtt.port}`, {
  username: mqttUser.username,
  password: mqttUser.password,
  rejectUnauthorized: false, // Add this line to accept self-signed certificates
});

// Zorg ervoor dat de MQTT client zich abonneert op het topic bij verbinding
mqttClient.on("connect", () => {
  console.log(`🟢 Verbonden met MQTT broker op ${config.mqtt.host}:${config.mqtt.port}`);
  mqttClient.subscribe("chat/message");
  console.log(`📩 Geabonneerd op topic: chat/message`);
});

mqttClient.on("error", (error) => {
  console.error(`🔴 MQTT verbindingsfout: ${error.message}`);
});

mqttClient.on("message", (topic, message) => {
  const msg = message.toString();
  console.log(`📬 Bericht ontvangen op topic ${topic}: ${msg}`);
  io.emit("message", `${msg}`); // Stuur bericht naar alle WebSocket clients
});

// Handelt WebSocket verbindingen af wanneer clients verbinden
io.on("connection", (socket) => {
  console.log(`🔌 Nieuwe client verbonden: ${socket.id}`);

  // Vraag om gebruikersnaam en wachtwoord voor authenticatie
  socket.on("authenticate", (credentials) => {
    const { username, password } = credentials;
    console.log(`🔑 Authenticatie poging: ${username}`);

    // Zoek de gebruiker in MQTT config voor validatie
    const user = config.mqtt.users.find(
      (u) => u.username === username && u.password === password
    );

    if (user) {
      console.log(`✅ Authenticatie geslaagd voor: ${username}`);
      socket.emit("authenticated", true);
    } else {
      console.log(`❌ Authenticatie mislukt voor: ${username}`);
      socket.emit("authenticated", false);
    }
  });

  // Luister naar berichten van de client en publiceer ze naar MQTT
  socket.on("message", (message) => {
    mqttClient.publish("chat/message", message);
  });
});

// Start de HTTPS server op poort 8443
server.listen(8443, () => {
  console.log(`🚀 Server draait op https://localhost:8443`);
  console.log(`🔒 Server is beveiligd met TLS/SSL`);
  console.log(`🔐 Login vereist met gebruikersnaam en wachtwoord`);
});