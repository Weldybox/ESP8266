

/*
 * Program to send SMS from ESP8266 via IFTTT.
 * For complete detials visit: www.circuitdigest.com
 * EXAMPLE URL: (do not use This)
 *  https://maker.ifttt.com/trigger/ESP/with/key/b8h22xlElZvP27lrAXS3ljtBa0092_aAanYN1IXXXXX
 *  
 *  Edité par :  Julfi, administrateur de https://weldybox.com/
 *  Tuto disponible ici : http://weldybox.com/comment-recevoir-un-sms-venant-de-votre-esp8266-avec-ifttt/
 *   
 *  
 */




 /*
  * 
  * Initialisation des paramètres WIFI, et de l'Host IFTTT
  * 
  */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


const char* ssid = "";
const char* password = "";

const char* host = "maker.ifttt.com"; //host IFTTT
const int httpsPort = 443; //port de connexion


void setup() {
  Serial.begin(115200); //Le port série pour l'affichage
  Serial.println();

  
  /*
   * 
   * Connexion au WIFI selon les paramètres précédents
   * 
   */

   
  Serial.print("Connexion à ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.println("L'addresse IP est : ");
  Serial.println(WiFi.localIP());//pas nécessaire dans se cas de figure


/*
 * 
 * Connexion à l'host IFTTT
 * 
 */

 
  WiFiClientSecure client;
  Serial.print("Connexion à ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) { //similaire au WIFI!
    Serial.println("La connexion à échoué");
    return;
  }

 
/*
 * 
 * Création de la requête HTTP à envoyer au serveur IFTTT
 * 
 */

 
  String url = ""; //A remplacer par ta requête!
  Serial.print("La requête est : ");
  Serial.println(url);


/*
 * 
 * On envois ensuite la requête en y ajoutant les informations nécessaire pour que le serveur puisse identifier le paquet HTTPs
 * 
 * 
 */

 
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

/*
 * 
 * On retire l'entête HTTPs pour pouvoir récupérer les données utiles
 * 
 */


 
  Serial.println("La requête a été envoyé");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("L'entête du paquet a été reçus");
      break;
    }
  }

/*
 * On affiches les données utiles
 * /!\ PAS OBLIGATOIRE POUR CET API /!\
 * 
 */


 
  String line = client.readStringUntil('\n');

  Serial.println("La réponse du serveur était :");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Fermeture de la connexion");
}

void loop() {
}
