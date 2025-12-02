#include <WiFi.h>          // Biblioteca para conectar o ESP32 ao Wi-Fi
#include <PubSubClient.h>  // Biblioteca para comunicação MQTT
#include <WiFiClientSecure.h>
#include "env.h"
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

// Cria um objeto que representa a conexão Wi-Fi
WiFiClientSecure wifi_client;

// Cria um objeto MQTT usando a conexão Wi-Fi
PubSubClient mqtt(wifi_client);

/*
------------------------------
          VARIÁVEIS
------------------------------
*/

// LED
int r = 14;
int g = 26;
int b = 25;
int led = 19;

// DHT (umidade)
DHT dhtSensor(DHTPIN, DHTTYPE);

// LDR (iluminação)
int ldr = 34;

// ECHO (presença)
int echo = 23;
int trig = 22;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  msg.trim();
  Serial.print("Mensagem do BROKER: ");
  Serial.println(msg);

  if (msg == "Acender") {
    digitalWrite(led, HIGH);
  }
  if (msg == "Desligar") {
    digitalWrite(led, LOW);
  }
}

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial (para ver mensagens no monitor serial)
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(led, OUTPUT);

  pinMode(ldr, INPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  dhtSensor.begin();

  wifi_client.setInsecure();
  Serial.println("Conectando no WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);
  mqtt.setCallback(callback);

  mqttConnect();

  Serial.println("Setup finalizado");
}

void loop() {
   if (!mqtt.connected()) {
    mqttConnect();
  }
  mqtt.loop();

  // ---------------------------
  // LDR: leitura e automação
  // ---------------------------
  int ldrValue = analogRead(ldr);
  mqtt.publish(TOPIC_ILUM, String(ldrValue).c_str());

  // Limite de luz (ajusta se necessário)
  int limite = 2000; // quanto menor = mais escuro

  if (ldrValue < limite) {
    // Escuro
    digitalWrite(led, HIGH);
  } else {
    // Claro
    digitalWrite(led, LOW);
  }

  // ---------------------------
  // DHT11
  // ---------------------------
  float umid = dhtSensor.readHumidity();
  float temp = dhtSensor.readTemperature();

  if (!isnan(umid))
    mqtt.publish(TOPIC_UMID, String(umid).c_str());

  if (!isnan(temp))
    mqtt.publish(TOPIC_TEMP, String(temp).c_str());

  // ---------------------------
  // Ultrassônico
  // ---------------------------
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duracao = pulseIn(echo, HIGH);
  long distancia = duracao * 0.034 / 2;

  mqtt.publish(TOPIC_PRESENCA, String(distancia).c_str());

  delay(500);
}


void mqttConnect() {
  Serial.print("Conectando ao broker MQTT... ");

  String clientId = "ESP32_";
  clientId += WiFi.macAddress();
  clientId.replace(":", "");

  while (!mqtt.connected()) {
    if (mqtt.connect(clientId.c_str(), BROKER_USD_ID, BROKER_PASS_USR_PASS)) {
      Serial.println("conectado!");
      // nenhum subscribe aqui
    } else {
      Serial.print("falhou, rc=");
      Serial.println(mqtt.state());
      Serial.println(" tentando novamente em 2s...");
      delay(2000);
    }
  }
}
