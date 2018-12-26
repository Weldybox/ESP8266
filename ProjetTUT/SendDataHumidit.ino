#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int data = A0;
char buf[20];
int resultatCurrentData = 0;
int iteration = 3600;

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.111";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 2222);
  pinMode(data,OUTPUT);
}

/*
 * 
 */void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Vicky")) {
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
  if (!client.connected()) {
    reconnect();
  }
  SendData(450, 800);
}

void SendData (int valMin, int valMax) {
  int difference = valMax - valMin;
  resultatCurrentData = 100-(((analogRead(data)-valMin)*100)/difference);
  const char* sendMessage = itoa (resultatCurrentData, buf, 10);
  client.publish("/sensor/plante/humidity", sendMessage);
  Serial.println("envoyé");
  ESP.deepSleep(iteration*1000000);
}
