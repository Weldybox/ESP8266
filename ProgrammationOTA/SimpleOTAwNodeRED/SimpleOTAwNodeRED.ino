#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

#ifndef STASSID
#define STASSID "Livebox-8E6A"
#define STAPSK  "EC6364F7327751F195ECA47DAC"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* mqtt_server = "192.168.1.222";

ESP8266WebServer server;
WiFiClient espClient;
PubSubClient client(espClient);

bool prog = true;
uint16_t time_elapsed = 0;
int LEDv = D1;
//int LEDr = D2;
//int Button = D3;
char buf[20];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  
  //pinMode(LEDr, OUTPUT);
  pinMode(LEDv, OUTPUT);
  //pinMode(Button, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, 2222);
  //client.setCallback(callback);
  
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
  
  server.on("/prog",[](){
  server.send(200,"text/plain", "programming...");
  prog = true;
  time_elapsed = 0;
  programming();
  });
  server.begin();

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

void loop() {
  
server.handleClient();
 
 digitalWrite(LEDv, !digitalRead(LEDv));
 delay(500);
 //ESP.deepSleep(5000000);
}

void SendDataACK () {
    if (!client.connected()) {
    reconnect();
  }
  client.publish("/sensor/test/ack", "ok");
  delay(100);
}
void programming(){
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
