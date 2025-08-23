#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h" 
#include "mic_i2s.h"
#include "speaker_i2s.h"
#include "wakeword.h"
#include "Mymqtt_client.h"
#include "wifi.h" 
#include "esp_log.h"  
//To add later for commercial product  
#include "config.h"
//#include "system/ota.h" // 


 
  
#define TAG "MAIN"  
 
static esp_mqtt_client_handle_t mqtt_client = NULL; 
 

void send_audio_to_server(void* param)
{
    const int record_time_sec = 5;
    const int sample_rate = 16000;
    const int channels = 1;

    const int bytes_per_sample_out = 2;  // 16-bit PCM we send

    const int total_samples = sample_rate * record_time_sec * channels;
    const int total_out_bytes = total_samples * bytes_per_sample_out;

    // samples per MQTT chunk and max outgoing bytes
    const int CHUNK_SAMPLES = MQTT_CHUNK_SIZE / bytes_per_sample_out;
    const int out_chunk_bytes_max = CHUNK_SAMPLES * bytes_per_sample_out;

    // publish buffer (PCM16)
    uint8_t *publish_buf = malloc(out_chunk_bytes_max);
    if (!publish_buf) {
        ESP_LOGE(TAG, "publish_buf allocation failed (%d bytes)", out_chunk_bytes_max);
        vTaskDelete(NULL);
        return;
    }

    // publish meta
    uint8_t meta[4];
    meta[0] = (total_out_bytes) & 0xFF;
    meta[1] = (total_out_bytes >> 8) & 0xFF;
    meta[2] = (total_out_bytes >> 16) & 0xFF;
    meta[3] = (total_out_bytes >> 24) & 0xFF;
    char meta_topic[64];
    const char *device_id = "testDevice";
    snprintf(meta_topic, sizeof(meta_topic), "esp32/audio/%s/meta", device_id);
    if (mqtt_publish_audio(mqtt_client, meta_topic, (const char*)meta, sizeof(meta)) != 0) {
        ESP_LOGW(TAG, "Failed to publish meta; continuing anyway");
    } else {
        ESP_LOGI(TAG, "Published meta total_out_bytes=%d", total_out_bytes);
    }
 
    // streaming loop
    int samples_sent = 0;
    while (samples_sent < total_samples) {
        // how many samples we want in this chunk
        int samples_to_request = CHUNK_SAMPLES;
        if ((total_samples - samples_sent) < samples_to_request) {
            samples_to_request = (total_samples - samples_sent);
        }

        int bytes_to_read = samples_to_request * bytes_per_sample_out;
        int bytes_read = 0;
        int attempts = 0;

        // fill chunk by repeatedly calling loop_read_and_resample()
        while (bytes_read < bytes_to_read && attempts < 50) {
            int r = i2s_mic_read(publish_buf + bytes_read, bytes_to_read - bytes_read);
            if (r < 0) {
                ESP_LOGE(TAG, "loop_read_and_resample error %d", r);
                break;
            } else if (r == 0) {
                // no data available right now
                vTaskDelay(pdMS_TO_TICKS(5));
                attempts++;
                continue;
            }
            bytes_read += r;
        }

        if (bytes_read <= 0) {
            ESP_LOGW(TAG, "No bytes read for this chunk, aborting");
            break;
        }

        int got_samples = bytes_read / bytes_per_sample_out;
        int out_bytes = bytes_read;

        char data_topic[64];
        snprintf(data_topic, sizeof(data_topic), "esp32/audio/%s", device_id);

        int rc = mqtt_publish_audio(mqtt_client, data_topic, (const char*)publish_buf, out_bytes);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed publish chunk (rc=%d)", rc);
        } else {
            ESP_LOGI(TAG, "Published chunk: samples=%d bytes=%d total_sent_samples=%d/%d",
                     got_samples, out_bytes, samples_sent + got_samples, total_samples);
        }

        samples_sent += got_samples;
        vTaskDelay(pdMS_TO_TICKS(5));
    } 
     
    //wav_stop();

    ESP_LOGI(TAG, "Streaming finished: samples_sent=%d total_samples=%d", samples_sent, total_samples);
    free(publish_buf);
    vTaskDelete(NULL);
}

static void wakeword_detected_callback() {
    ESP_LOGI(TAG, "Wake word callback triggered");
    xTaskCreate(&send_audio_to_server, "send_audio_to_server", 8192, NULL, 5, NULL);
} 

void mqtt_message_handler(const char *topic, const char *data, int len) {
    ESP_LOGI("MQTT_CB", "Received topic: %s", topic);
    ESP_LOGI("MQTT_CB", "Payload: %.*s", len, data);
} 

void app_main(void)
{ 
    ESP_LOGI(TAG, "Starting application");

    // Initialize Wi-Fi 
    wifi_init_sta();

    // Initialize MQTT client 
    mqtt_client = mqtt_init_client(mqtt_url, mqtt_message_handler);

    if (!mqtt_client) {
        ESP_LOGE(TAG, "MQTT client init failed");
    }


    // Initialize I2S microphone 
    ESP_LOGI(TAG, "Starting audio recording...");
    i2s_mic_init();
    i2s_speaker_init(); 
    i2s_speaker_play_sine_wave();
     
    // Initialize wake-word detection
    wakeword_init(wakeword_detected_callback);  //, NULL // Init WakeNet

    xTaskCreate(&wakeword_task, "wakeword_task", 8192, NULL, 5, NULL);

    // Initialize OTA
    // ota_init();

    // Periodically check for OTA updates
    // Consider creating a separate task for OTA checks
 
//-------------------------------------------------------------------------------------------------------------------------------
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
     
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
}
