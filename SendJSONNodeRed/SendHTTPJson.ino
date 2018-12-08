#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

String host = "192.168.1.111:1880/data"; //Configuration de l'url HTTP pour POSTER les données

/*
 * Configuration connexion au point d'accès Wifi
 */
 
const char* ssid = "";
const char* password = "";

/*
 * Fonction qui POST les données en JSON
 */
void JsonPOST (int data, String host){
  HTTPClient http;    //on déclare l'objet HTTP
  http.begin("http://" + host);      //On spécifie la destination de la requête
  http.addHeader("Content-Type", "application/json");  //ON déclare le type de contenue
  char buf[20]; //définition d'un buffer pour transcrire les données int en données char
  const char* datajson = itoa (data, buf, 10); //transcription...
  /*
   * définition de la requête POST
   */
  String result = "{\"data\":";
    result += datajson;
    result += "}";
  http.POST(result);//POST de la requête
  http.end(); //Fermeture de la connexion
}

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password); //Connexion au Wifi

  server.begin();
  Serial.println("Serveur demarrer !");
  while (WiFi.status() != WL_CONNECTED) { //Attente que la carte soit connecté au WIFI
    delay(500);
    Serial.println("Waiting for connection");
  }
}
 
void loop() {
 for (int i = 0; i<20; i++){ //Modifiction de la variable i pour simuler un changement de valeur
   if(WiFi.status()== WL_CONNECTED){ //Si la carte est bien connecté au WIFI
      JsonPOST(i, host); //Envois de la requête JSON
   }
   else{ //Sinon
      Serial.println("Error in WiFi connection");
   }
   delay(1000);
 }
}
