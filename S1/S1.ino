#include <WiFi.h>          // Biblioteca para conectar o ESP32 ao Wi-Fi
#include <PubSubClient.h>  // Biblioteca para comunicação MQTT

// Cria um objeto que representa a conexão Wi-Fi
WiFiClient wifi_client;

// Cria um objeto MQTT usando a conexão Wi-Fi
PubSubClient mqtt(wifi_client);

// Nome e senha da rede Wi-Fi que o ESP32 vai se conectar
const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

// Endereço e porta do broker MQTT público
const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;
const String topic = "gatinhos";

// Usuário e senha do broker (aqui estão vazios porque esse broker é público)
const String brokerUser = "";
const String brokerPass = "";

void setup() {
  Serial.begin(115200);  // Inicia a comunicação serial (para ver mensagens no monitor serial)

  WiFi.begin(SSID, PASS);  // Tenta conectar o ESP32 ao Wi-Fi com o SSID e senha informados
  Serial.println("Conectando no WiFi...");

  // Espera até que a conexão Wi-Fi seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");  // Mostra pontos enquanto tenta conectar
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao WiFi!");

  // Configura o servidor MQTT (endereço e porta)
  mqtt.setServer(brokerURL.c_str(), brokerPort);

  // Cria um ID único para o cliente MQTT (evita conflito com outros dispositivos)
  String clientID = "S2";
  clientID += String(random(0xffff), HEX);

  Serial.println("Conectando ao Broker MQTT...");

  // Tenta conectar ao broker MQTT
  while (mqtt.connect(clientID.c_str()) == 0) {
    Serial.print(".");  // Mostra pontos enquanto tenta conectar
    delay(2000);
  }
  mqtt.subscribe(topic.c_str());
  mqtt.setCallback(callback);
  Serial.println("\nConectado ao broker MQTT!");
}

void loop() {
  // Aqui vai o código que será executado continuamente
  // (por exemplo, publicar ou receber mensagens MQTT)
  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    mensagem = "sara: " + mensagem;
    mqtt.publish("bob", mensagem.c_str());
    mqtt.publish("macaco", mensagem.c_str());
    mqtt.publish("felpz", mensagem.c_str());
  }
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned long length) {
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    //Pega cada letra de payload e junta na mensagem
    MensagemRecebida += (char)payload[i];
  }
  Serial.println(MensagemRecebida);
}
