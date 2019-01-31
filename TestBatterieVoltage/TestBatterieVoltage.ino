#include <ESP8266WiFi.h>
#include <PubSubClient.h>


ADC_MODE(ADC_VCC);

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.1.222";

char buf[20];

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
    delay(10);
    Serial.print(".");
  }

  //randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 2222);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("Voltage")) {
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
  int vcc = ESP.getVcc();
  Serial.println(vcc);
  const char* sendMessage = itoa (vcc, buf, 10);
  client.publish("/sensor/voltage", sendMessage);
  Serial.println("envoyé");
}
