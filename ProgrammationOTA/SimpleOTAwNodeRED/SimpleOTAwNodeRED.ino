#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = "192.168.1.222";

ESP8266WebServer server;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long current = 0;
unsigned long previous = 0;
const long interval = 5000;

bool prog = true;
uint16_t time_elapsed = 0;
int LEDv = D1;
char buf[20];

void setup() {
 
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
  
  client.publish("/sensor/test/ack", "ok");
  client.subscribe("/sensor/test");
 
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
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}


void loop() {
  server.handleClient();
  client.loop();
  LEDvManage();
}

void LEDvManage (){
  current = millis();
  if (current - previous >= interval){
    previous = current;
    digitalWrite(LEDv, !digitalRead(LEDv));
    delay(10);
  }
}

void SendDataACK () {
    if (!client.connected()) {
    reconnect();
  }
  client.publish("/sensor/test/ack", "ok");
  delay(100);
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
