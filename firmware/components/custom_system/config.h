#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations
void load_config(void);

// Main constants

#define WIFI_SSID      "Imtiaz"/*---"AzlanKhan31"---"G12 Cam""NUTECH-Student"*/
#define WIFI_PASS      "imt12345"/*---"walikhan9"---"isbtechtechisb123""Nu@tech#911"*/
#define MQTT_CHUNK_SIZE 4096  
 
#define mqtt_url  "mqtt://10.4.12.179:1883"

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
