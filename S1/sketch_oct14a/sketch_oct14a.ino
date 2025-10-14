#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifi_client;
PubSubClient mqtt(wifi_client);

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;
const String topic = "macaco";

const String brokerUser = "";
const String brokerPass = "";

// Protótipo da função callback
void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);

  WiFi.begin(SSID, PASS);
  Serial.println("Conectando no WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso ao WiFi!");

  mqtt.setServer(brokerURL.c_str(), brokerPort);

  String clientID = "S1";
  clientID += String(random(0xffff), HEX);

  Serial.println("Conectando ao Broker MQTT...");
  while (!mqtt.connect(clientID.c_str(), brokerUser.c_str(), brokerPass.c_str())) {
    Serial.print(".");
    delay(2000);
  }

  mqtt.setCallback(callback);   // Define o callback antes de assinar o tópico
  mqtt.subscribe(topic.c_str());
  Serial.println("\nConectado ao broker MQTT!");
}

void loop() {
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n');
    mensagem = "daniel: " + mensagem;
    mqtt.publish("gatinhos", mensagem.c_str());
    mqtt.publish("bob", mensagem.c_str());
    mqtt.publish("felpz", mensagem.c_str());
    Serial.println(mensagem);
  }
  mqtt.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String MensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    MensagemRecebida += (char)payload[i];
  }
  Serial.println(MensagemRecebida);
}