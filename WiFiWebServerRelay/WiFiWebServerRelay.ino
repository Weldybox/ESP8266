//Debut du code avec l'inclusion de la librairie nécessaire

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
  digitalWrite(relay, 0);
  
  // Connexion au WIFI
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  
  // Essai de connexion au WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Lancement du Serveur
  server.begin();
  Serial.println("Server started");

  // Affichage de l'IP pour se connecter à notre serveur
  Serial.println(WiFi.localIP());
}

void loop() {
  // Ragarde si un client a été connecte
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Attend que le client envoi des donnees
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Lit les pemiere lignes de la requete
  String requete = client.readStringUntil('\r');
  Serial.println(requete);
  client.flush();
  
 //Verifie et execute les requetes donnees par le client
  int val = 0;
  if (requete.indexOf("/relay/ON") != -1) {
    digitalWrite(relay, 1);
    val = 1;
  } 
  if (requete.indexOf("/relay/OFF") != -1){
    digitalWrite(relay, 0);
    val = 0;
  }
  
  // Affichage de la page html, reponse donnees au client
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

 // regarge la dans quelle position est mis le relay pour afficher son etat
client.print("Le relay est maintenant ");
  if(val == 1) {
    client.print("allumer");  
  } else {
    client.print("eteint");
  }
  client.println("</html>");
  
 //Deconnexion du client
  delay(1);
  Serial.println("Client deconnecte");
  Serial.println("");
}
//Fin du programme
