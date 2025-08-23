#include "esp_wn_models.h" 
#include "wakeword.h"
#include "mic_i2s.h"
#include "esp_log.h"
#include "model_path.h" // Replaced esp_srmodel.h with model_path.h
#include "esp_wn_iface.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "WakeMultinet";

#define SAMPLE_RATE     16000
#define BUFFER_SIZE     512
#define WAKE_MODE       DET_MODE_95
#define CMD_TIMEOUT     40  // ~0.5s @ 16kHz

static wakeword_callback_t ww_callback = NULL;

static srmodel_list_t *sr_models = NULL;
static const esp_wn_iface_t *wakenet = NULL;
static model_iface_data_t *wn_handle = NULL;

void wakeword_init(wakeword_callback_t wake_cb) {
    ww_callback = wake_cb;

    // Initialize model list from partition
    sr_models = esp_srmodel_init("model");
    if (!sr_models) {
        ESP_LOGE(TAG, "Failed to init model list");
        return;
    }

    // Initialize WakeNet
    char *wn_name = esp_srmodel_filter(sr_models, ESP_WN_PREFIX, NULL);
    if (!wn_name) {
        ESP_LOGE(TAG, "No WakeNet model found");
        esp_srmodel_deinit(sr_models);
        return;
    } 

    int wn_index = esp_srmodel_exists(sr_models, wn_name);
     if (wn_index < 0 || wn_index >= sr_models->num) {
        ESP_LOGE(TAG, "WakeNet model %s not found in list", wn_name);
        esp_srmodel_deinit(sr_models);
        return;
    } 
    
    //srmodel_data_t *wn_data = sr_models->model_data[wn_index];//dev says dont need this ,just add name

    wakenet = esp_wn_handle_from_name(wn_name);
    if (!wakenet) {
        ESP_LOGE(TAG, "Failed to get WakeNet handle for %s", wn_name);
        esp_srmodel_deinit(sr_models);
        return;
    } 
 
    //for debugging 
    ESP_LOGI(TAG, "Using WakeNet: %s", wn_name); 

    wn_handle = wakenet->create(wn_name, WAKE_MODE);
    if (!wn_handle) {
        ESP_LOGE(TAG, "WakeNet creation failed for %s", wn_name);
        esp_srmodel_deinit(sr_models);
        return;
    }

    ESP_LOGI(TAG, "WakeNet (%s) and MultiNet (%s) initialized", wn_name, "placeholder");//mn_name
}

void wakeword_task(void *arg) {
    int16_t *buffer = calloc(BUFFER_SIZE, sizeof(int16_t));
    if (!buffer) {
        ESP_LOGE(TAG, "Buffer allocation failed");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        int bytes_read = i2s_mic_read((char *)buffer, BUFFER_SIZE * sizeof(int16_t));
        if (bytes_read <= 0) continue;

        if (wakenet->detect(wn_handle, buffer) == WAKENET_DETECTED) {
            ESP_LOGI(TAG, "Wake word detected");
            if (ww_callback) ww_callback();
        }
    }

    // Cleanup (not usually reached)
    free(buffer);
    wakenet->destroy(wn_handle);
    esp_srmodel_deinit(sr_models);
    vTaskDelete(NULL);
} 