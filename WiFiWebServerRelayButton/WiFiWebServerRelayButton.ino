//Debut d code

#include <ESP8266WiFi.h>

//Declaration basique pour connexion ainsi que du relay

const char* ssid = "Livebox-8E6A";
const char* password = "***************";
const int relay = D1;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Preparation du relay
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  
  // Connexion au WIFI
  Serial.println();
  Serial.println();
  Serial.print("Connexion: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  
  // Essai de connexion au WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi est connecte");
  
  // Lancement du Serveur
  server.begin();
  Serial.println("Serveur est lance");

  // Affichage de l'IP pour se connecter à notre serveur
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Attend que le client envoi des donnees
  Serial.println("Nouveau client");
  while(!client.available()){
    delay(1);
  }
  
  // Lit les pemiere lignes de la requete
  String requete = client.readStringUntil('\r');
  Serial.println(requete);
  client.flush();

 //Verifie et execute les requetes donnees par le client
  int value = LOW;
  if (requete.indexOf("/relay=ON") != -1) {
    digitalWrite(relay, HIGH);
    value = HIGH;
  } 
  if (requete.indexOf("/relay=OFF") != -1){
    digitalWrite(relay, LOW);
    value = LOW;
  }
  // Affichage de la page html, reponse donnees au client

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Le relay est maintenant ");
 
  if(value == HIGH) {
    client.print("allumer");  
  } else {
    client.print("eteint");
  }
  client.println("<br><br><br>");
  client.println("<a href=\"/relay=ON\">ON</a> <br><br><br>");
  client.println("<a href=\"/relay=OFF\">OFF</a><br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client deconnecte");
  Serial.println("");
}
//Fin du programme
