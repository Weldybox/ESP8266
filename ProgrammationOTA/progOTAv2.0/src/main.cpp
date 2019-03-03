#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char* ssid = "Livebox-8E6A";
const char* mdp = "EC6364F7327751F195ECA47DAC";
//const char* otamdp = "Nasreddine";
const char* hostString = "WemosOTAv2.0";

unsigned long previousMillis = 0;
unsigned long otaMillis;

void confOTA(){
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(hostString);
  //ArduinoOTA.setPassword(otamdp);

  ArduinoOTA.onStart([](){
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    Serial.println("MAJ OTA is coming!");
    otaMillis=millis();
  });

  ArduinoOTA.onEnd([]{
    Serial.print("MAJ terminé en : ");
    Serial.print((millis() - otaMillis)/1000);
    Serial.println(" secondes.");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
    Serial.printf("Progression : %u%%\r", (progress / (total/100)));
  });

  ArduinoOTA.onError([](ota_error_t error){
    switch (error)
    {
      case OTA_AUTH_ERROR:  Serial.println("OTA_AUTH_ERROR");
        break;
      case OTA_BEGIN_ERROR: Serial.println("OTA_BEGIN_ERROR");
        break;
      case OTA_CONNECT_ERROR: Serial.println("OTA_CONNECT_ERROR");
        break;
      case OTA_END_ERROR: Serial.println("OTA_END_ERROR");
        break;
      case OTA_RECEIVE_ERROR: Serial.println("OTA_RECEIVE_ERROR");
        break;
      default: Serial.println("Erreur inconnue");
    }
  });

  ArduinoOTA.begin();

}

void setup() {
  Serial.begin(115200);
  pinMode(D1, OUTPUT);
  Serial.println("\r\nDémarrage...");
  //ESP.restart();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, mdp);
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("Erreur de connexion Wifi! Reboot...");
    delay(5000);
    ESP.restart();
  }

  confOTA();

  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 2000){
    previousMillis = millis();
    Serial.println("on/off");
    digitalWrite(D1, !digitalRead(D1));
  }
  ArduinoOTA.handle();
  delay(10);

}