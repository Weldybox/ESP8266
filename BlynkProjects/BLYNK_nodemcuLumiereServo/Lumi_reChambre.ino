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
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    test.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    test.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15ms for the servo to reach the position
  }
}


BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

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
  // Debug console
  Serial.begin(115200);
  myservo1.attach(D1);
  myservo2.attach(D2);
  Blynk.begin(auth, ssid, pass);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
}
