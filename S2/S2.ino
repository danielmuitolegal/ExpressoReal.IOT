#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClientSecure wifi_client;
PubSubClient mqtt(wifi_client);

const int trig_1 = 12;
const int echo_1 = 13;
const int trig_2 = 14;
const int echo_2 = 15;
const byte LED_PIN = 18;

void setup() {
  Serial.begin(115200);

  wifi_client.setInsecure();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(trig_1, OUTPUT);
  pinMode(echo_1, INPUT);

  pinMode(trig_2, OUTPUT);
  pinMode(echo_2, INPUT);

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nWiFi conectado!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);
  mqtt.setCallback(callback);

  conectarMQTT();
}

void loop() {
  if (!mqtt.connected()) {
    conectarMQTT();
  }

  mqtt.loop();

  long dist_1 = lerDistancia(trig_1, echo_1);
  if (dist_1 > 0 && dist_1 < 10) {
    mqtt.publish(TOPICO_ULTRASSONICO1, "Presente");
  }

  long dist_2 = lerDistancia(trig_2, echo_2);
  if (dist_2 > 0 && dist_2 < 10) {
    mqtt.publish(TOPICO_ULTRASSONICO2, "Presente");
  }

  delay(200);
}

long lerDistancia(byte trig_pin, byte echo_pin) {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  long duracao = pulseIn(echo_pin, HIGH, 30000);

  if (duracao == 0) return -1;

  long distancia = duracao * 0.034 / 2;
  return distancia;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem = "";
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("TOPICO: ");
  Serial.println(topic);

  Serial.print("Mensagem: ");
  Serial.println(mensagem);

  if (String(topic) == TOPICO_LED_S2) {
    if (mensagem == "Acender") {
      digitalWrite(LED_PIN, HIGH);
    } else if (mensagem == "Apagar") {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void conectarMQTT() {
  Serial.print("Conectando ao Broker MQTT...");

  String clientID = "S2_" + String(random(0xffff), HEX);

  while (!mqtt.connected()) {
    if (mqtt.connect(clientID.c_str(), BROKER_USER, BROKER_PASS)) {
      Serial.println("\nConectado ao Broker!");
      mqtt.subscribe(TOPICO_LED_S2);

    } else {
      Serial.print(".");
      delay(500);
    }
  }
}
