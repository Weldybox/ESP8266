#define BLYNK_PRINT Serial

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";
int pos;
char ssid[] = "";
char pass[] = "";
int current = 0;
int interval = 1000;


Servo myservo1;
Servo myservo2;


void actionner(Servo test){
  for (pos = 0; pos <= 180; pos += 1) { 
    test.write(pos);//Le servo moteur tourne jusqu'à atteindre 180 degré        
    delay(5);                       
  }
  for (pos = 180; pos >= 0; pos -= 1) { 
    test.write(pos); //Il revient à sa position initiale           
    delay(5);                       
  }
}


BLYNK_WRITE(V2)//Fonction blynk qui surveille l'arrivée de trame sur le lien virtual "V2"
{
  int pinValue = param.asInt(); 

  if(pinValue == 1){
    actionner(myservo1); //Si le payload reçus est "1" alors on actionne le premier servo
    delay(10);
  }else{
    actionner(myservo2); //Sinon on actionne le second servo
    delay(10);   
}
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  myservo1.attach(D1);
  myservo2.attach(D2);
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run(); //Fonction de blynk qui surveille si un requête est émise depuis le client
}
