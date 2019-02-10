
#define BLYNK_PRINT Serial

#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";
int pos;
char ssid[] = "";
char pass[] = "";


Servo myservo;


BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    for (int i = 0; i<2; i++){
    for (pos = 90; pos <= 115; pos += 1) {
    // in steps of 1 degree
      myservo.write(pos);           
      delay(5);                      
    }
    for (pos = 115; pos >= 90; pos -= 1) {
      myservo.write(pos);             
      delay(5);                       
    }
    delay(200);
    }
  }else{
        for (pos = 90; pos <= 115; pos += 1) {
    // in steps of 1 degree
      myservo.write(pos);              
      delay(5);                       
    }
    for (pos = 115; pos >= 90; pos -= 1) { 
      myservo.write(pos);              
      delay(5);                       
    }
    Serial.println("notcool");
    digitalWrite(2, LOW);
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
  Blynk.run();
}
