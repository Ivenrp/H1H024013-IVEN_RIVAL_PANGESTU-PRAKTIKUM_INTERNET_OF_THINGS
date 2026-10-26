# Laporan Praktikum Internet of Things — Modul 5: Pengolahan dan Analisis Data IoT
**Pengolahan Data Edge (ESP8266/ESP32) dan Server Processing (Python, Moving Average, Z-Score, & Visualisasi)**

---

## Informasi Praktikan
- **Nama** : Iven Rival Pangestu
- **NIM** : H1H024013
- **Mata Kuliah** : Praktikum Internet Of Things (TK245005)
- **Tahun / Semester** : 2026 / 5 (Genap)
- **Modul** : Modul 5 — Pengolahan dan Analisis Data IoT
- **Nama Asisten / NIM** : Athallah Tsany Satriyaji / H1D023013
- **Program Studi** : S1 Teknik Komputer
- **Fakultas / Universitas** : Fakultas Teknik, Universitas Jenderal Soedirman

---

## Daftar Isi
1. [Detail dan Ringkasan Percobaan](#1-detail-dan-ringkasan-percobaan)
   - 1.1 [Tujuan Praktikum](#11-tujuan-praktikum)
   - 1.2 [Dasar Teori](#12-dasar-teori)
   - 1.3 [Alat dan Bahan](#13-alat-dan-bahan)
2. [Library dan Dependencies yang Diperlukan](#2-library-dan-dependencies-yang-diperlukan)
3. [Percobaan 5A: Pengolahan Data Sensor pada ESP8266 (Moving Average & Statistik)](#3-percobaan-5a-pengolahan-data-sensor-pada-esp8266-moving-average--statistik)
   - 3.1 [Penjelasan Kode Base Percobaan 5A](#31-penjelasan-kode-base-percobaan-5a)
   - 3.2 [Jawaban Pertanyaan Praktikum 5A](#32-jawaban-pertanyaan-praktikum-5a)
   - 3.3 [Modifikasi Kode Percobaan 5A (Perhitungan Simpangan Baku / Standard Deviation)](#33-modifikasi-kode-percobaan-5a-perhitungan-simpangan-baku--standard-deviation)
   - 3.4 [Penjelasan Baris demi Baris Kode Modifikasi 5A](#34-penjelasan-baris-demi-baris-kode-modifikasi-5a)
   - 3.5 [Penjelasan Fungsi yang Digunakan pada Percobaan 5A](#35-penjelasan-fungsi-yang-digunakan-pada-percobaan-5a)
   - 3.6 [Penjelasan Percabangan / Conditional Logic pada Percobaan 5A](#36-penjelasan-percabangan--conditional-logic-pada-percobaan-5a)
4. [Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data Suhu (Server Processing)](#4-percobaan-5b-akuisisi-analisis-dan-visualisasi-data-suhu-server-processing)
   - 4.1 [Penjelasan Kode Base Percobaan 5B](#41-penjelasan-kode-base-percobaan-5b)
   - 4.2 [Ulasan Alur Pemrosesan Data pada Python](#42-ulasan-alur-pemrosesan-data-pada-python)
   - 4.3 [Jawaban Pertanyaan Praktikum 5B](#43-jawaban-pertanyaan-praktikum-5b)
   - 4.4 [Modifikasi Kode Percobaan 5B (Moving Average Sebelum Deteksi Z-Score)](#44-modifikasi-kode-percobaan-5b-moving-average-sebelum-deteksi-z-score)
   - 4.5 [Penjelasan Baris demi Baris Kode Modifikasi 5B](#45-penjelasan-baris-demi-baris-kode-modifikasi-5b)
   - 4.6 [Penjelasan Fungsi yang Digunakan pada Percobaan 5B](#46-penjelasan-fungsi-yang-digunakan-pada-percobaan-5b)
   - 4.7 [Penjelasan Percabangan / Conditional Logic pada Percobaan 5B](#47-penjelasan-percabangan--conditional-logic-pada-percobaan-5b)
5. [Jawaban Pertanyaan Analisis (Bagian VII Buku Catatan Praktikum)](#5-jawaban-pertanyaan-analisis-bagian-vii-buku-catatan-praktikum)
6. [Catatan Kendala dan Solusi](#6-catatan-kendala-dan-solusi)
7. [Dokumentasi Praktikum](#7-dokumentasi-praktikum)

---

## 1. Detail dan Ringkasan Percobaan

### 1.1 Tujuan Praktikum
Berdasarkan eksperimen langsung dan Buku Catatan/Log Praktikum (BCP), tujuan praktikum Modul 5 adalah:
1. Memahami dan mengimplementasikan pengolahan data di sisi perangkat (*edge processing*) pada mikrokontroler ESP8266 sebelum data dikirim ke server.
2. Memprogram dan menganalisis algoritma *Moving Average* (rata-rata bergerak) untuk menyaring derau (*noise*) data sensor serta mencatat statistik dasar (suhu minimum dan maksimum) secara *real-time*.
3. Memahami pemrosesan data di sisi komputer/server (*server processing*) menggunakan Python untuk menyimpan data tabular (CSV), mengkalkulasi statistik deskriptif, dan mendeteksi pencilan (anomali) menggunakan metode Z-Score.
4. Mampu memvisualisasikan data runtun waktu (*time series*) dan menyoroti titik data anomali ke dalam bentuk grafik yang informatif.

### 1.2 Dasar Teori
Dalam arsitektur sistem IoT modern, data mentah (*raw data*) dari transduser fisik sering kali terkontaminasi oleh gangguan acak (*noise*), fluktuasi tegangan, atau lonjakan sesaat (*spike*). 

#### A. Edge Processing vs Server Processing
- **Edge Processing**: Pemrosesan data dilakukan langsung pada mikrokontroler (ESP8266). Keunggulannya adalah latensi sangat rendah, menghemat *bandwidth* jaringan, dan tetap bekerja saat koneksi terputus. Namun, dibatasi oleh kapasitas RAM dan daya komputasi mikrokontroler.
- **Server Processing**: Pemrosesan terpusat pada komputer/cloud menggunakan bahasa tingkat tinggi (Python). Mampu menjalankan analisis statistik kompleks, penyimpanan historis jangka panjang (CSV/SQL), dan visualisasi data.

#### B. Filter Moving Average (Rata-Rata Bergerak) & Circular Buffer
Filter Moving Average (MA) menghitung rata-rata dari $N$ sampel data terakhir:
$$y[k] = \frac{1}{N} \sum_{i=0}^{N-1} x[k-i]$$
Agar hemat memori dan efisien, data dikelola menggunakan struktur *Circular Buffer* (*Ring Buffer*) dengan operasi modulo `(indeks + 1) % N` yang berkecepatan konstan $O(1)$.

#### C. Deteksi Anomali dengan Z-Score
Z-score mengukur seberapa jauh suatu titik data menyimpang dari rata-rata sampel dalam satuan simpangan baku:
$$z = \frac{x - \mu}{\sigma}$$
- $\mu$ (*mean*): Rata-rata sampel.
- $\sigma$ (*standard deviation*): Simpangan baku sampel.
- **Batas $|z| > 2$**: Berdasarkan aturan distribusi normal ( Empirical Rule ), sekitar 95% data normal berada di rentang $-2\sigma \le x \le +2\sigma$. Titik data dengan $|z| > 2$ memiliki probabilitas kemunculan acak $< 5\%$, sehingga dikategorikan sebagai data anomali.

### 1.3 Alat dan Bahan
- **Hardware**: 1 unit Board ESP8266 (NodeMCU / Wemos D1 Mini) dan 1 unit kabel USB (Micro-USB ke USB-A).
- **Software IDE**: Arduino IDE dengan paket *board* ESP8266 terpasang.
- **Software Analisis**: Laptop/PC dengan Python 3, pustaka `pandas`, dan `matplotlib`.

---

## 2. Library dan Dependencies yang Diperlukan

### A. Sisi Mikrokontroler (Arduino IDE)
- `<ESP8266WiFi.h>`: Pustaka dasar *board* ESP8266 (bawaan *template*).
- `<math.h>`: Pustaka matematika C/C++ bawaan untuk fungsi trigonometri `sin()` dan akar kuadrat `sqrt()`.

### B. Sisi Server / Komputer (Python Environment)
Instalasi pustaka Python dilakukan melalui perintah terminal:
```bash
pip install pandas matplotlib
```
| Library Python | Versi / Fungsi | Peran dan Kegunaan Teknis |
| :--- | :--- | :--- |
| `pandas` | Data Analysis | Struktur data DataFrame untuk manipulasi data tabular, ekspor CSV, dan komputasi statistik (mean, std). |
| `matplotlib` | Visualization | Modul pembuatan grafik visualisasi runtun waktu (*time series*) dan penandaan titik anomali. |

---

## 3. Percobaan 5A: Pengolahan Data Sensor pada ESP8266 (Moving Average & Statistik)

### 3.1 Penjelasan Kode Base Percobaan 5A
File implementasi: `code/modul5_pengolahan_data.ino`

```cpp
#include <ESP8266WiFi.h>

//--- MOCK FUNCTION (pengganti sensor DHT22)
float bacaSuhuMock() {
  static unsigned long jumlahBaca = 0;
  jumlahBaca++;
  float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
  float noise = random(-5, 6) * 0.1;
  float suhu = suhuDasar + noise;
  if (jumlahBaca % 25 == 0) {
    suhu += 8.0; // Simulasi anomali
  }
  return suhu;
}

const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;

float suhuMin = 1000.0;
float suhuMax = -1000.0;

float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;
  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  float suhu = bacaSuhuMock();
  float suhuFilter = hitungMovingAverage(suhu);

  if (suhu < suhuMin) suhuMin = suhu;
  if (suhu > suhuMax) suhuMax = suhu;

  Serial.print("Mentah: ");
  Serial.print(suhu, 2);
  Serial.print(" | Filter: ");
  Serial.print(suhuFilter, 2);
  Serial.print(" | Min: ");
  Serial.print(suhuMin, 2);
  Serial.print(" | Max: ");
  Serial.println(suhuMax, 2);

  delay(2000);
}
```

### 3.2 Jawaban Pertanyaan Praktikum 5A

#### 1. Diagram Alur (Flowchart) Proses Perhitungan Moving Average pada `hitungMovingAverage()`
```
 [ Mulai ]
     │
 [ Terima nilaiBaru ]
     │
 [ buffer[indeks] = nilaiBaru ]
     │
 [ indeks = (indeks + 1) % N ]
     │
    / \
   /   \  Ya
  < jumlah < N ? > ───────────┐
   \   /                      │
    \ / Tidak                 ▼
     │                   [ jumlah++ ]
     │                        │
     └────────────────────────┴──► [ total = 0 ]
                                       │
                                   [ Loop i = 0 s.d. jumlah-1 ]
                                   [ total += buffer[i]       ]
                                       │
                                   [ rata-rata = total / jumlah ]
                                       │
                                   [ Return rata-rata ]
```

#### 2. Fungsi Operasi `(indeks + 1) % N` pada Program
Operasi modulo (`%`) membuat *array buffer* bekerja secara melingkar (*circular buffer*). Dengan $N=5$, variabel `indeks` berjalan 0, 1, 2, 3, 4, lalu kembali ke 0 karena $(4+1) \% 5 = 0$. Hal ini mencegah *array out of bounds* dan membuat data terbaru selalu menimpa data paling lama dengan kompleksitas waktu konstan $O(1)$.

#### 3. Dampak Pemilihan Ukuran Jendela $N$ ($N=20$ vs $N=2$)
- **Jika $N=20$**: Jendela mencakup 40 detik data ($20 \times 2$ detik). Kurva sangat halus dan lonjakan $+8.0^\circ\text{C}$ hanya menaikkan rata-rata sebesar $\approx 0.4^\circ\text{C}$. Namun, timbul *lagging* (keterlambatan respons) yang tinggi terhadap perubahan suhu nyata.
- **Jika $N=2$**: Jendela hanya 4 detik. Respons sangat cepat, tetapi lonjakan anomali langsung menaikkan rata-rata sebesar $\approx 4.0^\circ\text{C}$, sehingga derau dan anomali nyaris tidak teredam.

### 3.3 Modifikasi Kode Percobaan 5A (Perhitungan Simpangan Baku / Standard Deviation)
**Soal Modifikasi**: "Modifikasi program agar ESP8266 juga menghitung simpangan baku pada data dalam jendela!"

File implementasi: `code/modul5_pengolahan_data_modifikasi.ino`
```cpp
#include <ESP8266WiFi.h>
#include <math.h>

float simpangan = 0; // Variabel global simpangan baku

float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  float rata = total / jumlah;

  // --- Perhitungan Varians dan Simpangan Baku ---
  float varians = 0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rata;
    varians += (selisih * selisih);
  }
  varians = varians / jumlah;
  simpangan = sqrt(varians); // Akar kuadrat varians

  return rata;
}

void loop() {
  float suhu = bacaSuhuMock();
  float suhuFilter = hitungMovingAverage(suhu);

  if (suhu < suhuMin) suhuMin = suhu;
  if (suhu > suhuMax) suhuMax = suhu;

  Serial.print("Mentah: ");   Serial.print(suhu, 2);
  Serial.print(" | Filter: "); Serial.print(suhuFilter, 2);
  Serial.print(" | StdDev: "); Serial.print(simpangan, 2);
  Serial.print(" | Min: ");    Serial.print(suhuMin, 2);
  Serial.print(" | Max: ");    Serial.println(suhuMax, 2);

  delay(2000);
}
```

### 3.4 Penjelasan Baris demi Baris Kode Modifikasi 5A
1. `float varians = 0;`: Inisialisasi variabel akomodasi penjumlahan kuadrat selisih.
2. `float selisih = buffer[i] - rata;`: Menghitung deviasi tiap titik data di buffer terhadap nilai rata-rata sampel.
3. `varians += (selisih * selisih);`: Mengakumulasikan kuadrat deviasi ke dalam variabel `varians`.
4. `varians = varians / jumlah;`: Menghitung rata-rata kuadrat selisih (varians).
5. `simpangan = sqrt(varians);`: Mengambil akar kuadrat dari varians menggunakan pustaka `<math.h>` untuk memperoleh simpangan baku ($\sigma$).
6. `Serial.print(" | StdDev: "); Serial.print(simpangan, 2);`: Mencetak nilai simpangan baku ke Serial Monitor.

### 3.5 Penjelasan Fungsi yang Digunakan pada Percobaan 5A
| Nama Fungsi | Parameter Masukan | Return Value | Penjelasan Kegunaan Teknis |
| :--- | :--- | :--- | :--- |
| `bacaSuhuMock()` | None | `float` | Menghasilkan sinyal suhu sintetis (sinusoidal + noise acak + anomali tiap kelipatan 25). |
| `hitungMovingAverage()` | `float nilaiBaru` | `float` | Memperbarui circular buffer dan mengembalikan rata-rata bergerak $N=5$. |
| `sin()` | `double` | `double` | Menghitung nilai gelombang sinus untuk simulasi suhu dasar. |
| `random(min, max)` | `long, long` | `long` | Menghasilkan angka acak batas bawah hingga batas atas $-1$ untuk simulasi *noise*. |
| `sqrt()` | `double` | `double` | Menghitung akar kuadrat dari varians matematika. |

### 3.6 Penjelasan Percabangan / Conditional Logic pada Percobaan 5A
1. `if (jumlahBaca % 25 == 0)`: Injeksi lonjakan suhu $+8.0^\circ\text{C}$ pada *mock function* setiap siklus kelipatan 25.
2. `if (jumlah < N) jumlah++;`: Menjamin pembagian rata-rata awal memakai jumlah data aktual sebelum buffer terisi penuh 5 elemen.
3. `if (suhu < suhuMin) suhuMin = suhu;` & `if (suhu > suhuMax) suhuMax = suhu;`: Membandingkan suhu mentah terbaru untuk memperbarui nilai batas ekstremum global.

---

## 4. Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data Suhu (Server Processing)

### 4.1 Penjelasan Kode Base Percobaan 5B
File implementasi: `code/modul5_analisis_data.py`

```python
import pandas as pd
import matplotlib.pyplot as plt

# Data suhu riil dari log ESP8266 (Percobaan 5A)
data_suhu = [
    30.40, 29.49, 30.26, 29.73, 30.18, 37.72, 30.15,
    30.16, 29.57, 29.36, 29.65, 29.53, 28.80, 25.90,
    26.35, 26.32, 26.21, 26.00, 26.51, 25.73, 26.36,
    26.61, 26.36, 25.83, 26.01, 25.90
]

waktu_log = [
    "14:28:31", "14:28:33", "14:28:35", "14:28:37", "14:28:39", "14:28:41",
    "14:28:43", "14:28:45", "14:28:47", "14:28:49", "14:28:51", "14:28:53",
    "14:28:55", "14:29:41", "14:29:43", "14:29:45", "14:29:47", "14:29:49",
    "14:29:51", "14:29:53", "14:29:55", "14:29:57", "14:29:59", "14:30:01",
    "14:30:03", "14:30:05"
]

df = pd.DataFrame({"waktu": waktu_log, "suhu": data_suhu})
df.to_csv("data_suhu_esp8266.csv", index=False)

rata2 = df["suhu"].mean()
std = df["suhu"].std()

df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata: {rata2:.2f} | Std: {std:.2f} | Anomali: {df['anomali'].sum()}")

plt.figure(figsize=(10, 5))
plt.plot(df.index + 1, df["suhu"], marker="o", label="Suhu Mentah", color="black")
anom = df[df["anomali"]]
plt.scatter(anom.index + 1, anom["suhu"], color="red", s=150, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
plt.title("Analisis Data Suhu & Deteksi Anomali")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.legend()
plt.savefig("grafik_suhu.png")
plt.show()
```

### 4.2 Ulasan Alur Pemrosesan Data pada Python
1. **Inisialisasi & Tabulasi**: Mengonversi array `data_suhu` dan `waktu_log` menjadi `pandas.DataFrame`.
2. **Penyimpanan tabular**: Menyimpan DataFrame ke file `data_suhu_esp8266.csv`.
3. **Analisis Statistik & Z-Score**: Menghitung mean ($\mu = 28.27^\circ\text{C}$) dan standard deviation ($\sigma = 2.67^\circ\text{C}$). Menghitung nilai Z-Score untuk tiap baris data.
4. **Visualisasi Data**: Memetakan suhu mentah ke grafik garis, memberi acuan garis rata-rata, dan menyoroti titik anomali ($|z| > 2$) menggunakan titik merah besar (`s=150`).

### 4.3 Jawaban Pertanyaan Praktikum 5B

#### 1. Fungsi Blok `try-except` pada `on_message()` dan Akibat Jika Dihilangkan
- **Fungsi**: Mencegah program berhenti akibat galat parsing JSON saat menerima data MQTT yang tidak valid atau korup.
- **Akibat Jika Dihilangkan**: Satu pesan rusak/non-JSON akan memicu *unhandled exception* (`JSONDecodeError`) yang langsung menghentikan seluruh eksekusi skrip Python.

#### 2. Arti Nilai Z-Score dan Alasan Batas $|z| > 2$ Digunakan
- **Arti Z-Score**: Menyatakan seberapa jauh suatu titik data menyimpang dari rata-rata dalam satuan simpangan baku.
- **Alasan Batas $|z| > 2$**: Pada distribusi normal, sekitar 95% data berada dalam rentang $-2 \le z \le +2$. Nilai di luar batas ini hanya berpeluang muncul $< 5\%$, sehingga dikategorikan sebagai pencilan/anomali.

#### 3. Kelemahan Metode Z-Score jika Data Memiliki Tren Naik/Turun yang Tajam
Z-score membandingkan nilai terhadap rata-rata global. Pada tren naik/turun yang tajam:
1. Data normal di awal (paling rendah) dan di akhir (paling tinggi) akan terpaut jauh dari rata-rata global, sehingga keliru terdeteksi sebagai anomali (*False Positive*).
2. Nilai tren yang lebar memperbesar nilai $\sigma$ global, sehingga anomali lokal berskala kecil justru gagal terdeteksi (*False Negative*).

### 4.4 Modifikasi Kode Percobaan 5B (Moving Average Sebelum Deteksi Z-Score)
**Soal Modifikasi**: "Modifikasi program agar memakai moving average (jendela 5 data) pada data CSV sebelum Z-score dihitung!"

File implementasi: `code/modul5_analisis_data_modifikasi.py`
```python
import pandas as pd

df = pd.read_csv("data_suhu_esp8266.csv")

# Menerapkan Moving Average (window=5)
df["suhu_ma"] = df["suhu"].rolling(window=5).mean()

# Membuang 4 data awal (jendela belum penuh)
sub = df.dropna(subset=["suhu_ma"]).copy()

rata_ma = sub["suhu_ma"].mean()
std_ma = sub["suhu_ma"].std()

sub["zscore_ma"] = (sub["suhu_ma"] - rata_ma) / std_ma
sub["anomali_ma"] = sub["zscore_ma"].abs() > 2

print(f"Rata-rata MA : {rata_ma:.2f}°C")
print(f"StdDev MA    : {std_ma:.2f}°C")
print(f"Z-Max MA     : {sub['zscore_ma'].abs().max():.2f}")
print(f"Jumlah Anomali Setelah MA: {sub['anomali_ma'].sum()}")
```

### 4.5 Penjelasan Baris demi Baris Kode Modifikasi 5B
1. `df["suhu"].rolling(window=5).mean()`: Menghitung rata-rata bergerak jendela 5 baris pada DataFrame pandas.
2. `df.dropna(subset=["suhu_ma"])`: Menghapus 4 baris pertama yang bernilai `NaN` karena sampel buffer belum memenuhi jendela $N=5$.
3. `rata_ma = sub["suhu_ma"].mean()` & `std_ma = sub["suhu_ma"].std()`: Menghitung parameter statistik baru dari data terfilter.
4. `sub["zscore_ma"].abs() > 2`: Mengevaluasi kondisi anomali dari kolom Moving Average.
5. **Hasil Perbandingan**:
   - Rata-rata terfilter: $28.32^\circ\text{C}$, Standard Deviation: $2.22^\circ\text{C}$.
   - Z-score maksimum teredam menjadi **1.48** (di bawah batas 2.0).
   - Jumlah anomali berkurang dari **1 menjadi 0**. Lonjakan $37.72^\circ\text{C}$ teredam menjadi $31.48^\circ\text{C}$.

### 4.6 Penjelasan Fungsi yang Digunakan pada Percobaan 5B
| Nama Fungsi | Pustaka / Modul | Penjelasan Kegunaan Teknis |
| :--- | :--- | :--- |
| `pd.DataFrame()` | `pandas` | Mengonversi struktur dictionary menjadi tabel data dua dimensi. |
| `df.to_csv()` | `pandas` | Mengekspor DataFrame ke dalam file fisik berformat `.csv`. |
| `df["col"].mean()` | `pandas` | Menghitung rata-rata aritmatika dari kolom sampel. |
| `df["col"].std()` | `pandas` | Menghitung simpangan baku dari kolom sampel. |
| `df["col"].rolling()` | `pandas` | Menerapkan operasi jendela bergerak (*moving window*) pada deret waktu. |
| `plt.scatter()` | `matplotlib` | Menggambar grafik sebar titik (*scatter plot*) penanda posisi anomali. |

### 4.7 Penjelasan Percabangan / Conditional Logic pada Percobaan 5B
1. `df["zscore"].abs() > 2`: Evaluasi nilai mutlak Z-score yang melebihi 2 untuk menetapkan flag boolean `True`/`False` pada kolom `anomali`.
2. `anom = df[df["anomali"]]`: *Filtering* baris DataFrame yang bernilai `True` untuk digambar khusus pada layar grafik.

---

## 5. Jawaban Pertanyaan Analisis (Bagian VII Buku Catatan Praktikum)

### 1. Uraian Hasil Tugas pada Praktikum yang Telah Dilakukan
#### Tabel 5.1: Hasil Pengolahan Data Suhu pada ESP8266 (Percobaan 5A)
| No | Waktu | Suhu Mentah (°C) | Moving Average (°C) | Suhu Min (°C) | Suhu Max (°C) |
| :-: | :-: | :-: | :-: | :-: | :-: |
| 1 | 14:28:31 | 30.40 | 29.99 | 28.23 | 30.40 |
| 2 | 14:28:33 | 29.49 | 29.84 | 28.23 | 30.40 |
| 3 | 14:28:35 | 30.26 | 30.01 | 28.23 | 30.40 |
| 4 | 14:28:37 | 29.73 | 30.04 | 28.23 | 30.40 |
| 5 | 14:28:39 | 30.18 | 30.01 | 28.23 | 30.40 |
| 6 | 14:28:41 | **37.72** | **31.48** | 28.23 | **37.72** |
| 7 | 14:28:43 | 30.15 | 31.61 | 28.23 | 37.72 |
| 8 | 14:28:45 | 30.16 | 31.59 | 28.23 | 37.72 |
| 9 | 14:28:47 | 29.57 | 31.56 | 28.23 | 37.72 |
| 10 | 14:28:49 | 29.36 | 31.39 | 28.23 | 37.72 |

#### Tabel 5.2: Hasil Akuisisi dan Analisis Data Suhu pada Python (Percobaan 5B)
| No | Waktu | Suhu (°C) | Z-score | Status Anomali | Status Simpan CSV |
| :-: | :-: | :-: | :-: | :-: | :-: |
| 1 | 14:28:31 | 30.40 | 0.79 | Tidak | Tersimpan |
| 2 | 14:28:33 | 29.49 | 0.45 | Tidak | Tersimpan |
| 3 | 14:28:35 | 30.26 | 0.74 | Tidak | Tersimpan |
| 4 | 14:28:37 | 29.73 | 0.54 | Tidak | Tersimpan |
| 5 | 14:28:39 | 30.18 | 0.71 | Tidak | Tersimpan |
| 6 | 14:28:41 | **37.72** | **3.54** | **Ya (Anomali)** | Tersimpan |
| 7 | 14:28:43 | 30.15 | 0.70 | Tidak | Tersimpan |
| 8 | 14:28:45 | 30.16 | 0.70 | Tidak | Tersimpan |
| 9 | 14:28:47 | 29.57 | 0.48 | Tidak | Tersimpan |
| 10 | 14:28:49 | 29.36 | 0.40 | Tidak | Tersimpan |

**Parameter Statistik Hasil Eksekusi Python (26 Data)**:
- Rata-rata (*mean*) = $28.27^\circ\text{C}$
- Simpangan baku (*std*) = $2.67^\circ\text{C}$
- Suhu Minimum = $25.73^\circ\text{C}$ | Suhu Maksimum = $37.72^\circ\text{C}$
- Jumlah Anomali ($|z| > 2$) = **1 data** (Data ke-6: $37.72^\circ\text{C}$, $Z = +3.54$)

---

### 2. Komparasi Edge Processing (5A) vs Server Processing (5B)
| Parameter Komparasi | Edge Processing (ESP8266 / Percobaan 5A) | Server Processing (Python / Percobaan 5B) |
| :--- | :--- | :--- |
| **Lokasi Komputasi** | Mikrokontroler lokal (ESP8266) | Komputer lokal / Cloud Server |
| **Kecepatan / Latensi** | Sangat cepat (sub-milidetik, tanpa latensi jaringan) | Bergantung pada latensi jaringan dan antrean server |
| **Kompleksitas Analisis** | Terbatas (Moving average, min/max) | Sangat tinggi (Z-Score, Machine Learning, Visualisasi) |
| **Kebutuhan Resources** | RAM & flash sangat terbatas | RAM gigabyte & ruang penyimpanan hard disk besar |
| **Konsumsi Bandwidth** | Hemat (hanya mengirim ringkasan/data terfilter) | Lebih besar (mengirim seluruh sampel data mentah) |

---

### 3. Pengaruh Ukuran Jendela Moving Average terhadap Aktuator
- **Jendela Terlalu Besar ($N=20$)**: Kurva sangat halus dan stabil dari *noise*, tetapi memiliki keterlambatan respons (*phase lag*) tinggi. Sangat berbahaya pada sistem seperti alarm kebakaran karena respon aktuator terlambat.
- **Jendela Terlalu Kecil ($N=2$)**: Respons sangat cepat, tetapi *noise* sensor membuat aktuator sering menyala-mati secara liar (*chattering*), yang memicu keausan alat.
- **Kesimpulan**: Jendela $N=5$ merupakan nilai kompromi optimal untuk menjaga keseimbangan antara kestabilan dan kecepatan respons.

---

### 4. Pemanfaatan Analisis Anomali pada Sistem IoT Dunia Nyata
- **Smart Farming**: Mendeteksi kerusakan saluran irigasi atau malfungsi sensor kelembaban tanah sebelum tanaman mati dehidrasi.
- **Smart Home**: Mendeteksi lonjakan arus listrik atau kenaikan suhu tak wajar di ruangan untuk pencegahan kebakaran dini dan otomatis memutus saklar utama.

---

## 6. Catatan Kendala dan Solusi

1. **Ketiadaan Sensor Physical DHT22**:
   - *Kendala*: Sensor DHT22 fisik tidak tersedia di laboratorium.
   - *Solusi*: Menggunakan *mock function* `bacaSuhuMock()` berbasis gelombang sinus, derau acak, dan injeksi anomali pada ESP8266 sesuai arahan asisten praktikum.
2. **Koneksi Port Serial COM Error**:
   - *Kendala*: Arduino IDE gagal mendeteksi port COM (`FileNotFoundError`) saat mengunggah program.
   - *Solusi*: Menutup Serial Monitor yang aktif, mengganti kabel data USB, dan menyegarkan ulang Arduino IDE hingga port terbaca.

---

## 7. Dokumentasi Praktikum

File dokumentasi praktikum dapat diakses pada struktur repositori berikut:
- `dokumentasi/flowchart_moving_average.png` — Flowchart fungsi filter Moving Average.
- `dokumentasi/percobaan5a_serial.png` — Tampilan output Serial Monitor Arduino IDE Percobaan 5A.
- `dokumentasi/grafik_suhu.png` — Grafik hasil visualisasi suhu mentah, rata-rata, dan penanda titik anomali Z-score dari Python.