package com.internship.esp32_chatbot.controller;

import com.internship.esp32_chatbot.model.DeviceConfig;
import com.internship.esp32_chatbot.service.DeviceConfigService;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController; 
 
//Rest controller does not render
@RestController
@RequestMapping("/device-config")
public class DeviceConfigController {
    private final DeviceConfigService configService;

    public DeviceConfigController(DeviceConfigService configService) {
        this.configService = configService;
    }

    @PostMapping("/save")
    public ResponseEntity<DeviceConfig> saveConfig(@RequestBody DeviceConfig config) {
        DeviceConfig saved = configService.saveConfig(config);
        return ResponseEntity.ok(saved);
    }

    @GetMapping("/{deviceId}")
    public ResponseEntity<DeviceConfig> getConfig(@PathVariable String deviceId) {
        return configService.getConfig(deviceId)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }
}
 
// Thymeleaf Page Controller
@Controller
class DashboardController {
    @GetMapping("/dashboard")
    public String showDashboard() {
        return "device-config";  // device-config.html in templates folder
    }
}