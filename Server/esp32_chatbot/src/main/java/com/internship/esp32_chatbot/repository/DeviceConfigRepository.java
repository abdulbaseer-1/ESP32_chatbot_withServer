package com.internship.esp32_chatbot.repository;

import com.internship.esp32_chatbot.model.DeviceConfig;
import org.springframework.data.jpa.repository.JpaRepository;

public interface DeviceConfigRepository extends JpaRepository<DeviceConfig, String> {
    DeviceConfig findByDeviceId(String deviceId);
}
