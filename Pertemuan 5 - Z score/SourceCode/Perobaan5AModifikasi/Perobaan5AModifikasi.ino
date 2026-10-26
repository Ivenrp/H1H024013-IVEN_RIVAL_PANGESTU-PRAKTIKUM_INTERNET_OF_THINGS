import pandas as pd
import matplotlib.pyplot as plt

# Data suhu riil dari log ESP8266 (Percobaan 5A BCP)
data_suhu = [
    30.40, 29.49, 30.26, 29.73, 30.18, 37.72, 30.15, 30.16, 29.57, 29.36,
    29.65, 29.53, 28.80, 25.90, 26.35, 26.32, 26.21, 26.00, 26.51, 25.73,
    26.36, 26.61, 26.36, 25.83, 26.01, 25.90
]

waktu_log = [
    "14:28:31", "14:28:33", "14:28:35", "14:28:37", "14:28:39", "14:28:41",
    "14:28:43", "14:28:45", "14:28:47", "14:28:49", "14:28:51", "14:28:53",
    "14:28:55", "14:29:41", "14:29:43", "14:29:45", "14:29:47", "14:29:49",
    "14:29:51", "14:29:53", "14:29:55", "14:29:57", "14:29:59", "14:30:01",
    "14:30:03", "14:30:05"
]

# Pembuatan DataFrame
df = pd.DataFrame({"waktu": waktu_log, "suhu": data_suhu})
df.to_csv("data_suhu_esp8266.csv", index=False)

# Analisis Statistik
rata2 = df["suhu"].mean()
std = df["suhu"].std()
df["zscore"] = (df["suhu"] - rata2) / std
df["anomali"] = df["zscore"].abs() > 2

print(f"Rata-rata: {rata2:.2f} °C | Std: {std:.2f} °C | Anomali: {df['anomali'].sum()}")

# Visualisasi Data
plt.figure(figsize=(10, 5))
plt.plot(df.index + 1, df["suhu"], marker="o", label="Suhu Mentah", color="black")

anom = df[df["anomali"]]
plt.scatter(anom.index + 1, anom["suhu"], color="red", s=150, label="Anomali", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")

plt.title("Analisis Data Suhu & Deteksi Anomali")
plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.legend()
plt.grid(True, linestyle=":", alpha=0.6)
plt.savefig("grafik_suhu.png")
plt.show()