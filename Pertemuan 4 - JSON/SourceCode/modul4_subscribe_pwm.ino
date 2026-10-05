// Tugas tambahan 4A: kecerahan LED diatur lewat "intensitas" (PWM) pada JSON
// Contoh pesan: {"perintah":"ON","intensitas":200}
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;
const char* topicPerintah = "unsoed/tk245004/KelompokIvenRani/perintah";

const int ledPin = D2;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* perintah = doc["perintah"] | "";       // "" jika key tidak ada
  int intensitas = doc["intensitas"] | 255;          // 255 (terang penuh) jika tidak dikirim
  intensitas = constrain(intensitas, 0, 255);        // batasi ke rentang PWM

  if (strcmp(perintah, "ON") == 0) {
    analogWrite(ledPin, intensitas);                 // PWM sesuai intensitas
    Serial.print("Aktuator: ON, intensitas = ");
    Serial.println(intensitas);
  } else if (strcmp(perintah, "OFF") == 0) {
    analogWrite(ledPin, 0);
    Serial.println("Aktuator: OFF");
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWriteRange(255);   // PWM 0-255
  analogWrite(ledPin, 0);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();
}
