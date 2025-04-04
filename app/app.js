// Client-side JavaScript voor de chat applicatie

// Maak verbinding met de Socket.IO server
const socket = io("https://localhost:8443", {
  rejectUnauthorized: false,
});

// DOM elementen ophalen
const messagesList = document.querySelector("ul");
const messageInput = document.querySelector("input");
const sendButton = document.querySelector("button");

// Gebruikersgegevens
let username = "";
let isAuthenticated = false;

// Vraag om inloggegevens wanneer pagina laadt
window.onload = () => {
  // Vraag gebruiker om inloggegevens
  username = prompt("Voer je gebruikersnaam in:");
  password = prompt("Voer je wachtwoord in:");

  // Als gebruiker annuleert, gebruik de standaard waarden
  username = username || "student";
  password = password || "welkom01";

  console.log("Bezig met authenticeren...");
  socket.emit("authenticate", { username, password });
};

// Ontvang authenticatie resultaat
socket.on("authenticated", (result) => {
  if (result) {
    isAuthenticated = true;
    console.log("Authenticatie succesvol");
    addMessage(`Beveiligde verbinding. Ingelogd als ${username}`);
  } else {
    console.error("Authenticatie mislukt");
    addMessage(
      "Authenticatie mislukt. Ververs de pagina om opnieuw te proberen."
    );
  }
});

// Event listener voor verzendknop
sendButton.addEventListener("click", sendMessage);

// Event listener voor Enter toets
messageInput.addEventListener("keypress", (e) => {
  if (e.key === "Enter") {
    sendMessage();
  }
});

// Functie om berichten te verzenden
function sendMessage() {
  if (!isAuthenticated) {
    addMessage("Je moet eerst inloggen!");
    return;
  }

  const message = messageInput.value.trim();
  if (message) {
    // Stuur bericht naar server
    socket.emit("message", `${username}: ${message}`);
    messageInput.value = "";
  }
}

// Luister naar inkomende berichten
socket.on("message", (message) => {
  addMessage(message);
});

// Functie om berichten toe te voegen aan de UI
function addMessage(message) {
  const li = document.createElement("li");
  li.textContent = message;
  messagesList.appendChild(li);
}
