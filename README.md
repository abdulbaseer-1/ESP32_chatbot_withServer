# 📘 ESP32 ChatBot Server: Comprehensive Technical Documentation

---

## 1. Introduction

The ESP32 ChatBot Server provides a robust backend for enabling natural language interaction through an ESP32 microcontroller.
It bridges embedded voice capture with modern AI services such as OpenAI, Whisper, or custom NLP models.

This document details the architecture, configuration, communication protocols, and security considerations required to deploy the system.

### 1.1 System Goals

* Provide a reliable backend for handling audio data from ESP32
* Support flexible AI integrations for speech-to-text (STT), natural language processing (NLP), and text-to-speech (TTS)
* Ensure modularity and extensibility for future enhancements
* Maintain security and scalability for production environments

---

## 2. Architecture

```
      [Voice]
        ↓
     +--------+      Wi-Fi       +------------+     API     +------------------+
     | ESP32  | ───────────────▶ |  SpringBoot| ────────▶ |   AI Assistant    |
     | +Mic   |                   |   Server  |            | (OpenAI, etc.)    |
     | +WakeWD| ◀─────────────── | (API + Cfg)| ◀───────  |  TTS, Whisper,etc.|
     +--------+    Response      +------------+   Text    +------------------+
```

### 2.1 High-Level Flow

1. The ESP32 detects a wake word and streams PCM audio.
2. The Spring Boot server receives audio via REST or MQTT.
3. Audio is processed through **STT → NLP → TTS** pipeline.
4. The server returns either text or audio response back to the ESP32.

### 2.2 Architecture Diagram

* ESP32 — Wake Word Detection
* Spring Boot Server — REST & MQTT Broker
* AI API / Service — (STT / NLP / TTS)
* Communication:

  * Wi-Fi / MQTT
  * API Calls
  * TTS Audio / Data
  * Responses

### 2.3 Design Principles

* **Modularity:** Separate services for STT, NLP, and TTS allow swapping providers.
* **Scalability:** Server can be deployed in containers and scaled horizontally.
* **Extensibility:** New AI providers or protocols (WebSockets, gRPC) can be added.

---

## 3. Project Structure

```
Server/
+-- src/main/java/com/espchatbot/
|   +-- controller/   # REST endpoints
|   +-- service/      # AI integration services
|   +-- config/       # Configuration and security
|   \-- mqtt/         # MQTT client handler
+-- src/main/resources/
|   +-- application.yml # Configurations
|   \-- logback.xml     # Logging
+-- pom.xml             # Maven dependencies
\-- README.md
```

---

## 4. Configuration

### 4.1 Application Configuration

Server configurations are defined in **application.yml**:

```yaml
server:
  port: 8080

ai:
  provider: openai
  apiKey: YOUR_API_KEY
  model: gpt-4o-mini
  ttsModel: gpt-4o-mini-tts
```

### 4.2 Environment Variables

```bash
export OPENAI_API_KEY="sk-xxxx"
```

### 4.3 Logging Configuration

Logback is used for configurable and structured logging. Developers can enable debug-level logging for troubleshooting.

---

## 5. Communication Protocols

### 5.1 REST Endpoints

**Audio Upload**

```http
POST /api/audio
Content-Type: audio/raw

[binary PCM16LE data]
```

Returns transcription and AI-generated response.

**Chat Interaction**

```http
POST /api/chat
Content-Type: application/json

{ "message": "Hello ESP32!" }
```

---

### 5.2 MQTT Topics

* PCM Audio Input: `esp/audio/hijason`
* AI Response Output: `esp/response/hijason`

### 5.3 Data Formats

* **Audio:** PCM16, mono, 16kHz
* **Text:** JSON payload with fields `transcript`, `response`

---

## 6. Build & Deployment

### 6.1 Building with Maven

```bash
mvn clean package
```

### 6.2 Running the Server

```bash
java -jar target/esp32-chatbot-server-0.0.1-SNAPSHOT.jar
```

---

# ESP ChatBot Firmware

**ESP ChatBot** is a lightweight, offline-first voice interface framework for ESP32 using Espressif’s **ESP-SR v2.0.5**.
It detects a custom wake word ("**hijason**"), captures audio from an I2S microphone, and publishes it via **MQTT** or **HTTP** to a backend for further AI processing (e.g., chat, transcription, commands).

---

##  Features

* **Wake Word Detection** using **WakeNet9** with custom keyword "hijason"
* I2S microphone audio recording
* Sends recorded audio to server using **MQTT** *(easily convertible to **HTTP**)*
* Modular architecture with `custom_audio`, `custom_network`, `custom_system` components
* Built with **ESP-SR v2.0.5** and **ESP-IDF v5.x**
* Easily extensible for on-device AI, NLP, or command control

---

## Project Structure

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

## Wake Word Detection

* Uses **WakeNet9**, part of ESP-SR
* Wake word: **“hijason”**
* Model stored as C array in `main/hijason.h`, generated from `.bin` using:

```bash
xxd -i hijason.bin > hijason.h
```

Integration:

```c
wn_iface = &ESP_WN9_MODEL;
wn_model = (model_t *)&hijason;  // Referencing the binary array
```

---

## Audio Pipeline

* Captures **5 seconds** of PCM audio on wake word detection
* Audio buffer: **mono, 16-bit, 16kHz**
* Uses **FreeRTOS tasks** for non-blocking capture
* Sends audio via:

  * **MQTT** binary payload

---

## Configuration via `menuconfig`

Run:

```bash
idf.py menuconfig
```

Enable and configure:

* **ESP Speech Recognition**

  * WakeNet Model: `WakeNet9`
  * Wake Word: `wn9_hijason_tts2` *(you can choose any of the already available models from esp-sr)*

* **Custom Audio**

  * I2S Pins, Sample Rate: `16kHz`, Channels: `Mono`

* **Custom Network**

  * Enable MQTT or HTTP
  * Server IP, Port, Endpoint/Topic

* **Wi-Fi Credentials**

---

## 🔌 Hardware Requirements

| Component      | Details       |
| -------------- | ------------- |
| ESP32          | Dual-core, S3 |
| I2S Microphone | e.g., INMP441 |
| Server/Backend | MQTT broker   |

---

##  Build & Flash

### 1. Set up ESP-IDF v5.x

```bash
. $HOME/esp/esp-idf/export.sh
```

### 2. Build and Flash

```bash
idf.py build
idf.py -p COM17 flash  
idf.py monitor  

# OR  
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

[binary PCM16LE data]
```

---

##  Example Use Case Flow

```
[Mic] --> [WakeNet detects "hijason"] --> [Start I2S Recording]
     --> [Send Audio Buffer] --> [Cloud AI/NLP backend]
     --> [Respond with Text/Speech/Action]
```

---

##  Development Tips

* Use `.devcontainer/` for VS Code remote development
* Use `.vscode/launch.json` to debug with IDF
* Keep model files small (< 200 KB) for flash safety

---

##  Security Notes

* MQTT not encrypted by default
* Recommend enabling **TLS + authentication** in production
* Validate buffer bounds when modifying components

---

##  Performance

* **WakeNet9 on ESP32**: \~28% avg CPU, \~39% peak
* Optimized task prioritization for wake detection + recording

---

##  Future Enhancements

* [ ] On-device Multinet (command recognition)
* [ ] Automatic TFLite STT pipeline
* [ ] Voice feedback playback (TTS)
* [ ] Power management (light sleep between detections)

---

##  Contributing

We welcome contributions to improve wake word models, add protocols, or integrate new voice features.

---

## License

MIT License. See [`LICENSE`](./LICENSE) for details.

---

## Author

**Abdul Baseer**

> AI | Embedded AI | ESP32 Developer | MERN | Spring Boot
