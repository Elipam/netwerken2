// Configuratie bestand voor de applicatie
module.exports = {
  // MQTT configuratie
  mqtt: {
    // Poort waarop de MQTT broker luistert
    port: 1883,
    // Lijst van gebruikers die geautoriseerd zijn om te verbinden
    users: [
      { username: "user1", password: "user1password" },
      { username: "user2", password: "user2password" },
    ],
  },
};
