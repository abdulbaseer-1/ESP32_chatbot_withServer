# 🧠 ESP ChatBot

**ESP ChatBot** is a lightweight, offline-first voice interface framework for ESP32 using Espressif’s **ESP-SR v2.0.5**. It detects a custom wake word ("**hijason**"), captures audio from an I2S microphone, and publishes it via **MQTT** or **HTTP** to a backend for further AI processing (e.g., chat, transcription, commands).

---

## 🚀 Features

* 🔊 **Wake Word Detection** using **WakeNet9** with custom keyword "hijason"
* 🎙️ I2S microphone audio recording
* 🌐 Sends recorded audio to server using **MQTT** `(easil convertable to `**HTTP**`)`
* 🧩 Modular architecture with `custom_audio`, `custom_network`, `custom_system` components
* 🧠 Built with **ESP-SR v2.0.5** and **ESP-IDF v5.x**
* 📦 Easily extensible for on-device AI, NLP, or command control

---

## 🏗️ Project Structure

```
esp_chatbot/
├── main/                        # Main entry point
│   ├── main.c                  # App logic
│   └── CMakeLists.txt
├── components/
│   ├── custom_audio/           # I2S mic interface, audio capture
│   ├── custom_network/         # HTTP/MQTT handling
│   ├── custom_system/          # WakeNet integration, task control
│   └── ...                     # ESP-IDF + ESP-SR components
├── .devcontainer/              # Dev container config
├── .vscode/                    # VS Code settings
├── sdkconfig                   # IDF menuconfig output
├── partitions.csv              # Flash partition layout
├── CMakeLists.txt
└── README.md                   # This file
```

---

## 🧠 Wake Word Detection

* Uses **WakeNet9**, part of ESP-SR
* Wake word: **“hijason”**
* Model stored as C array in `main/hijason.h`, generated from `.bin` using:

```bash
xxd -i hijason.bin > hijason.h
```

* Integration:

```c
wn_iface = &ESP_WN9_MODEL;
wn_model = (model_t *)&hijason;  // Referencing the binary array
```

---

## 🎙️ Audio Pipeline

* Captures 5 seconds of PCM audio on wake word detection
* Audio buffer: mono, 16-bit, 16kHz
* Uses FreeRTOS tasks for non-blocking capture
* Sends audio via:

  * 📨 **MQTT** binary payload

---

## ⚙️ Configuration via `menuconfig`

Run:

```bash
idf.py menuconfig
```

Enable and configure:

* `ESP Speech Recognition`

  * WakeNet Model: `WakeNet9`
  * Wake Word: `wn9_hijason_tts2` ('you can choose any of the already available models from esp-sr' )
* `Custom Audio`

  * I2S Pins, Sample Rate: `16kHz`, Channels: `Mono`
* `Custom Network`

  * Enable MQTT or HTTP
  * Server IP, Port, Endpoint/Topic
* `Wi-Fi Credentials`

---

## 🔌 Hardware Requirements

| Component      | Details                      |
| -------------- | ---------------------------- |
| ESP32          | Dual-core, S3            |
| I2S Microphone | e.g., INMP441                |
| Server/Backend | MQTT broker |

---

## 📦 Build & Flash

### 1. Set up ESP-IDF v5.x

```bash
. $HOME/esp/esp-idf/export.sh
```

### 2. Build and Flash

```bash
idf.py build
idf.py -p COM17 flash  
idf.py monitor 
 
OR 
idf.py build -p COM17 flash monitor 
```

---

## 📡 MQTT / HTTP Format

### MQTT (example)

* Topic: `esp/audio/hijason`
* Payload: raw PCM audio

### HTTP (example)

```http
POST /api/audio HTTP/1.1
Content-Type: audio/raw
Body: [binary PCM16LE data]
```

---

## 📋 Example Use Case Flow

```
[Mic] --> [WakeNet detects "hijason"] --> [Start I2S Recording]
     --> [Send Audio Buffer] --> [Cloud AI/NLP backend]
     --> [Respond with Text/Speech/Action]
```

---

## 🧰 Development Tips

* Use `.devcontainer/` for VS Code remote development
* Use `.vscode/launch.json` to debug with IDF
* Keep model files small (< 200 KB) for flash safety

---

## 🔐 Security Notes

* MQTT not encrypted by default
* Recommend enabling TLS + authentication in production
* Validate buffer bounds when modifying components

---

## 📈 Performance

* WakeNet9 on ESP32: \~28% avg CPU, \~39% peak
* Optimized task prioritization for wake detection + recording
---

## 🔮 Future Enhancements

* [ ] On-device Multinet (command recognition)
* [ ] Automatic TFLite STT pipeline
* [ ] Voice feedback playback (TTS)
* [ ] Power management (light sleep between detections)

---

## 🤝 Contributing

We welcome contributions to improve wake word models, add protocols, or integrate new voice features.

---

## 📄 License

MIT License. See [`LICENSE`](./LICENSE) for details.

---

## 👤 Author

**Abdul Baseer**

> AI | Embedded AI | ESP32 Developer | MERN | Spring Boot
