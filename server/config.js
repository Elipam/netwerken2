// Configuratie bestand voor de applicatie
module.exports = {
  // MQTT configuratie
  mqtt: {
    // Nieuwe broker configuratie
    host: "netwerkenbasis.com",
    port: 8883,
    tls: true,
    // Nieuwe gebruikersnaam en wachtwoord
    users: [
      { username: "student", password: "welkom01" },
    ],
  },
};
