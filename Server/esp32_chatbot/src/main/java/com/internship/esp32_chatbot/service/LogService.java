package com.internship.esp32_chatbot.service;

import com.internship.esp32_chatbot.model.AudioLog;
import com.internship.esp32_chatbot.repository.AudioLogRepository;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class LogService {

    @Autowired
    private AudioLogRepository logRepo;

    private static int num=1; //access modifiers and static only in classes

    public void saveLog(String deviceId,byte[] AudioInput, String transcript) { 
        try {
            Files.write(Paths.get("output" + (num++) + ".wav"), AudioInput); 
            System.out.println("Saved as output.wav — open in VLC/Windows Media Player"); 
        } catch (IOException e) {
            e.printStackTrace();
        }

        AudioLog log = new AudioLog();
        log.setDeviceId(deviceId); 
        log.setAudioBlob(AudioInput);
        log.setTranscript(transcript);
        logRepo.save(log);
    }
}