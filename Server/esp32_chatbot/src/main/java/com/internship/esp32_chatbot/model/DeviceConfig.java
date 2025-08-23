package com.internship.esp32_chatbot.model; 
 
import jakarta.persistence.Entity; 
import jakarta.persistence.Id; 
 

@Entity
public class DeviceConfig {
    @Id
    private String deviceId;
    private String username;
    private String password;
    private String voiceMode;
    private String aiMode;
    private String location;
    private String wifi_ssid;
    private String wifiPassword; 
     
    public DeviceConfig() {
    }

    // Getters and Setters
    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getVoiceMode() {
        return voiceMode;
    }

    public void setVoiceMode(String voiceMode) {
        this.voiceMode = voiceMode;
    }

    public String getAiMode() {
        return aiMode;
    }

    public void setAiMode(String aiMode) {
        this.aiMode = aiMode;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public String getwifi_ssid() {
        return wifi_ssid;
    }

    public void setwifi_ssid(String wifi_ssid) {
        this.wifi_ssid = wifi_ssid;
    }

    public String getWifiPassword() {
        return wifiPassword;
    }

    public void setWifiPassword(String wifiPassword) {
        this.wifiPassword = wifiPassword;
    }
}