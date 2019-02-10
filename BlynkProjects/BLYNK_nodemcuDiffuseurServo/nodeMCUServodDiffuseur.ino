#define BLYNK_PRINT Serial

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Authentification pour blynk
char auth[] = "";
int pos;

char ssid[] = "";
char pass[] = "";


Servo myservo;

void actionner(Servo test){ //Fonction pour actionner le servo moteur
  for (pos = 90; pos <= 115; pos += 1) { //
    test.write(pos);              // On fait bouger le servo moteur d'un degré
    delay(5);                       // on attend 5 seconde avant de réitérer l'opération
  }
  for (pos = 115; pos >= 90; pos -= 1) { // La même fonction pour remettre le servo moteur dans sa position initiale
    test.write(pos);              
    delay(5);                       
  }
}

BLYNK_WRITE(V1) //Fonction de blynk qui surveille la ligne virtuelle v1 et exécute le code ci-dessous
{
  int pinValue = param.asInt(); 

  if(pinValue == 1){ //Si la valeure reçus est "1"
    for (int i = 0; i<2; i++){
      actionner(myservo); //On active le servo deux fois
      delay(200);
    }
  }else{ //Sinon
    actionner(myservo); //On l'active une fois
    delay(100);
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  myservo.attach(4);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run(); //Fonction de blynk qui permet de surveiller en permanence l'arrivée d'éventuelle requête
}
