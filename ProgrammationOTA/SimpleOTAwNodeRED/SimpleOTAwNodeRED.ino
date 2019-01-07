/*
 * Gestion de communication vers broker MQTT sur raspberry.
 * Programme jumelé avec Node-RED.
 * Programmation OTA avec Webserver pour mise à jour.
 * Simple clignotement de LED
 * 
 * Programme édité par Julfi
 * date d'édition : 07/01/2019
 *
 */


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID "Livebox-8E6A"
#define STAPSK  "EC6364F7327751F195ECA47DAC"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = "192.168.1.222"; //adresse IP du broker MQTT Node-RED sur rasp

/*
 * Définition des objets relatifs au webserver, au MQTT et au Wifi
 */
ESP8266WebServer server;
WiFiClient espClient;
PubSubClient client(espClient);

/*
 * Définition des variables pour la fonction millis() dédié au clignotement de la LED
 */
unsigned long current = 0;
unsigned long previous = 0;
const long interval = 5000;

/*
 * Définition des autres variables utiles pour programmer le module OTA
 */
 
bool prog = true;
uint16_t time_elapsed = 0;

int LEDv = D1; //LED de signalisation verte
//char buf[20];

void setup() {
 
  Serial.begin(115200);
  pinMode(LEDv, OUTPUT);
  
  /*
   * Connexion au réseau WIFI
   */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  /*
   * Connexion au service MQTT de la raspberry
   */
   
  client.setServer(mqtt_server, 2222);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ProjetBME")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }

    /*
     * Initialisation d'ArduinoOTA pour la mise à jour à distance
     */
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  }
  
  /*
   * Définition de la réponse à la requête '/prog' sur le Webserveur embarqué
   */
  server.on("/prog",[](){
  server.send(200,"text/plain", "programming..."); //réponse textuelle pour l'interface Node-RED
  prog = true; //On autorise la reprogrammation de l'ESP8266
  time_elapsed = 0; //On définis le temps passé à 0 pour faire un décompte dans la suite
  OTAprog(); //On appel la fonction OTAprog
  });
  
  server.begin();//Démarrage du Webserveur
  
  client.subscribe("/sensor/test"); //Inscription au topic '/sensor/test' pour recevoir des ordres de la Raspberry
}

/*
 * Fonction de reconnexion au service MQTT en cas de perte
 */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ProjetBME")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(10);
    }
  }
}

/*
 * Fonction callback, recois et traite les message venant de '/sensor/test'
 */
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic); //Indique sur le port série le topic du message arrivant
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); //On parcours le message et on l'affiche sur le port série
  }
  Serial.println();
  Serial.println("-----------------------");
}

/*
 * Fonction loop qui écoute sur les différents canaux et effectue le clignotement des LED
 */ 
void loop() {
  server.handleClient(); //Appel de la fonction d'écoute du Webserveur pour la programmtion OTA
  client.loop(); //Appel de la fonction d'écoute pour les messages venant du broker MQTT
  LEDvManage(); //Appel de la fonction qui fait cligniter la LED verte 'LEDv'
}

/*
 * Fonction qui gère le clignotement de LEDv
 */
void LEDvManage (){
  current = millis(); //Depuis combien de temps est exécuté le programme
  if (current - previous >= interval){ //Si cela fait 1 seconde alors on fait on allume/éteind la LEDv
    previous = current; //IMPORTANT: retablis le previous état de temps à celui actuelle pour que l'interval soit constant
    digitalWrite(LEDv, !digitalRead(LEDv));
    delay(10);
  }
}

/*
 * Fonction qui envois un acquittement au raspberry (visible depuis l'interface utilisateur en cas de mise à jour du programme à distance)
 */
void SendDataACK () {
    if (!client.connected()) {
    reconnect();
  }
  client.publish("/sensor/test/ack", "ok");
  delay(100);
}

/*
 * Fonction qui laisse un interval de 15sec pour un mise à jour du progamme à distance
 */
void OTAprog(){
  if(prog)
  {
    uint16_t time_start = millis(); //Définis depuis combien de temps le programme est lancé
    while(time_elapsed < 15000) //Tant que le temps passé dans la boucle n'est pas supérieur à 15sec alors on on continue l'écoute sur le port réseau de l'ESP
    {
      digitalWrite(LEDv, HIGH); //indicateur physique
      ArduinoOTA.handle(); //Appel de la fonction qui permet d'effectuer cette mise à jour
      time_elapsed = millis()-time_start; //Décrémentation du temps avec le temps de départ pour éviter les confusion
      delay(10);
    }
    SendDataACK(); //Appel de la fonction SendDataACK qui envoie un acquittement de bonne modification à la raspberry
    digitalWrite(LEDv, LOW);
    prog = false; //On définit de nouveau la variable prog à false
  }
}
