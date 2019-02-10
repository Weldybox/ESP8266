#define BLYNK_PRINT Serial

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";
int pos;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
int current = 0;
int interval = 1000;


Servo myservo1;
Servo myservo2;


void actionner(Servo test){
  for (pos = 0; pos <= 180; pos += 1) {
    test.write(pos);             
    delay(5);                       
  }
  for (pos = 180; pos >= 0; pos -= 1) { 
    test.write(pos);              
    delay(5);                      
  }
}


BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); 

  if(pinValue == 1){
    actionner(myservo1);
    delay(10);
  }else{
    actionner(myservo2);
    delay(10);   
}
}

void setup()
{

  Serial.begin(115200);
  myservo1.attach(D1);
  myservo2.attach(D2);
  Blynk.begin(auth, ssid, pass);

}

void loop()
{
  Blynk.run();
}
