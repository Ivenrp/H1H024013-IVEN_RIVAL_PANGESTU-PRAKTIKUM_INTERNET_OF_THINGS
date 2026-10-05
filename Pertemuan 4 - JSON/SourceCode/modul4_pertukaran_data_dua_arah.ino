// Percobaan 4B: publish suhu + subscribe perintah secara bersamaan (non-blocking)
// DHT11 rusak, jadi suhu memakai DATA DUMMY dari larik suhuDummy[]
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid          = "NAMA_WIFI_ANDA";
const char* password      = "PASSWORD_WIFI_ANDA";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;
const char* topicData     = "unsoed/tk245004/KelompokIvenRani/data";
const char* topicPerintah = "unsoed/tk245004/KelompokIvenRani/perintah";

const int ledPin = D2;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;   // publish setiap 5 detik (non-blocking)

// DATA DUMMY pengganti DHT11 (sensor rusak)
const float suhuDummy[] = {28.5, 28.7, 28.6, 28.9, 29.1, 29.0, 28.8, 29.2, 29.3, 29.1};
const int jumlahDummy = sizeof(suhuDummy) / sizeof(suhuDummy[0]);
int indeksDummy = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) pesan += (char)payload[i];

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return;   // abaikan jika parsing gagal

  const char* perintah = doc["perintah"] | "";
  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
  Serial.print("Perintah diterima -> Aktuator: ");
  Serial.println(perintah);
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
      Serial.println("Terhubung dan subscribe topic perintah");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();   // memproses pesan masuk secara terus-menerus

  // Publish data secara berkala tanpa memblokir proses subscribe
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = suhuDummy[indeksDummy];            // data dummy
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
