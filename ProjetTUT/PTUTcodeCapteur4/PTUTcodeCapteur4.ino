/*
 * Déclaration des différentes librairies nécessaires
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
 * Déclaration des variables
 */
int data = A0; //Port analogique du capteur d'humidité
char buf[20]; //Déclaration du buffer qui contiendra les valeurs analogique pour le transfert
//int resultatCurrentData = 0; //
int iteration = 3600; //Temps en seconde entre chaque mesure
const char* ssid = "#Dumartin"; //SSID du Wifi de la rasp
const char* password = "FE4i3v54vR"; //Mot de passe du Wifi de la rasp
const char* mqtt_server = "192.168.15.1"; //Adresse ip du serveur MQTT

WiFiClient espClient; //Déclaration de l'objet Wifi
PubSubClient client(espClient); //Déclaration de l'objet MQTT

/*
 * Fonction de connexion au Wifi
 */
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); //On se connecte au SSID précédemment déclaré
  IPAddress ip(192,168,15,5);//Une adresse IP fixe pour l'objet
  IPAddress gateway(192,168,15,1); //Une gateway (pour aller sur internet mais inutile)
  IPAddress subnet(255,255,255,240); //Le masque du réseau 
  WiFi.config(ip, gateway, subnet); //Configuration du Wifi selon les paramètre ci-dessus
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * Fonction de recoùnnexion qui permet la connexion au broker MQTT
 */
void reconnect() {
  while (!client.connected()) { //Tant que nous sommes pas connecté
    Serial.print("Attempting MQTT connection...");
    String clientId = "Humidity4"; //ATTENTION IL FAUT ABSOLUMENT CHANGER CETTE VALEURE SI ON VEUT METTRE D'AUTRE CAPTEURS 
    if (client.connect(clientId.c_str())) { //Si le client est connecté
      Serial.println("connected"); //On annonce qu'il est connecté
    } else {
      Serial.print("failed, rc="); 
      Serial.print(client.state()); //S'il n'est pas connecté alors on affiche son status
      Serial.println(" try again in"); //On attend un petit peut
      delay(10);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi(); //On configure de Wifi
  client.setServer(mqtt_server, 2222); //On définit les paramètre de connexion au serveur MQTT
  pinMode(data,OUTPUT); //On indique que le pin A0 est une sortie
}

void loop() {
  if (!client.connected()) { //Si nous ne sommes pas connecté
    reconnect(); //On appel la fonction reconnect()
  }
  SendData(); //Sinon on appel la fonction SendData
  delay(100);
  ESP.deepSleep(1800000000); //La carte s'endort
}

void SendData () { //La fonction qui permet d'envoyer la valeure que le capteur d'humidité à relevé
  //int difference = valMax - valMin;
  int Sensordata = analogRead(A0); 
  //resultatCurrentData = 100-(((analogRead(data)-valMin)*100)/difference);
  const char* sendMessage = itoa (Sensordata, buf, 10); //Il faut convertir les données 'integer' en char
  client.publish("JardinConnecte/humidity/data/capteur4", sendMessage); //On envoie le tout sur le topic adéquat
  Serial.println("envoyé");
}
