import pandas as pd
import matplotlib.pyplot as plt
import datetime

# Data asli dari Serial Monitor ESP8266 Anda
data_suhu = [
    30.40, 29.49, 30.26, 29.73, 30.18, 37.72, 30.15, 30.16, 29.57, 29.36, 
    29.65, 29.53, 28.80, 25.90, 26.35, 26.32, 26.21, 26.00, 26.51, 25.73, 
    26.36, 26.61, 26.36, 25.83, 26.01, 25.90
]

# Simulasi Waktu (kita pakai waktu dari log asli Anda)
waktu_log = [
    "14:28:31", "14:28:33", "14:28:35", "14:28:37", "14:28:39", "14:28:41", 
    "14:28:43", "14:28:45", "14:28:47", "14:28:49", "14:28:51", "14:28:53", 
    "14:28:55", "14:29:41", "14:29:43", "14:29:45", "14:29:47", "14:29:49", 
    "14:29:51", "14:29:53", "14:29:55", "14:29:57", "14:29:59", "14:30:01", 
    "14:30:03", "14:30:05"
]

# Masukkan ke DataFrame
df = pd.DataFrame({"waktu": waktu_log, "suhu": data_suhu})
df.to_csv("data_suhu_esp8266.csv", index=False)
print("Data tersimpan di data_suhu_esp8266.csv")

# --- Analisis Statistik & Z-score ---
rata2 = df["suhu"].mean()
std = df["suhu"].std()

df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print("-" * 30)
print(f"Rata-rata   : {rata2:.2f}")
print(f"Std deviasi : {std:.2f}")
print(f"Minimum     : {df['suhu'].min():.2f}")
print(f"Maksimum    : {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")
print("-" * 30)

print("\nDetail Data Anomali:")
print(df[df["anomali"]][["waktu", "suhu", "zscore"]])

# --- Visualisasi ---
plt.figure(figsize=(10, 5))
plt.plot(df.index + 1, df["suhu"], marker="o", label="Suhu Mentah ESP8266")
anom = df[df["anomali"]]
plt.scatter(anom.index + 1, anom["suhu"], color="red", s=150, label="Anomali (Z > 2)", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label=f"Rata-rata ({rata2:.2f})")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Percobaan 5B: Analisis Data Asli ESP8266 & Deteksi Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu_asli.png")
plt.show()