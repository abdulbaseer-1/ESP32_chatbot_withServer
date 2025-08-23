package com.internship.esp32_chatbot.model;

import jakarta.persistence.*;
import java.time.LocalDateTime;

@Entity
public class AudioLog {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    private String deviceId;

    @Lob
    private String transcript;

    private LocalDateTime timestamp; 
     
    @Lob 
    private byte[] AudioInput;

    // Constructors, Getters, Setters
    public AudioLog() {
        this.timestamp = LocalDateTime.now();
    }
  
    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    } 
     
    public String getTranscript() {
        return transcript;
    } 
     
    public void setTranscript(String transcript) {
        this.transcript = transcript;
    }

    public Long getId() {
        return id;
    }
    
    public void setAudioBlob(byte[] AudioInput) { 
        this.AudioInput = AudioInput;
    } 
    
    public byte[] getAudioBlob() { 
        return AudioInput;
    } 


    public LocalDateTime getTimestamp() {
        return timestamp;
    }

}
