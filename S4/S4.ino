#include <WiFi.h>              // Biblioteca para conectar o ESP32 ao Wi-Fi
#include <PubSubClient.h>      // Biblioteca para comunicação MQTT

#include "env.h"
void setup() {
  Serial.begin(115200);             // Inicia a comunicação serial (para ver mensagens no monitor serial)

  WiFi.begin( WIFI_SSID , WIFI_PASS);           // Tenta conectar o ESP32 ao Wi-Fi com o SSID e senha informados
  Serial.println("Conectando no WiFi...");
  
  // Espera até que a conexão Wi-Fi seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");              // Mostra pontos enquanto tenta conectar
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao WiFi!");

  // Configura o servidor MQTT (endereço e porta)
  mqtt.setServer(BROKER_URL, BROKER_PORT);

  // Cria um ID único para o cliente MQTT (evita conflito com outros dispositivos)
  String clientID = "S2";
  clientID += String(random(0xffff), HEX);

  Serial.println("Conectando ao Broker MQTT...");

  // Tenta conectar ao broker MQTT
  while (mqtt.connect(clientID.c_str(),BROKER_USD_ID, BROKER_PASS_USR_PASS) == 0) {
    Serial.print(".");               // Mostra pontos enquanto tenta conectar
    delay(2000);
  }

  mqtt.subscribe(TOPIC_VELOCIDADE);
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker MQTT!");
}

void loop() {
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length){
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++){
    //Pega cada letra de payload e junta na mensagem
    MensagemRecebida += payload[i];

  }
  Serial.println(MensagemRecebida);



}