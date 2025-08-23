package com.internship.esp32_chatbot.mqtt; 

import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.stereotype.Component;

@Component
public class MqttPublisher {

    private final MqttClient mqttClient;

    public MqttPublisher() throws Exception {
        String broker = "tcp://127.0.0.1:1883"; // Local Broker
        String clientId = "springboot_audio_publisher";
        this.mqttClient = new MqttClient(broker, clientId, null);
        mqttClient.connect();
    }

    public void publishResponse(String deviceId, String message) {
        try {
            String topic = "esp32/response/" + deviceId;
            MqttMessage mqttMessage = new MqttMessage(message.getBytes());
            mqttClient.publish(topic, mqttMessage);
            System.out.println("Published AI reply to: " + topic);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}