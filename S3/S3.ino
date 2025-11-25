

//CONECTA O WIFI

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include "env.h"
#include <ESP32Servo.h>


// === Objetos de comunicação === //
WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);


// === Pinos === //
const int LED_PIN = 14;
const int SERVO1_PIN = 26;
const int SERVO2_PIN = 27;
const byte TRIGGER_PIN = 5;
const byte ECHO_PIN = 18;


// === Atuadores === //
Servo servo1;
Servo servo2;




//Essa parte conecta o ESP32 ao servidor MQTT e se inscreve nos tópicos. Se a conexão cair, essa função tenta reconectar automaticamente.

// ========================================= //
// ===========  CONEXÃO MQTT =============== //
// ========================================= //
void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Tentando conectar ao broker... ");
    String clientID = "ESP32_S3_Cliente_";
    clientID += String(random(0xffff), HEX);


    if (mqtt.connect(clientID.c_str())) {
      Serial.println("Conectado!");
      mqtt.subscribe(TOPIC_LED);
      mqtt.subscribe(TOPIC_SERVO_1);
      mqtt.subscribe(TOPIC_SERVO_2);
      mqtt.subscribe(TOPIC_ULTRASSONICO);
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(mqtt.state());
      Serial.println(" Tentando novamente...");
      delay(1000);
    }
  }
}


















//Essa função é chamada quando chega uma mensagem enviada pelo MQTT.
 //Se a mensagem for do LED, liga ou desliga.
 //Se for do servo, ele gira o motor para o ângulo enviado.”


// ========================================= //
// =============== CALLBACK ================= //
// ========================================= //
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }


  String top = String(topic);


  // Controle do LED
  if (top == TOPIC_LED) {
    digitalWrite(LED_PIN, msg == "Acender" ? HIGH : LOW); //aqui
  }


  // Servo 1
  if (top == TOPIC_SERVO_1) {
    int angulo = msg.toInt();
    servo1.write(angulo);
    mqtt.publish(TOPIC_SERVO_1, String(angulo).c_str());
  }


  // Servo 2
  if (top == TOPIC_SERVO_2) {
    int angulo = msg.toInt();
    servo2.write(angulo);
    mqtt.publish(TOPIC_SERVO_2, String(angulo).c_str());
  }}


//Essa parte envia um pulso pelo TRIGGER, mede o tempo que o eco leva para voltar e transforma isso em centímetros.
//Se a distância for menor que 10 cm, envia uma mensagem dizendo que há presença.”


// ========================================= //
// =========== LEITURA ULTRASSÔNICO ======== //
// ========================================= //
long lerDistancia() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);


  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);


  digitalWrite(TRIGGER_PIN, LOW);


  long duracao = pulseIn(ECHO_PIN, HIGH);
  long distancia = (duracao * 0.0343) / 2;  
  return distancia;
}




//O setup prepara tudo antes de o programa começar: configura pinos, inicia Wi-Fi, conecta ao MQTT e posiciona os servos.
// ========================================= //
// ================ SETUP ================== //
// ========================================= //
void setup() {
  Serial.begin(115200);




  // Pinos
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  // Servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);


  // Wi-Fi
  wifiClient.setInsecure();
  WiFi.begin(WIFI_SSID, WIFI_PASS);


  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");


  // MQTT
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  mqtt.setCallback(callback);


  reconnectMQTT();
}




//Aqui é onde o ESP32 roda o tempo todo:
//mantém o MQTT funcionando, lê o sensor e envia a mensagem quando detecta presença.”
// ========================================= //
// ================= LOOP ================== //
// ========================================= //
void loop() {
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();


  long distancia = lerDistancia();
  if (distancia < 10 && distancia > 0) {
    mqtt.publish(TOPIC_ULTRASSONICO, "90");
    Serial.println("Presença detectada!");
  }


  delay(200);
}












