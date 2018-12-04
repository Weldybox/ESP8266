/*
 * Nom : ServeurESP8266Gestion_v1.0.ino
 * Code by julfi (MailduJeudi, 15/11/2018
 * 
 * Blog: https://weldybox.com/
 * 
 * Code de base d'un Serveur WEB avec un ESP8266
 * Ex: Contrôler des lumère, un état.
 * 
 * v1.0:
 * 
 * - Un interface web avec un CSS
 * - Un panneau de contrôle basique (Bouton ON/OFF)
 * - Un retour sur l'état de l'OUPUT
 * 
 */
 
 #include <ESP8266WiFi.h>

  /*
 * ==============================================================================================
 * Paramètres de connexion à la box.
 * ==============================================================================================
 */
 
const char* ssid = "";
const char* password = "";

  /*
 * ==============================================================================================
 * Initialisation des paramètres qui nous serviront
 * ==============================================================================================
 */
 
// Le serveur Web écoute sur le port 80
WiFiServer server(80);

// Variable qui stock le HTTP request header
String header;

// Le status de la LED
String output5State = "off";

//Le code css mis en forme avec le site http://davidjwatts.com/youtube/esp8266/esp-convertHTM.html
String css ="html { font-family: Roboto; display: inline-block; margin: 0px auto; text-align: center; } body strong{ font-size: 20px; } * { -webkit-box-sizing: border-box; -moz-box-sizing: border-box; box-sizing: border-box; } .buttons { margin: 10%; text-align: center; } .btn-hover { width: 200px; font-size: 16px; font-weight: 600; color: #fff; cursor: pointer; margin: 5px; height: 55px; text-align:center; border: none; background-size: 300% 100%; border-radius: 50px; moz-transition: all .4s ease-in-out; -o-transition: all .4s ease-in-out; -webkit-transition: all .4s ease-in-out; transition: all .4s ease-in-out; } .btn-hover:hover { background-position: 100% 0; moz-transition: all .4s ease-in-out; -o-transition: all .4s ease-in-out; -webkit-transition: all .4s ease-in-out; transition: all .4s ease-in-out; } .btn-hover:focus { outline: none; } .btn-hover.color-9 { background-image: linear-gradient(to right, #eb3941, #f15e64, #e14e53, #e2373f); box-shadow: 0 4px 15px 0 rgba(242, 97, 103, .4); } .btn-hover.color-11 { background-image: linear-gradient(to right, #39eb44, #5df07b, #e14e53, #37e23c); box-shadow: 0 5px 15px rgba(96, 241, 106, 0.75); }";

void setup() {
/*
 * ==============================================================================================
 * Déclaration des paramètres de départ
 * ==============================================================================================
 */
 
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  
/*
 * ==============================================================================================
 * Connexion au WIFI
 * ==============================================================================================
 */ 
 
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
  Serial.println(WiFi.localIP());
  
  // Lancement du Serveur
  server.begin();
  Serial.println("Serveur est lance");

}
void loop(){
  
  WiFiClient client = server.available(); // Ecoute et attent l'arrivée de nouveau client

  /*
 * ==============================================================================================
 * Traitement dans le cas ou un nouveau client est connecté
 * ==============================================================================================
 */
  if (client) { // Si un nouveau client se connecté
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) { // On refait cette boucle tant qu'il est conencté
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          if (currentLine.length() == 0) {
  /*
 * ==============================================================================================
 * Ajout du code html de la page
 * ==============================================================================================
 */
            client.println("HTTP/1.1 200 OK"); // Le header HTTP on commence toujours par (e.g. HTTP/1.1 200 OK)
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Traitement et stokage des états de la requête reçus de l'utilisateur
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("LED on");
              output5State = "on";
              digitalWrite(LED_BUILTIN, LOW);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("LED off");
              output5State = "off";
              digitalWrite(LED_BUILTIN, HIGH);
            }
            
            client.println("<!DOCTYPE html><html lang=\"fr\">");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link href=\"https://fonts.googleapis.com/css?family=Lato\" rel=\"stylesheet\">");
            client.println("<style>");
            client.println(css); //met le CSS ici!
            client.println("</style>");
            client.println("<body><h1>Serveur de gestion des lumi&egrave;res</h1>");
            
            // On affiche l'état du GPIO
            client.println("<p>Lumi&egrave;re principale - Statue <strong>" + output5State + "</strong></p>");
            client.println("<div class=\"buttons\">");

            // On met en forme le bouton on/off en fonction de l'état du GPIO
            if (output5State=="off") {
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
    
    // Fermeture de la connexion
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
