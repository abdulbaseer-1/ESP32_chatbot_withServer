#ifndef LOG_H
#define LOG_H

#define LOGI(tag, fmt, ...) ESP_LOGI(tag, fmt, ##__VA_ARGS__)
#define LOGE(tag, fmt, ...) ESP_LOGE(tag, fmt, ##__VA_ARGS__)

#endif
