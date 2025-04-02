# Gebruik een Node.js image als basis
FROM node:14

# Stel de werkdirectory in binnen de container
WORKDIR /app

# Kopieer package.json en package-lock.json voor efficiënte caching van dependencies
COPY package*.json ./

# Installeer de dependencies
RUN npm install

# Kopieer de rest van de applicatie naar de container
COPY . .

# Installeer de benodigde Node.js libraries expliciet
RUN npm install https fs dotenv socket.io mqtt express aedes net path

# Expose poort 443 voor HTTPS verkeer
EXPOSE 443
# Expose poort 1883 voor MQTT verkeer
EXPOSE 1883

# Start both the MQTT server and the HTTPS server
CMD ["sh", "-c", "node server/index.js & node server/mqtt-server.js"]

