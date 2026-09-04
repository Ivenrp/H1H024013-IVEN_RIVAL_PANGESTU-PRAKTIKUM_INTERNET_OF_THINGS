## Identitas

- **Nama:** Iven Rival Pangestu
- **NIM:** H1H024013
- **Shift:** A

---

## 1. Tujuan Praktikum

Praktikum ini bertujuan untuk memahami penggunaan sensor dan aktuator pada sistem mikrokontroler, khususnya:

1. Membaca data suhu dari sensor DHT11.
2. Menampilkan hasil pembacaan sensor melalui Serial Monitor.
3. Mengendalikan modul relay berdasarkan nilai ambang batas suhu (*threshold*).
4. Memahami validasi data sensor menggunakan `isnan()`.
5. Menerapkan logika histeresis untuk mencegah *chattering* pada relay.

---

## 2. Penjelasan Program

Program berikut mengintegrasikan sensor suhu DHT11 dengan modul relay sebagai aktuator. Relay dikendalikan berdasarkan ambang batas suhu sebesar **30 °C**.

```cpp
#include <DHT.h>

#define DHTPIN 4          // Pin data DHT terhubung ke GPIO 4 
#define DHTTYPE DHT11     // Tipe sensor yang digunakan
#define RELAYPIN 7       // Pin kontrol relay terhubung ke GPIO 7 

DHT dht(DHTPIN, DHTTYPE);

const float suhuThreshold = 30.0; // Batas ambang suhu (°C)

void setup() {
  Serial.begin(115200);           // Inisialisasi komunikasi serial
  pinMode(DHTPIN, INPUT_PULLUP);  // Pin DHT sebagai input dengan pull-up
  dht.begin();                    // Mengaktifkan sensor DHT
  pinMode(RELAYPIN, OUTPUT);      // Pin relay sebagai output
  digitalWrite(RELAYPIN, LOW);    // Kondisi awal relay OFF
}

void loop() {
  delay(2000);                    // Pembacaan setiap 2 detik

  float suhu = dht.readTemperature();

  // Validasi pembacaan sensor
  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // Kendali relay berdasarkan threshold
    if (suhu > suhuThreshold) {
      digitalWrite(RELAYPIN, HIGH);
      Serial.println("Aktuator: ON");
    } else {
      digitalWrite(RELAYPIN, LOW);
      Serial.println("Aktuator: OFF");
    }
  }
}
```

### Alur Kerja Program

```text
        ┌──────────────────┐
        │      START       │
        └────────┬─────────┘
                 │
                 ▼
        ┌──────────────────┐
        │ Inisialisasi     │
        │ DHT + Relay      │
        └────────┬─────────┘
                 │
                 ▼
        ┌──────────────────┐
        │ Baca suhu DHT11  │
        └────────┬─────────┘
                 │
                 ▼
          ┌─────────────┐
          │ Data valid? │
          └──────┬──────┘
             Ya  │  Tidak
                 │
       ┌─────────┘ └──────────────┐
       ▼                          ▼
┌───────────────┐       ┌─────────────────┐
│ Bandingkan    │       │ Tampilkan error │
│ suhu > 30 °C? │       └────────┬────────┘
└───────┬───────┘                │
    Ya  │  Tidak                 │
        │                        │
    ┌───┴───┐                    │
    ▼       ▼                    │
 Relay ON  Relay OFF             │
    │       │                    │
    └───┬───┘                    │
        └──────────┬─────────────┘
                   ▼
             ┌──────────┐
             │ Ulangi   │
             └──────────┘
```

---

## 3. Fungsi-Fungsi yang Digunakan

| Fungsi | Keterangan |
|---|---|
| `setup()` | Dijalankan satu kali saat mikrokontroler mulai untuk melakukan inisialisasi. |
| `loop()` | Fungsi utama yang dijalankan berulang-ulang. |
| `Serial.begin(115200)` | Mengatur komunikasi serial dengan baud rate 115200 bps. |
| `Serial.print()` | Menampilkan data pada Serial Monitor tanpa pindah baris. |
| `Serial.println()` | Menampilkan data pada Serial Monitor kemudian pindah baris. |
| `pinMode()` | Menentukan fungsi GPIO sebagai input atau output. |
| `digitalWrite()` | Mengatur level logika GPIO menjadi `HIGH` atau `LOW`. |
| `dht.begin()` | Menginisialisasi sensor DHT. |
| `dht.readTemperature()` | Membaca nilai suhu dari sensor dalam °C. |
| `isnan()` | Memeriksa apakah nilai pembacaan merupakan `NaN` (*Not a Number*). |
| `delay()` | Memberikan jeda eksekusi dalam satuan milidetik. |

> **Catatan:** DHT11 memiliki batas kecepatan pembacaan sehingga jeda sekitar 2 detik digunakan agar pembacaan sensor lebih stabil.

---

## 4. Percabangan (*Conditional Statement*)

### 4.1 Validasi Data Sensor

```cpp
if (isnan(suhu)) {
  Serial.println("Gagal membaca data sensor!");
} else {
  // Memproses data sensor
}
```

Mikrokontroler terlebih dahulu memeriksa apakah nilai suhu berhasil dibaca.

- Jika `isnan(suhu)` bernilai **TRUE**, pembacaan dianggap gagal dan sistem menampilkan pesan error.
- Jika bernilai **FALSE**, nilai suhu valid sehingga dapat digunakan untuk mengendalikan relay.

Validasi ini penting agar sistem tidak mengambil keputusan berdasarkan data sensor yang tidak valid.

### 4.2 Kendali Berdasarkan Threshold

```cpp
if (suhu > suhuThreshold) {
  digitalWrite(RELAYPIN, HIGH);
  Serial.println("Aktuator: ON");
} else {
  digitalWrite(RELAYPIN, LOW);
  Serial.println("Aktuator: OFF");
}
```

Dengan:

```cpp
const float suhuThreshold = 30.0;
```

Maka:

| Kondisi Suhu | Kondisi Logika | Relay |
|---|---:|---|
| `suhu > 30 °C` | TRUE | ON |
| `suhu <= 30 °C` | FALSE | OFF |

Contoh:

- Suhu **32 °C** → Relay **ON**
- Suhu **30.5 °C** → Relay **ON**
- Suhu **30 °C** → Relay **OFF**
- Suhu **28.9 °C** → Relay **OFF**

---

## 5. Library yang Digunakan

Program menggunakan library berikut:

- **DHT sensor library** oleh Adafruit — versi 1.4.x atau terbaru.
- **Adafruit Unified Sensor** — dependency pendukung untuk library sensor Adafruit.

Pastikan kedua library telah terpasang sebelum melakukan proses kompilasi program.

---

## 6. Board yang Digunakan

Board yang dapat digunakan:

- **NodeMCU 1.0 (ESP-12E Module)** / ESP8266
- **ESP32 Dev Module**

### Pin yang Digunakan

| Komponen | GPIO | NodeMCU | Fungsi |
|---|---:|---|---|
| DHT11 Data | GPIO 4 | D2 | Input data sensor |
| Relay | GPIO 14 | D5 | Output kontrol aktuator |

> **Catatan:** Pastikan logika `HIGH/LOW` sesuai dengan jenis modul relay yang digunakan. Beberapa modul relay bersifat **active-low**, sehingga `LOW` justru dapat berarti ON.

---

# 7. Modifikasi Program — Logika Histeresis

## 7.1 Konsep Histeresis

Pada kontrol threshold tunggal, relay dapat mengalami **chattering**, yaitu kondisi ON/OFF secara cepat ketika suhu berada di sekitar nilai ambang.

Untuk mengatasi masalah tersebut digunakan **histeresis** dengan dua ambang:

- **Batas atas = 30 °C** → Aktuator ON.
- **Batas bawah = 28 °C** → Aktuator OFF.
- **28–30 °C** → Aktuator mempertahankan kondisi sebelumnya.

Dengan demikian, relay tidak perlu berpindah kondisi hanya karena perubahan suhu kecil.

### Zona Histeresis

```text
Suhu naik
   │
   ▼
> 30 °C ───────────────► RELAY ON
                          │
                          │
                     28–30 °C
                     DEAD BAND
                          │
                          │
< 28 °C ───────────────► RELAY OFF
   ▲
   │
Suhu turun
```

---

## 7.2 Program Histeresis (Modifikasi)

```cpp
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define RELAYPIN 14

DHT dht(DHTPIN, DHTTYPE);

// Dua ambang batas kendali histeresis
const float suhuAtas = 30.0;   // Ambang atas (°C) -> Aktuator ON
const float suhuBawah = 28.0;  // Ambang bawah (°C) -> Aktuator OFF

void setup() {
  Serial.begin(115200);
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
}

void loop() {
  delay(2000);

  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" °C | ");

  // Logika histeresis
  if (suhu > suhuAtas) {
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Status Aktuator: ON (Suhu melampaui batas atas 30°C)");
  } else if (suhu < suhuBawah) {
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Status Aktuator: OFF (Suhu turun di bawah batas bawah 28°C)");
  } else {
    Serial.println("Status Aktuator: Mempertahankan kondisi sebelumnya (Zona Deadband)");
  }
}
```

### Tabel Logika Histeresis

| Suhu | Keputusan |
|---|---|
| `> 30 °C` | Relay ON |
| `28 °C ≤ suhu ≤ 30 °C` | Pertahankan kondisi sebelumnya |
| `< 28 °C` | Relay OFF |

> **Penting:** Pada zona *deadband*, program tidak memberikan perintah `digitalWrite()` baru. Oleh karena itu relay mempertahankan kondisi output terakhirnya.

---

# 8. Detail Percobaan

## Percobaan 1 — Akuisisi Data Sensor

Pengujian awal dilakukan dengan menghubungkan sensor DHT11 ke mikrokontroler untuk menguji kestabilan transfer data.

Hasil pengujian:

- Sensor berhasil membaca suhu secara berkala.
- Pembacaan dilakukan setiap ±2 detik.
- Data dapat ditampilkan melalui Serial Monitor.
- Tidak ditemukan *data loss* selama pengujian.

## Percobaan 2 — Integrasi Aktuator Relay dan Stress Testing

Sensor DHT11 kemudian diintegrasikan dengan modul relay 5V.

Pada kondisi suhu ruangan laboratorium sekitar:

**31.3 °C – 32.8 °C**

nilai suhu berada di atas threshold 30 °C sehingga sistem memberikan perintah:

```text
Aktuator: ON
```

## Pengujian Transisi Suhu Rendah

Breadboard diarahkan ke aliran udara pendingin ruangan (AC).

Perubahan suhu yang diamati:

```text
32.8 °C
   ↓
31.8 °C
   ↓
30.8 °C
   ↓
28.9 °C
```

Ketika suhu turun hingga melewati batas threshold, sistem mengubah kondisi relay menjadi:

```text
Aktuator: OFF
```

Hal tersebut menunjukkan bahwa sistem mampu merespons perubahan input sensor dan mengendalikan aktuator secara otomatis.

> **Catatan:** Pada program threshold awal, kondisi OFF terjadi pada `suhu <= 30 °C`. Pada implementasi histeresis, kondisi OFF baru dipicu ketika `suhu < 28 °C`.

---

# 9. Skematik Rangkaian

## Percobaan 1 — Sensor DHT11

```text
       NodeMCU / ESP
      ┌──────────────┐
      │              │
3.3V ─┤ VCC       D2 ├──── DATA DHT11
GND  ─┤ GND          │
      │              │
      └──────────────┘
             │
             ▼
         ┌─────────┐
         │  DHT11  │
         │         │
         │ VCC GND │
         │ DATA    │
         └─────────┘
```

## Percobaan 2 — DHT11 + Relay

```text
             ┌─────────────────┐
             │     NodeMCU     │
             │                 │
D2 / GPIO4 ──┤ DHT Data        │
D5 / GPIO14 ─┤ Relay IN        │
             │                 │
             └───────┬─────────┘
                     │
          ┌──────────┴──────────┐
          ▼                     ▼
      ┌─────────┐          ┌─────────┐
      │  DHT11  │          │  Relay  │
      │ Sensor  │          │ Module  │
      └─────────┘          └────┬────┘
                                │
                                ▼
                           Aktuator/Beban
```

> **Perhatian keselamatan:** Jika relay digunakan untuk mengendalikan beban tegangan tinggi (AC PLN), lakukan pengkabelan sesuai prosedur keselamatan dan jangan menyentuh bagian konduktif saat rangkaian aktif.



Contoh output:

```text
Suhu: 28.9 °C -> Aktuator: OFF
```


---

# 10. Kesimpulan

Praktikum Pertemuan 1 berhasil menunjukkan integrasi antara **sensor DHT11** sebagai perangkat input dan **relay** sebagai aktuator pada mikrokontroler.

Sistem mampu:

1. Membaca suhu dari DHT11 secara berkala.
2. Memvalidasi hasil pembacaan sensor.
3. Menampilkan data suhu melalui Serial Monitor.
4. Mengaktifkan relay ketika suhu melewati threshold.
5. Mematikan relay ketika suhu berada pada atau di bawah threshold.
6. Menggunakan logika histeresis untuk mengurangi risiko *chattering* pada relay.

Penerapan histeresis membuat sistem kendali lebih stabil karena relay tidak langsung berganti keadaan akibat fluktuasi suhu kecil di sekitar nilai ambang.

---

## 11. Ringkasan Konsep

```text
SENSOR
DHT11
  │
  │ Data Suhu
  ▼
MIKROKONTROLER
NodeMCU / ESP
  │
  │ Logika Threshold / Histeresis
  ▼
AKTUATOR
Relay
  │
  ▼
Beban / Perangkat yang Dikendalikan
```

**Konsep utama:** Sensor → Akuisisi Data → Validasi → Logika Kendali → Aktuator.
"""
