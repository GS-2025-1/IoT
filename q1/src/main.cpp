#define USE_THINGSPEAK true

#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#if USE_THINGSPEAK
  #include <WiFiClientSecure.h>
#endif


// Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ThingSpeak MQTT
const char* thingspeak_server = "mqtt3.thingspeak.com";
const int thingspeak_port = 8883;
const char* channelID = "2983641";
const char* mqttUser = "GSQqJAMqDSkXECkiMRsnOTQ";
const char* mqttPassword = "xtNw3istrmjtWHjB/YCqDvVF";
const char* thingspeak_topic = "channels/2983641/publish";

// ThingSpeak HTTP REST
const char* thingspeak_rest_url = "https://api.thingspeak.com/update";
const char* thingspeak_write_key = "MALOEIZ4RQDBXZGB";

// HiveMQ MQTT
const char* hivemq_server = "broker.hivemq.com";
const int hivemq_port = 1883;
const char* hivemq_topics[3] = {
  "/alagamenos/setor1",
  "/alagamenos/setor2",
  "/alagamenos/setor3"
};

// =========================

#if USE_THINGSPEAK
  WiFiClientSecure espClient;
#else
  WiFiClient espClient;
#endif

PubSubClient client(espClient);

// Sensores
const int triggerPins[3] = {12, 14, 27};
const int echoPins[3] = {13, 26, 25};

void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");

      if (client.connect(mqttUser, mqttUser, mqttPassword)) {
        Serial.println(" conectado ao ThingSpeak.");
      } else {
        Serial.print(" falhou, rc=");
        Serial.print(client.state());
        Serial.println(" tentando em 10s");
        delay(10000);
      }
  }
}

long readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  #if USE_THINGSPEAK
    espClient.setInsecure(); // permite conexão sem verificação de certificado
    client.setServer(thingspeak_server, thingspeak_port);
  #else
    client.setServer(hivemq_server, hivemq_port);
  #endif

  for (int i = 0; i < 3; i++) {
    pinMode(triggerPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }
}

void publishViaHttpREST(long distances[3]) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(thingspeak_rest_url) + "?api_key=" + thingspeak_write_key +
                 "&field1=" + String(distances[0]) +
                 "&field2=" + String(distances[1]) +
                 "&field3=" + String(distances[2]);

    Serial.print("Enviando dados via HTTP REST: ");
    Serial.println(url);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("Resposta do servidor: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro na requisição HTTP: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi não conectado!");
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  long distances[3];

  for (int i = 0; i < 3; i++) {
    distances[i] = readDistanceCM(triggerPins[i], echoPins[i]);
    Serial.print("Setor ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(distances[i]);
    Serial.println(" cm");
  }

  #if USE_THINGSPEAK
    // Monta payload com todos os fields juntos
    char payload[100];
    sprintf(payload, "field1=%ld&field2=%ld&field3=%ld", distances[0], distances[1], distances[2]);

    bool success = client.publish(thingspeak_topic, payload);

    if (success) {
      Serial.print("Payload enviado ao ThingSpeak via MQTT: ");
      Serial.println(payload);
    } else {
      Serial.println("FALHA na publicação MQTT! Tentando via HTTP REST...");
      publishViaHttpREST(distances);
    }

    delay(15000); // ThingSpeak recomenda 15 segundos mínimo
  #else
    // Publica cada distância em seu tópico no HiveMQ
    for (int i = 0; i < 3; i++) {
      char msg[20];
      sprintf(msg, "%ld", distances[i]);
      client.publish(hivemq_topics[i], msg);

      Serial.print("Publicado no HiveMQ topic ");
      Serial.print(hivemq_topics[i]);
      Serial.print(": ");
      Serial.println(msg);
    }

    delay(5000); // HiveMQ permite intervalo menor
  #endif
}
