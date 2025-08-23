CREATE TABLE IF NOT EXISTS audio_log (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(255) NOT NULL, 
    audio_input MEDIUMBLOB,
    transcript TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
 
CREATE TABLE IF NOT EXISTS DeviceConfig ( 
    device_id VARCHAR(100) PRIMARY KEY, 
    username VARCHAR(100),
    password VARCHAR(100),
    voice_mode VARCHAR(10),
    ai_mode VARCHAR(20),
    location VARCHAR(100),
    wifi_ssid VARCHAR(100),
    wifi_password VARCHAR(100)
); 