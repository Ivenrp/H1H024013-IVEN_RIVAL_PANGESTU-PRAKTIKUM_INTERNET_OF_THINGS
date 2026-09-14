# Jawaban Modul Praktikum 2 — Konfigurasi Jaringan (WiFi Mode Station & Access Point pada ESP32)

**Nama:** Iven Rival Pangestu
**NIM:** H1H024013
**Shift Awal:** A 
**Shift Akhir:** A 

---

## Bagian 2.5.4 — Pertanyaan Percobaan 2A (Mode Station)

### Soal 1
Gambarkan diagram alur (flowchart) proses koneksi ESP32 ke jaringan WiFi pada program di atas!


**Jawaban:**
Diagram alur proses koneksi ESP32 mode Station:

1. **Mulai (start)**
2. **Set mode Station** — `WiFi.mode(WIFI_STA)`
3. **Mulai koneksi** — `WiFi.begin(ssid, password)`
4. **Cek status koneksi** — `WiFi.status() == WL_CONNECTED?`
   - Jika **belum terhubung** → tunggu 500 ms, cetak `"."` ke Serial Monitor, lalu kembali ke langkah cek status (loop).
   - Jika **sudah terhubung** → lanjut ke langkah berikutnya.
5. **Tampilkan info** — cetak IP Address, MAC Address, RSSI, dan nyalakan LED indikator.
6. **Pantau status** — dalam `loop()`, cek status koneksi setiap 5 detik dan tampilkan "Terhubung" atau "Terputus".

<img width="502" height="642" alt="flowchart_koneksi_wifi_sta" src="https://github.com/user-attachments/assets/a1dfc660-efe4-4d9e-8d53-18b7f6d303cd" />

---

### Soal 2
Apa fungsi dari perintah `WiFi.mode(WIFI_STA)` pada program tersebut?

**Jawaban:**
Perintah ini mengatur mode operasi radio WiFi ESP32 menjadi Station (klien), artinya ESP32 akan bertindak sebagai perangkat yang mencari dan bergabung ke access point/router yang sudah ada, bukan membuat jaringannya sendiri. Perintah ini harus dipanggil sebelum `WiFi.begin()` agar chip mengetahui peran apa yang harus dijalankan (STA, bukan AP atau AP+STA).

---

### Soal 3
Jelaskan apa yang terjadi apabila SSID atau password yang dimasukkan salah!

**Jawaban:**
`WiFi.begin(ssid, password)` akan tetap dieksekusi, tetapi ESP32 tidak akan pernah berhasil melewati status `WL_CONNECTED`:

- Jika SSID tidak ditemukan → status tetap `WL_NO_SSID_AVAIL`.
- Jika SSID ada tapi password salah → ESP32 mencoba autentikasi lalu gagal, status berputar antara `WL_IDLE_STATUS`/`WL_CONNECT_FAILED` dan tidak pernah menjadi `WL_CONNECTED`.
- Karena program asli memakai `while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }`, loop ini akan berjalan **selamanya** (infinite loop): Serial Monitor hanya terus mencetak titik tanpa pernah mencapai bagian "WiFi berhasil terhubung!", dan LED indikator tidak akan menyala.

---

### Soal 4
Modifikasi program agar ESP32 mencoba menghubungkan ulang (reconnect) secara otomatis apabila koneksi WiFi terputus, dan berikan penjelasan di setiap baris kode yang ditambahkan dalam bentuk README.md!

**Jawaban — Kode program (`modul2_konfigurasi_jaringan_sta_reconnect.ino`):**

```cpp
#include <WiFi.h>

const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const int ledPin = 2;

unsigned long previousMillis = 0;
const long reconnectInterval = 5000; // cek tiap 5 detik

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    digitalWrite(ledPin, HIGH);
  } else {
    Serial.println();
    Serial.println("Gagal terhubung, akan dicoba lagi otomatis.");
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  connectWiFi();
}

void loop() {
  unsigned long currentMillis = millis();

  if (WiFi.status() != WL_CONNECTED &&
      currentMillis - previousMillis >= reconnectInterval) {
    previousMillis = currentMillis;
    Serial.println("Koneksi terputus, mencoba reconnect...");
    connectWiFi();
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledPin, HIGH);
  }
}
```

**README.md untuk modifikasi ini:**

```markdown
# Modifikasi Auto-Reconnect WiFi ESP32

## Perubahan dari kode dasar
1. `connectWiFi()` — logika koneksi dipindah ke fungsi terpisah supaya bisa
   dipanggil ulang kapan saja (saat setup maupun saat reconnect), tanpa
   duplikasi kode.
2. `attempt` dengan batas 20 percobaan — mencegah `while` asli terjebak
   infinite loop selamanya jika SSID/password salah; setelah 20x gagal,
   program lanjut ke `loop()` dan mencoba lagi secara berkala, bukan
   berhenti total.
3. `previousMillis` + `reconnectInterval` (non-blocking timer dengan
   `millis()`) — dipakai di `loop()` agar ESP32 tidak "diam" (blocking
   dengan `delay()`) menunggu koneksi, sehingga tugas lain tetap bisa
   berjalan sambil menunggu reconnect.
4. Pengecekan `WiFi.status() != WL_CONNECTED` di `loop()` — jika koneksi
   terputus, dan sudah lewat 5 detik sejak percobaan terakhir,
   `connectWiFi()` dipanggil lagi.
5. LED dikendalikan berdasarkan status koneksi terkini di setiap iterasi
   `loop()`, bukan hanya sekali di `setup()`, sehingga LED mati otomatis
   saat WiFi putus.
```

---

## Bagian 2.6.4 — Pertanyaan Percobaan 2B (Mode Access Point)

### Soal 1
Mengapa alamat IP default Access Point pada ESP32 umumnya bernilai 192.168.4.1?

**Jawaban:**
Ini adalah nilai default yang ditetapkan oleh library `WiFi.h`/SDK ESP-IDF Espressif untuk mode Access Point. Alamat `192.168.x.x` adalah blok IP privat (sesuai RFC 1918) yang umum dipakai jaringan lokal, dan Espressif memilih subnet `192.168.4.0/24` dengan gateway `192.168.4.1` sebagai konvensi bawaan agar setiap ESP32 yang dijadikan AP langsung memiliki alamat yang konsisten dan bisa diprediksi tanpa perlu konfigurasi DHCP tambahan. Nilai ini dapat diubah manual dengan `WiFi.softAPConfig()` jika diperlukan.

---

### Soal 2
Apa perbedaan mendasar antara mode Station dan mode Access Point pada ESP32?

**Jawaban:**

| Aspek | Mode Station (STA) | Mode Access Point (AP) |
|---|---|---|
| Peran | Klien, bergabung ke jaringan yang sudah ada | Penyedia jaringan (host hotspot) |
| Butuh router eksternal? | Ya | Tidak |
| Fungsi utama | `WiFi.begin(ssid, pass)` | `WiFi.softAP(ssid, pass)` |
| IP yang didapat | Dari DHCP router (`WiFi.localIP()`) | IP yang dibuat sendiri (`WiFi.softAPIP()`, default 192.168.4.1) |
| Contoh penggunaan | ESP32 mengakses internet/server | Konfigurasi awal perangkat via halaman web |

---

### Soal 3
Jelaskan risiko keamanan apabila password Access Point tidak diberikan atau terlalu sederhana!

**Jawaban:**
- Jika tanpa password (open network), siapa pun dalam jangkauan sinyal bisa terhubung tanpa izin, membuka celah untuk mengakses layanan web/API yang berjalan di ESP32, menyusupkan data palsu, atau melakukan serangan man-in-the-middle.
- Password lemah (misalnya `12345678` atau kata umum) mudah ditebak lewat brute-force atau serangan kamus, apalagi perangkat WiFi rumahan/IoT jarang memiliki sistem lockout setelah gagal login berkali-kali.
- Karena AP ESP32 sering dipakai untuk tahap provisioning (memasukkan kredensial WiFi rumah), jika direbut orang tak berwenang, kredensial WiFi utama bisa dicuri, atau perangkat bisa "dibajak" untuk dikonfigurasi ulang ke jaringan milik penyerang.
- Untuk IoT yang mengontrol aktuator (relay, motor, dsb.), akses tak sah ke AP bisa berujung pada pengendalian fisik yang berbahaya.

---

### Soal 4
Modifikasi program agar ESP32 berjalan pada mode AP+STA (terhubung ke WiFi rumah sekaligus menyediakan Access Point), dan berikan penjelasan di setiap baris kodenya dalam bentuk README.md!

**Jawaban — Kode program (`modul2_konfigurasi_jaringan_apsta.ino`):**

```cpp
#include <WiFi.h>

// Kredensial WiFi rumah (mode Station)
const char* sta_ssid     = "NAMA_WIFI_ANDA";
const char* sta_password = "PASSWORD_WIFI_ANDA";

// Kredensial Access Point ESP32
const char* ap_ssid     = "ESP32_AccessPoint";
const char* ap_password = "12345678";

void setup() {
  Serial.begin(115200);

  // Set mode gabungan AP + STA
  WiFi.mode(WIFI_AP_STA);

  // Aktifkan Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("Access Point aktif, IP: ");
  Serial.println(WiFi.softAPIP());

  // Sambungkan sebagai Station ke WiFi rumah
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi rumah");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Terhubung ke WiFi rumah, IP Station: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Perangkat terhubung ke AP: ");
  Serial.println(jumlahClient);
  delay(5000);
}
```

**README.md untuk modifikasi AP+STA:**

```markdown
# Modifikasi Mode AP+STA

## Penjelasan baris kode
1. `sta_ssid`/`sta_password` dan `ap_ssid`/`ap_password` — dua set
   kredensial disiapkan terpisah karena ESP32 kini menjalankan dua peran
   sekaligus.
2. `WiFi.mode(WIFI_AP_STA)` — mengaktifkan kedua mode radio (AP dan STA)
   secara bersamaan, berbeda dari `WIFI_STA` atau `WIFI_AP` saja.
3. `WiFi.softAP(ap_ssid, ap_password)` dipanggil lebih dulu — membuat
   ESP32 langsung bisa diakses sebagai hotspot walau proses koneksi ke
   WiFi rumah belum selesai.
4. `WiFi.softAPIP()` dan `WiFi.localIP()` dicetak terpisah — untuk
   membedakan alamat IP dari sisi AP (default 192.168.4.1) dan alamat IP
   dari sisi STA (didapat dari router rumah).
5. `loop()` hanya memantau jumlah client yang terhubung ke AP; koneksi
   STA sudah stabil sejak `setup()` sehingga tidak perlu dicek ulang di
   sini.

## Kegunaan skenario nyata
Mode ini cocok untuk provisioning: pengguna terhubung ke hotspot ESP32
untuk memasukkan kredensial WiFi rumah lewat halaman web, sementara ESP32
tetap bisa terhubung ke internet/server melalui koneksi STA yang sudah
ada.
```

---

## Bagian 2.7 — Pertanyaan Praktikum Umum (Bagian Hasil dan Analisis)

### Soal 1
Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!

**Jawaban:**
Pada Percobaan 2A, ESP32 berhasil dikonfigurasi pada mode Station dan terhubung ke jaringan WiFi yang telah ditentukan, ditandai dengan tampilnya IP Address, MAC Address, dan nilai RSSI yang stabil pada Serial Monitor, serta LED indikator yang menyala sesuai spesifikasi. Pengujian dengan kredensial salah (password salah maupun SSID salah) menunjukkan bahwa ESP32 tidak pernah mencapai status terhubung dan terus mencoba tanpa henti, yang mengonfirmasi pentingnya validasi kredensial dan mekanisme timeout. Pada Percobaan 2B, ESP32 berhasil dikonfigurasi sebagai Access Point mandiri dengan SSID ESP32_AccessPoint dan alamat IP default 192.168.4.1, serta mampu memantau jumlah perangkat yang terhubung secara real-time melalui Serial Monitor, sesuai dengan seluruh spesifikasi yang diharapkan pada modul
---

### Soal 2
Bagaimana pengaruh kekuatan sinyal (RSSI) terhadap kestabilan koneksi WiFi pada perangkat IoT?

**Jawaban:**
RSSI (Received Signal Strength Indicator, satuan dBm, nilainya negatif) menunjukkan kekuatan sinyal yang diterima ESP32 dari access point:
- Semakin mendekati 0 (misalnya -30 dBm) → sinyal sangat kuat, koneksi stabil, throughput tinggi, latensi rendah.
- Semakin jauh dari 0 (misalnya -80 dBm hingga -90 dBm) → sinyal lemah, rentan putus-nyambung (disconnect), paket data sering hilang (packet loss), throughput menurun, dan delay makin besar.
- Untuk aplikasi IoT yang mengirim data sensor secara berkala, RSSI yang buruk bisa menyebabkan data terlambat terkirim atau gagal terkirim sama sekali, sehingga penempatan fisik ESP32 relatif terhadap router perlu diperhatikan.

---

### Soal 3
Bagaimana cara kerja ESP32 dalam membedakan peran sebagai klien (Station) dan sebagai penyedia jaringan (Access Point)?

**Jawaban:**
Secara internal, chip WiFi ESP32 memiliki state machine yang dikendalikan oleh parameter mode (`WIFI_STA`, `WIFI_AP`, atau `WIFI_AP_STA`) yang diset lewat `WiFi.mode()`. Mode ini menentukan:
- Interface mana yang aktif (station interface vs softAP interface — keduanya memiliki alamat MAC berbeda meski berada dalam satu chip fisik).
- Perilaku pada layer manajemen 802.11: sebagai STA, ESP32 mengirim *probe request* dan melakukan proses *authentication/association* ke AP target; sebagai AP, ESP32 justru menjawab *probe request* dari perangkat lain dan menyediakan *beacon frame* secara berkala untuk mengumumkan keberadaan jaringannya.
- Stack jaringan (lwIP) di ESP32 menjalankan DHCP client saat STA (meminta IP dari router) dan DHCP server saat AP (memberi IP ke perangkat yang terhubung).

---

### Soal 4
Bagaimana kombinasi mode Station dan Access Point (AP+STA) dapat dimanfaatkan dalam skenario nyata sistem IoT, misalnya pada proses konfigurasi awal perangkat (provisioning)?

**Jawaban:**
Skema ini sangat umum pada perangkat IoT komersial (misalnya smart plug, smart bulb): saat pertama kali dinyalakan dan belum mengetahui kredensial WiFi rumah, perangkat otomatis membuka mode AP agar pengguna bisa menyambungkan HP-nya langsung ke perangkat tersebut, lalu membuka halaman web konfigurasi untuk memasukkan SSID/password WiFi rumah. Setelah kredensial disimpan, perangkat beralih memakai mode STA untuk tersambung ke internet, sementara mode AP bisa tetap aktif sebagai jalur cadangan (misalnya untuk reset konfigurasi tanpa harus factory reset fisik). Kombinasi ini menghindari kebutuhan hardcode kredensial WiFi di source code dan membuat proses setup jauh lebih ramah pengguna.
