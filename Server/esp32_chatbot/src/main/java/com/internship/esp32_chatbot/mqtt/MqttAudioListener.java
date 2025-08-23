package com.internship.esp32_chatbot.mqtt;

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.stereotype.Component;

import com.internship.esp32_chatbot.service.AudioProcessingService;

import jakarta.annotation.PostConstruct;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

@Component
public class MqttAudioListener {

    private final AudioProcessingService audioProcessingService;
    private final MqttPublisher mqttPublisher;

    // Store audio chunks for each device
    private final Map<String, List<byte[]>> audioBuffers = new ConcurrentHashMap<>();

    // Map device -> expected number of chunks (populated from meta message)
    private final Map<String, Integer> expectedChunksMap = new ConcurrentHashMap<>();
    private final int CHUNK_SIZE = 4096; // must match MQTT_CHUNK_SIZE on ESP

    public MqttAudioListener(AudioProcessingService audioProcessingService, MqttPublisher mqttPublisher) {
        this.audioProcessingService = audioProcessingService;
        this.mqttPublisher = mqttPublisher;
    }

    @PostConstruct
    public void init() {
        String broker = "tcp://10.4.12.179:1883";
        String clientId = "springboot_audio_listener";
        // subscribe to all subtopics under esp32/audio so we can receive meta and chunks
        String topic = "esp32/audio/#";

        try {
            MqttClient mqttClient = new MqttClient(broker, clientId, null);
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            mqttClient.connect(options);

            mqttClient.subscribe(topic, (t, message) -> {
                try {
                    // Topic examples:
                    //  - esp32/audio/<deviceId>           (chunks)
                    //  - esp32/audio/<deviceId>/meta      (4-byte total length)
                    String deviceId = extractDeviceId(t);
                    if (deviceId == null || deviceId.isEmpty()) {
                        System.out.println("Received topic with no device id: " + t);
                        return;
                    }

                    // Handle meta message (topic ends with "/meta")
                    if (t.endsWith("/meta")) {
                        byte[] payload = message.getPayload();
                        if (payload == null || payload.length < 4) {
                            System.out.println("Invalid meta payload for device " + deviceId);
                            return;
                        }
                        ByteBuffer bb = ByteBuffer.wrap(payload).order(ByteOrder.LITTLE_ENDIAN);
                        int totalBytes = bb.getInt();
                        int expectedChunks = (totalBytes + CHUNK_SIZE - 1) / CHUNK_SIZE;
                        expectedChunksMap.put(deviceId, expectedChunks);
                        // reset buffer for this recording
                        audioBuffers.put(deviceId, new ArrayList<>());
                        System.out.println("Meta: expecting " + expectedChunks + " chunks for " + deviceId + " (totalBytes=" + totalBytes + ")");
                        return;
                    }

                    // Otherwise treat as audio chunk
                    byte[] chunk = message.getPayload();
                    if (chunk == null || chunk.length == 0) {
                        System.out.println("Got empty chunk for device " + deviceId);
                        return;
                    }

                    audioBuffers.computeIfAbsent(deviceId, k -> new ArrayList<>()).add(chunk);
                    System.out.println("⚡ Received chunk from device " + deviceId + " (" + chunk.length + " bytes). Total chunks buffered: " + audioBuffers.get(deviceId).size());

                    Integer expected = expectedChunksMap.get(deviceId);
                    if (expected != null && audioBuffers.get(deviceId).size() >= expected) {
                        System.out.println("✅ All expected chunks received for device: " + deviceId);

                        // Merge chunks into one byte array
                        byte[] fullAudio = mergeChunks(audioBuffers.get(deviceId));

                        // Cleanup
                        expectedChunksMap.remove(deviceId);
                        audioBuffers.remove(deviceId);

                        // Process audio (transcribe / log / respond)
                        try {
                            String aiReply = audioProcessingService.processAudioFromMQTT(fullAudio, deviceId);
                            mqttPublisher.publishResponse(deviceId, aiReply);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    } else {
                        // If we don't have expected count yet, just wait for more chunks
                        // Optionally: implement a timeout to discard partial uploads after X seconds
                    }
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            });

            System.out.println("Subscribed to topic: " + topic);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private String extractDeviceId(String topic) {
        // topic forms: "esp32/audio/{deviceId}" or "esp32/audio/{deviceId}/meta"
        String[] parts = topic.split("/");
        if (parts.length >= 3) {
            return parts[2];
        }
        return null;
    }

    private byte[] mergeChunks(List<byte[]> chunks) {
        int totalLength = chunks.stream().mapToInt(c -> c.length).sum();
        byte[] merged = new byte[totalLength];
        int pos = 0;
        for (byte[] c : chunks) {
            System.arraycopy(c, 0, merged, pos, c.length);
            pos += c.length;
        }
        System.out.println("Merged bytes length = " + merged.length + ". Estimated duration(s) = " + (merged.length / (16000.0 * 2.0)));
        return merged;
    }
}
