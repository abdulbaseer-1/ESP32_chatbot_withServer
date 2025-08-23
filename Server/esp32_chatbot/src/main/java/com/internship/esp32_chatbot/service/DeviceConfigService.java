package com.internship.esp32_chatbot.service;

import com.internship.esp32_chatbot.model.DeviceConfig;
import com.internship.esp32_chatbot.repository.DeviceConfigRepository; 
import org.springframework.stereotype.Service; 
 
import java.util.Optional; 
 
@Service 
public class DeviceConfigService {
    private final DeviceConfigRepository repository;

    public DeviceConfigService(DeviceConfigRepository repository) {
        this.repository = repository;
    }

    public DeviceConfig saveConfig(DeviceConfig config) {
        return repository.save(config);
    }

    public Optional<DeviceConfig> getConfig(String deviceId) {
        return repository.findById(deviceId);
    } 
}