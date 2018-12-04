/*
 * Code sur ESP8266,
 * Récupération des données météo complètes en direct d'une ville (à définir).
 * Utilisation de l'API OpenWeathermap.
 * 
 * But:
 * Dispose d'un jeu de 5 LED qui s'allumes en fonction du temps qu'il fait.
 * 
 * Ecrit par :  Julfi, administrateur de https://weldybox.com/
 * Pour plus d'information : http://weldybox.com/comment-recevoir-un-sms-venant-de-votre-esp8266-avec-ifttt/
 *
 * 
 */


 
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

int LED[] = {D1,D2,D3,D4,D5}; //déclaration des broches de chaque LEDs

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
  
const char* host = "api.openweathermap.org";
String api = "";
String city = "La Rochelle";


void setup()
{
  
  /*
   * Type de broche pour les LEDs
   */
   
  pinMode(LED[0], OUTPUT);
  pinMode(LED[1], OUTPUT);
  pinMode(LED[2], OUTPUT);
  pinMode(LED[3], OUTPUT);
  pinMode(LED[4], OUTPUT);
  
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
  Serial.println(" connected");
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
    delay(500);
    Serial.println("Envois de la requête");
    
    //Préparation et envois de la reqête vers l'API avec vos paramètre initialisé ci-dessus
    client.print(String("GET /data/2.5/weather?q=" + city + "&APPID=" + api + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                ));
   
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
    else{
    delay(500);
    Serial.println("La réponse de la requête vient d'arriver");
    delay(500);
    }
   char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Réponse non valide"));
      return;
    }
    else{
      Serial.println("");
      Serial.println("============================================================");
      Serial.println("");
      delay(500);
      Serial.println("Traitement de la requête...");
    }
      
  /*
   * Mise en forme de la requête (afin qu'elle soit traitable dans l'IDE arduino)
   * 
   */
    //la taille du buffer à créer -> pour le savoir tester sur le site : https://arduinojson.org/v5/assistant/
    const size_t capacity =JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 390;
    DynamicJsonBuffer jsonBuffer(capacity); //création du buffer
    JsonObject& root = jsonBuffer.parseObject(client); //création du tableau qui va contenir la réponse à notre requête
    if (!root.success()) {
    Serial.println(F("l'Analyse de la requête json a échouée!"));
    return;
    }
    else{
    Serial.println("La requête a été correctement traitée, on passe au traitement des résultats");
    
    /*
     * Partie non obligatoire, c'est juste pour faire stylée
     */
      for (int i = 0; i<5; i++){
        Serial.print(".");
        delay(500);
      }
    }

    /*
    * Traitement de la requête json
    * 
    */
   
    //Parcours du tableau JSON reçus pour avoir les informations que l'on souhaite
    const char* temps = root["weather"][0]["description"]; 
    float temperatureK = root["main"]["temp"];
    int id = root["weather"][0]["id"];
    
    //Convertion de la température de Kelvin à Degré
    float temperatureC = temperatureK - 273.15;
    
    Serial.println("");
    //Affiche des informations sur le port 
    Serial.println("La température à " + city + " est de " + temperatureC + " C°");
    Serial.println("la météo à " + city + " est de type " + temps);
    Serial.print("L\'id du CurrentWeather est ");
    Serial.println(id);
    Serial.println("");
    
    //traitement de l'ID pour déterminer quelle LED allumer en fonction de la météo
    if (id >= 300 && id < 600){
      Serial.println("il pleut actuellement");
      digitalWrite(LED[2], HIGH);
      delay(10);
    }
    else if (id == 800){
      Serial.println("il fait beau");
      digitalWrite(LED[0], HIGH);
      delay(10);
    }
    else if (id >= 800){
      Serial.println("il y a des nuages actuellement");
      digitalWrite(LED[1], HIGH);
      delay(10);
    }
    else if (id >= 200 && id < 300){
      Serial.println("Le y a de l'orage actuellement");
      digitalWrite(LED[3], HIGH);
      digitalWrite(LED[4], HIGH);
      delay(10);
    }else{
      Serial.println("Id reçus inconnu");
    }
    
    client.stop();
  }
}

//Fin du code

void loop()
{
}
