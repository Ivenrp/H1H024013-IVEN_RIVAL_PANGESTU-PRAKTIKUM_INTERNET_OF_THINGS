#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // Tambahan wajib untuk HTTPS di ESP8266
#include <ArduinoJson.h>

const char* ssid = "vivo Y17s";
const char* password = "Aditiaaaa";
const char* serverUrl = "https://httpbin.org/post"; // endpoint uji HTTP POST

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client; 
    client.setInsecure(); // Mengabaikan verifikasi sertifikat SSL agar praktis

    HTTPClient http;
    // ESP8266 butuh argumen 'client' disisipkan sebelum serverUrl
    http.begin(client, serverUrl); 
    http.addHeader("Content-Type", "application/json");
    
    // Membuat objek data sensor dalam format JSON
    JsonDocument doc;
    doc["suhu"] = 28.5;       // contoh data suhu (°C)
    doc["kelembaban"] = 65.0; // contoh data kelembaban (%)
    String requestBody;
    serializeJson(doc, requestBody);
    
    Serial.print("Mengirim data: ");
    Serial.println(requestBody);
    
    // Mengirim data melalui HTTP POST
    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
      Serial.print("Kode Response HTTP: ");
      Serial.println(httpResponseCode);
      Serial.println("Isi Response:");
      Serial.println(http.getString());
    } else {
      Serial.print("Pengiriman gagal, kode error: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  delay(10000); // kirim data setiap 10 detik
}
