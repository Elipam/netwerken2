// Configuratie bestand voor de applicatie
module.exports = {
  // MQTT configuratie
  mqtt: {
    // Broker configuratie (blijft hetzelfde)
    host: "netwerkenbasis.com",
    port: 8883,
    tls: true,
    // Broker credentials blijven hetzelfde
    brokerUser: { username: "student", password: "welkom01" },
    // Nieuwe app gebruikers authenticatie
    users: [
      { username: "student1003233", password: "pwNetwerken" },
      {
        username: "Het bedrijf voor wie je aan het werk bent",
        password: "ruim 500.000 euro",
      },
    ],
  },
};
