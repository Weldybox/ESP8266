/*
 * Code sur ESP8266,
 * Récupération des coordonnées de l'ISS toute les heures.
 * interface web de contôle on/off
 * 
 * But: contrôle d'une lampe qui s'allume en plus à chaque passage de l'ISS au-dessus de la France.
 * 
 * 
 * Ecrit par :  Julfi, administrateur de https://weldybox.com/
 * Pour plus d'information : http://weldybox.com/diy-dune-lampe-connecte-qui-detecte-la-passage-de-liss/
 *
 * 
 */


#include <ESP8266WiFi.h>
#include <ArduinoJson.h>


WiFiServer server(80);
/*
 *Définition du header et du code CSS 
 *Ainsi que detecteur du status du relay
 * 
 */

int relayPin = D1; //définition du pin du shield relay
 
String header;
String css ="html { font-family: Roboto; display: inline-block; margin: 0px auto; text-align: center; } body strong{ font-size: 20px; } * { -webkit-box-sizing: border-box; -moz-box-sizing: border-box; box-sizing: border-box; } .buttons { margin: 10%; text-align: center; } .btn-hover { width: 200px; font-size: 16px; font-weight: 600; color: #fff; cursor: pointer; margin: 5px; height: 55px; text-align:center; border: none; background-size: 300% 100%; border-radius: 50px; moz-transition: all .4s ease-in-out; -o-transition: all .4s ease-in-out; -webkit-transition: all .4s ease-in-out; transition: all .4s ease-in-out; } .btn-hover:hover { background-position: 100% 0; moz-transition: all .4s ease-in-out; -o-transition: all .4s ease-in-out; -webkit-transition: all .4s ease-in-out; transition: all .4s ease-in-out; } .btn-hover:focus { outline: none; } .btn-hover.color-9 { background-image: linear-gradient(to right, #eb3941, #f15e64, #e14e53, #e2373f); box-shadow: 0 4px 15px 0 rgba(242, 97, 103, .4); } .btn-hover.color-11 { background-image: linear-gradient(to right, #39eb44, #5df07b, #e14e53, #37e23c); box-shadow: 0 5px 15px rgba(96, 241, 106, 0.75); }";
String outputRelayState = "off";


/*
* Définition de la millis
* puis de la variable de comparaison
* 
*/
  
unsigned long currentMillis;
unsigned long compar = 180;



/*
* Déclaration des paramètres de connexion WIFI
* 
*/
 
const char* ssid = "";
const char* password = "";

/*
* Déclaration des paramètre de connexion à l'API
* 
*/
  
const char* host = "api.open-notify.org";

void setup()
{
  /*
   * Type de broche pour les LEDs
   */
   
   pinMode(relayPin, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  
  /*
   * Connexion au WIFI
   * 
  */

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
  Serial.println(" connected");

   server.begin();
   Serial.println("Serveur est lance");

}

void loop()
{
  currentMillis = millis() / 1000; //convertions de la millis en seconde

  /*
   * 
   * Check si il y a un nouveau client
   * 
   */

  WiFiClient client = server.available();
  if (client) { // Si un nouveau client se connecté
    Serial.println("New Client.");          
    String currentLine = "";               
    while (client.connected()) { // On refait cette boucle tant qu'il est connecté
      if (client.available()) { //Si celui-ci est disponible            
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK"); // Le header HTTP on commence toujours par (e.g. HTTP/1.1 200 OK)
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Traitement et stokage des états de la requête reçus de l'utilisateur
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("LED on");
              outputRelayState = "on";
              digitalWrite(relayPin, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("LED off");
              outputRelayState = "off";
              digitalWrite(relayPin, LOW);
            }
            
            /*
             * Affichage de la page WEB
             * 
             */

            client.println("<!DOCTYPE html><html lang=\"fr\">");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link href=\"https://fonts.googleapis.com/css?family=Lato\" rel=\"stylesheet\">");
            client.println("<style>");
            client.println(css); //met le CSS ici!
            client.println("</style>");
            client.println("<body><h1>Serveur de gestion des lumi&egrave;res</h1>");
            
            // On affiche l'état du GPIO
            client.println("<p>Lumi&egrave;re principale - Statue <strong>" + outputRelayState + "</strong></p>");
            client.println("<div class=\"buttons\">");

            // On met en forme le bouton on/off en fonction de l'état du GPIO
            if (outputRelayState=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"btn-hover color-11\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"btn-hover color-9\">OFF</button></a></p>");
            }
            client.println("</div>"); 
            client.println();
            // Break du indique la fin de la boucle
            break;
          } else { // Si on a un nouvelle ligne alors on nettois currentline
            currentLine = "";
          }
        } else if (c != '\r') { // si on a autre chose comme un caractère de transport alors on retourne à la ligne
          currentLine += c; // Enfin, on l'ajoute à la currentline
        }
      }
    }
    // On nettoie le header
    header = "";
    //On stop le client
      client.stop();
    }
  
  /*
   * 
   * On compare pour savoir si ça fait une heure que le code est lancé
   * 
   */
    if (currentMillis >= compar){
      server.begin();
      Serial.println("Serveur est lance");
      WiFiClient client;
      
      /*
      * Connexion à l'API
      * 
      */
      
      Serial.printf("\n[Connexion à %s ... ", host);
      delay(500);
      if (client.connect(host, 80))
      {
        Serial.println("Connecté!");
        Serial.println("Envois de la requête");
      
        //Préparation et envois de la reqête vers l'API avec vos paramètre initialisé ci-dessus
        client.print(String("GET /iss-now.json HTTP/1.1\r\nHost: api.open-notify.org\r\nConnection: close\r\n\r\n"));
        /*
        * 
        * Récupération de la réponse
        */
       
        char status[32] = {0}; //définition d'un char pour contenir la réponse à la requête
        client.readBytesUntil('\r', status, sizeof(status));
        if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
          Serial.print(F("Réponse inattendue: "));
          Serial.println(status);
          return;
        }
        char endOfHeaders[] = "\r\n\r\n";
        if (!client.find(endOfHeaders)) {
          Serial.println(F("Réponse non valide"));
          return;
        }
        //la taille du buffer à créer -> pour le savoir tester sur le site : https://arduinojson.org/v5/assistant/
        const size_t capacity =JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
        DynamicJsonBuffer jsonBuffer(capacity); //création du buffer
        JsonObject& root = jsonBuffer.parseObject(client); //création du tableau qui va contenir la réponse à notre requête
        if (!root.success()) {
          Serial.println(F("l'Analyse de la requête json a échouée!"));
          return;
        }
        //Parcours du tableau JSON reçus pour avoir les informations que l'on souhaite
        float longitude = root["iss_position"]["longitude"]; 
        float latitude = root["iss_position"]["latitude"]; 
        
        
        //Affiche des informations sur le port (pour debuggage) 
        /*
        Serial.println("");
        Serial.print("longitude = ");
        Serial.print(longitude);
        Serial.print(" latitude = ");
        Serial.println(latitude);*/
        if (longitude >= -1,15 && longitude <= 6,23 && latitude >= 42.57 && latitude <= 50.96) { //test pour savoir si les coordonnées correspondent à celle de la France
          digitalWrite(relayPin, HIGH);
          outputRelayState = "on";
        }
       client.stop();
      }
      
      compar += 180; //On ajoute 3 min à compar pour la prochaine itération
      }

    // Fermeture de la connexion
    Serial.println("Client disconnected.");
    Serial.println("");
    
}
