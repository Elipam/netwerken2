const aedes = require("aedes")();
const server = require("net").createServer(aedes.handle);
const config = require("./config");

// Poort voor MQTT verbindingen
const port = config.mqtt.port;

// Authenticatie hook voor MQTT clients
aedes.authenticate = (client, username, password, callback) => {
  // Converteer buffer naar string als het bestaat
  const passwordStr = password ? password.toString() : "";
  const usernameStr = username ? username.toString() : "";

  // Zoek gebruiker in de config
  const user = config.mqtt.users.find(
    (u) => u.username === usernameStr && u.password === passwordStr
  );

  if (user) {
    // Authenticatie gelukt
    callback(null, true);
  } else {
    // Authenticatie mislukt
    const error = new Error("Authenticatie mislukt");
    error.returnCode = 4;
    callback(error, null);
  }
};

// Event wanneer een client verbindt
aedes.on("client", (client) => {
  // Client verbonden - hier kun je logging of andere acties toevoegen
});

// Event wanneer een client de verbinding verbreekt
aedes.on("clientDisconnect", (client) => {
  // Client verbinding verbroken - hier kun je opruimacties uitvoeren
});

// Event wanneer een bericht gepubliceerd wordt
aedes.on("publish", (packet, client) => {
  // Bericht gepubliceerd - hier kun je berichten monitoren of verwerken
});

// Start de MQTT server
server.listen(port);

// Exporteer de aedes instantie voor gebruik elders in de applicatie
module.exports = aedes;
