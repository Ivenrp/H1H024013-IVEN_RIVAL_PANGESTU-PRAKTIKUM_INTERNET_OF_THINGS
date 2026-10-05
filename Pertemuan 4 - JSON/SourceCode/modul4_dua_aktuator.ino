// Tugas tambahan 4B: dua aktuator (LED dan buzzer) dengan topic perintah terpisah
// callback() membedakan sumber pesan lewat argumen topic
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;
const char* topicData     = "unsoed/tk245004/KelompokIvenRani/data";
const char* topicPerintah = "unsoed/tk245004/KelompokIvenRani/perintah";  // LED
const char* topicBuzzer   = "unsoed/tk245004/KelompokIvenRani/buzzer";    // buzzer

const int ledPin    = D2;
const int buzzerPin = D5;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

// DATA DUMMY pengganti DHT11 (sensor rusak)
const float suhuDummy[] = {28.5, 28.7, 28.6, 28.9, 29.1, 29.0, 28.8, 29.2, 29.3, 29.1};
const int jumlahDummy = sizeof(suhuDummy) / sizeof(suhuDummy[0]);
int indeksDummy = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) pesan += (char)payload[i];

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return;
  const char* perintah = doc["perintah"] | "";
  bool aktif = (strcmp(perintah, "ON") == 0);

  if (strcmp(topic, topicPerintah) == 0) {
    digitalWrite(ledPin, aktif ? HIGH : LOW);
    Serial.print("LED -> ");
    Serial.println(perintah);
  } else if (strcmp(topic, topicBuzzer) == 0) {
    digitalWrite(buzzerPin, aktif ? HIGH : LOW);
    Serial.print("Buzzer -> ");
    Serial.println(perintah);
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(topicPerintah);
      client.subscribe(topicBuzzer);
      Serial.println("Terhubung dan subscribe topic LED + buzzer");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();

  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = suhuDummy[indeksDummy];
    indeksDummy = (indeksDummy + 1) % jumlahDummy;

    JsonDocument doc;
    doc["suhu"] = suhu;
    char buffer[128];
    serializeJson(doc, buffer);
    client.publish(topicData, buffer);

    Serial.print("Data terkirim: ");
    Serial.println(buffer);
  }
}
