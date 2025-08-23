// Mymqtt_client.c
#include "esp_log.h"
#include "Mymqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h" 
 
//repurposed from the one built for esp32s3
 
#define MQTT_CHUNK_SIZE 1024  // safe chunk size to avoid outbox overflow

static const char *TAG = "MQTT_CLIENT";

// Callback function to handle received messages
static void (*message_callback)(const char *topic, const char *data, int len) = NULL;

// Mutex for thread-safe publishing
static SemaphoreHandle_t mqtt_publish_mutex = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to MQTT broker");
            esp_mqtt_client_subscribe(client, "/audio/response", 1);
            ESP_LOGI(TAG, "Subscribed to topic: /audio/response");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from MQTT broker");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Subscribed, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT message published, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Received data on topic: %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Data length: %d", event->data_len);
            if (message_callback) {
                message_callback(event->topic, event->data, event->data_len);
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error occurred");
            break;
        default:
            ESP_LOGI(TAG, "Other MQTT event id:%d", event->event_id);
            break;
    }
}

esp_mqtt_client_handle_t mqtt_init_client(const char *broker_url, void (*callback)(const char *topic, const char *data, int len)) {
    message_callback = callback;

    if (mqtt_publish_mutex == NULL) {
        mqtt_publish_mutex = xSemaphoreCreateMutex();
        if (mqtt_publish_mutex == NULL) {
            ESP_LOGE(TAG, "Failed to create MQTT mutex");
            return NULL;
        }
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return NULL;
    }

    esp_err_t err = esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register MQTT event handler: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(client);
        return NULL;
    }

    err = esp_mqtt_client_start(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(client);
        return NULL;
    }

    return client;
}

int mqtt_publish_audio(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len) {
    if (client == NULL || topic == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Invalid arguments to mqtt_publish_audio");
        return -1;
    }

    if (mqtt_publish_mutex == NULL) {
        ESP_LOGE(TAG, "MQTT publish mutex is NULL");
        return -1;
    }

    if (xSemaphoreTake(mqtt_publish_mutex, portMAX_DELAY) == pdTRUE) {
        int msg_id = esp_mqtt_client_publish(client, topic, data, len, 1, 0);
        xSemaphoreGive(mqtt_publish_mutex);

        if (msg_id < 0) {
            ESP_LOGE(TAG, "Failed to publish audio data");
            return -1;
        }

        ESP_LOGI(TAG, "Published audio data to topic: %s, msg_id=%d", topic, msg_id);
        return 0;
    } else {
        ESP_LOGE(TAG, "Failed to acquire MQTT publish mutex");
        return -1;
    }
}

void mqtt_cleanup_client(esp_mqtt_client_handle_t client) {
    if (client) {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
        ESP_LOGI(TAG, "MQTT client stopped and destroyed");
    }

    if (mqtt_publish_mutex) {
        vSemaphoreDelete(mqtt_publish_mutex);
        mqtt_publish_mutex = NULL;
    }
}
 
int send_audio_chunked(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len) {
    if (client == NULL || topic == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Invalid arguments to send_audio_chunked");
        return -1;
    }

    int offset = 0;
    while (offset < len) {
        int chunk_len = (len - offset) > MQTT_CHUNK_SIZE ? MQTT_CHUNK_SIZE : (len - offset);

        if (xSemaphoreTake(mqtt_publish_mutex, portMAX_DELAY) == pdTRUE) {
            int msg_id = esp_mqtt_client_publish(client, topic, data + offset, chunk_len, 1, 0);
            xSemaphoreGive(mqtt_publish_mutex);

            if (msg_id < 0) {
                ESP_LOGE(TAG, "Failed to publish chunk at offset %d", offset);
                return -1;
            }

            ESP_LOGI(TAG, "Published chunk: offset=%d, size=%d, msg_id=%d", offset, chunk_len, msg_id);
        } else {
            ESP_LOGE(TAG, "Failed to acquire MQTT publish mutex");
            return -1;
        }

        offset += chunk_len;

        // Small delay to allow network to send and free outbox
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    ESP_LOGI(TAG, "All chunks published successfully, total size=%d", len);
    return 0;
}