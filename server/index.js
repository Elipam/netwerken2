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

// Verbind met externe MQTT broker met authenticatie
const mqtt = require("mqtt");

// MQTT connection options with retry logic
const mqttOptions = {
  username: config.mqtt.brokerUser.username,
  password: config.mqtt.brokerUser.password,
  rejectUnauthorized: false, // Accept self-signed certificates
  connectTimeout: 10000, // 10 seconds timeout
  reconnectPeriod: 5000, // Try to reconnect every 5 seconds
  keepalive: 60, // Keepalive of 60 seconds
};

// Gebruik de broker credentials (niet de app gebruikers)
const mqttClient = mqtt.connect(
  `mqtts://${config.mqtt.host}:${config.mqtt.port}`,
  mqttOptions
);

// Add additional event handlers for better diagnostics
mqttClient.on("reconnect", () => {
  console.log(`Opnieuw verbinden met MQTT broker...`);
});

mqttClient.on("offline", () => {
  console.log(`MQTT client offline`);
});

// Zorg ervoor dat de MQTT client zich abonneert op het topic bij verbinding
mqttClient.on("connect", () => {
  console.log(
    `Verbonden met externe MQTT broker op ${config.mqtt.host}:${config.mqtt.port}`
  );
  mqttClient.subscribe("chat/message");
  console.log(`Geabonneerd op topic: chat/message`);
});

mqttClient.on("error", (error) => {
  console.error(`MQTT verbindingsfout: ${error.message}`);
});

mqttClient.on("message", (topic, message) => {
  const msg = message.toString();
  console.log(`Bericht ontvangen op topic ${topic}: ${msg}`);
  io.emit("message", `${msg}`); // Stuur bericht naar alle WebSocket clients
});

// Handelt WebSocket verbindingen af wanneer clients verbinden
io.on("connection", (socket) => {
  console.log(`Nieuwe client verbonden: ${socket.id}`);

  // Add socket authentication state
  socket.authenticated = false;
  socket.username = null;

  // Vraag om gebruikersnaam en wachtwoord voor authenticatie
  socket.on("authenticate", (credentials) => {
    const { username, password } = credentials;
    console.log(`Authenticatie poging: ${username}`);

    // Zoek de gebruiker in MQTT config voor validatie
    const user = config.mqtt.users.find(
      (u) => u.username === username && u.password === password
    );

    if (user) {
      console.log(`Authenticatie geslaagd voor: ${username}`);
      socket.authenticated = true;
      socket.username = username;
      socket.emit("authenticated", true);
    } else {
      console.log(`Authenticatie mislukt voor: ${username}`);
      socket.authenticated = false;
      socket.username = null;
      socket.emit("authenticated", false);
    }
  });

  // Luister naar berichten van de client en publiceer ze naar MQTT
  socket.on("message", (message) => {
    // Only allow message publishing if authenticated
    if (socket.authenticated) {
      // Add client ID to the message
      const formattedMessage = `${socket.id}: ${message}`;
      mqttClient.publish("chat/message", formattedMessage);
    } else {
      console.log(`Ongeautoriseerde message poging: ${socket.id}`);
      socket.emit("error", "Je moet ingelogd zijn om berichten te versturen");
    }
  });

  // Handle disconnections
  socket.on("disconnect", () => {
    console.log(`Client verbroken: ${socket.id}`);
  });
});

// Start de HTTPS server op poort 443
server.listen(443, () => {
  console.log(`Server draait op https://localhost:443`);
  console.log(`Server is beveiligd met TLS/SSL`);
});
