#ifndef MYMQTT_CLIENT_H
#define MYMQTT_CLIENT_H

#include "mqtt_client.h"   // for esp_mqtt_client_handle_t
#include "esp_event.h"     // optional, included for completeness if needed

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize MQTT client and set the message receive callback.
 *
 * This function sets up the MQTT client, connects to the broker, and subscribes
 * to the "/audio/response" topic. When a message is received, the provided callback
 * function is called with topic, data, and length.
 *
 * @param broker_url URI of the MQTT broker (e.g., "mqtt://192.168.1.10:1883")
 * @param callback Function to call when a message is received
 * @return Handle to the initialized MQTT client, or NULL on failure
 */
esp_mqtt_client_handle_t mqtt_init_client(const char *broker_url, void (*callback)(const char *topic, const char *data, int len));

/**
 * @brief Publish audio data to the specified MQTT topic.
 *
 * @param client MQTT client handle
 * @param topic Topic to publish to
 * @param data Raw data buffer to send
 * @param len Length of the data
 * @return 0 on success, -1 on failure
 */
int mqtt_publish_audio(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len);

/**
 * @brief Clean up and stop the MQTT client.
 *
 * @param client MQTT client handle
 */
void mqtt_cleanup_client(esp_mqtt_client_handle_t client); 
 
int send_audio_chunked(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len);

#ifdef __cplusplus
}
#endif

#endif // MYMQTT_CLIENT_H
