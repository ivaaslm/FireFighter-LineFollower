#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LED_PIN D4   // LED onboard NodeMCU

// ===== WIFI =====
const char* ssid = "AAA";
const char* password = "12345678";

// ===== MQTT =====
const char* mqtt_server = "test.mosquitto.org";
const char* topic = "iot/fire_detection";

WiFiClient espClient;
PubSubClient client(espClient);

// ===== CALLBACK =====
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  if (msg == "ON") {
    digitalWrite(LED_PIN, LOW);   // LED onboard aktif LOW
  } else if (msg == "OFF") {
    digitalWrite(LED_PIN, HIGH);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("NodeMCU_FIRE")) {
      client.subscribe(topic);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}