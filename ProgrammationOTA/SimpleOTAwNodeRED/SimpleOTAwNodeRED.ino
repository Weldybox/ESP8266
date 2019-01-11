#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

Adafruit_BME280 bme;

String Vmax;

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = "192.168.1.222";

ESP8266WebServer server;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long current = 0;
unsigned long previous = 0;
int intervalLED = 1000;
const long intervalBME = 900000;

bool prog = true;
uint16_t time_elapsed = 0;
int LEDv = D1;
char buf[20];

void setup() {
  Wire.begin(2, 12);
  Wire.setClock(100000);
 
  Serial.begin(115200);
  pinMode(LEDv, OUTPUT);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqtt_server, 2222);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266SendReceive")) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }

    bool status;
    status = bme.begin(0x76);  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  }

   server.on("/prog",[](){
  server.send(200,"text/plain", "programming...");
  prog = true;
  time_elapsed = 0;
  OTAprog();
  });
  
  server.begin();
  
  Subinit();
 
}

void Subinit(){
  client.subscribe("/BME");
  client.subscribe("/BME/vmax");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("testOTApost2")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(10);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
}
 
  if ((strcmp(topic,"/BME")==0) && msg == "ok"){
    for(int i = 0; i<10; i++){
      digitalWrite(LEDv, !digitalRead(LEDv));
      delay(100);
    }
    Serial.println(msg);
  }
  else if(strcmp(topic,"/BME/vmax")==0){
    intervalLED = msg.toInt();
    delay(10);
    Serial.println(msg);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  server.handleClient();
  client.loop();
  Manage();
 // ShowTemp();
}

void ShowTemp (){/*
  current = millis();
  Serial.println(previous);
  if (current - previous >= intervalBME){
    previous = current;
    int temp = bme.readTemperature();
    Serial.println("msg BME envoyé");
    Serial.print("température = ");
    Serial.println(temp);
    Serial.println();
    const char* msg = itoa(temp,buf,10);
    Serial.println(msg);
    Serial.println();
    client.publish("/BME/value", msg);
    delay(10);
  }*/
}

void Manage (){

  current = millis();
  if (current - previous >= intervalBME){
    previous = current;

    float temp = bme.readTemperature();
    Serial.println("msg BME envoyé");
    Serial.print("température = ");
    Serial.println(temp);
    Serial.println();
    char tempX[5];
    char* msg = dtostrf(temp, 4, 2, tempX);
    Serial.println(msg);
    Serial.println();
    client.publish("/BME/value", msg);
    delay(10);
  }/*if (current - previous >= intervalLED){
    digitalWrite(LEDv, !digitalRead(LEDv));
    delay(10);
  }*/
}

void SendDataACK () {
  if (!client.connected()) {
    reconnect();
  }
  client.publish("/BME/prog/ack", "ok");
  delay(100);
  Subinit();
}

void OTAprog(){
  if(prog)
  {
    uint16_t time_start = millis();
    while(time_elapsed < 15000)
    {
      digitalWrite(LEDv, HIGH);
      ArduinoOTA.handle();
      time_elapsed = millis()-time_start;
      delay(10);
    }
    SendDataACK();
    digitalWrite(LEDv, LOW);
    prog = false;
  }
    
}
