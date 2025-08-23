#include "mic_i2s.h"
#include "driver/i2s_std.h"
#include "esp_log.h"
#include <math.h>
#include <string.h>
#include <portmacro.h>

static const char *TAG = "MIC_I2S";
i2s_chan_handle_t rx_chan; // RX channel handle

// ------------------------
// High-pass filter state
// ------------------------
typedef struct {
    float a0, a1, b1;
    float prev_input;
    float prev_output;
} hp_filter_t;

static hp_filter_t hp;

// ------------------------
// Noise gate parameters
// ------------------------
#define NOISE_GATE_THRESHOLD  1000.0f  // Adjust based on your mic/noise level
#define NOISE_GATE_HYSTERESIS 0.8f      // Prevent rapid on/off gating

static int gate_open = 0;

// ------------------------
// Initialize high-pass filter
// fc: cutoff frequency in Hz
// fs: sampling rate in Hz
// ------------------------
static void hp_filter_init(hp_filter_t *filt, float fc, float fs) {
    float c = tanf(M_PI * fc / fs);
    float a0_inv = 1.0f / (1.0f + c);
    filt->a0 = a0_inv;
    filt->a1 = -a0_inv;
    filt->b1 = (1.0f - c) * a0_inv;
    filt->prev_input = 0;
    filt->prev_output = 0;
}

// ------------------------
// Apply high-pass filter
// ------------------------
static inline float hp_filter_apply(hp_filter_t *filt, float input) {
    float output = filt->a0 * input + filt->a1 * filt->prev_input + filt->b1 * filt->prev_output;
    filt->prev_input = input;
    filt->prev_output = output;
    return output;
}

// ------------------------
// I2S mic init
// ------------------------
void i2s_mic_init(void) {
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_chan));

    i2s_std_config_t std_cfg = {
        .clk_cfg    = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg   = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(
                          I2S_DATA_BIT_WIDTH_32BIT,
                          I2S_SLOT_MODE_MONO),
        .gpio_cfg   = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = 7,
            .ws   = 16,
            .dout = I2S_GPIO_UNUSED,
            .din  = 15
        }
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));
    ESP_LOGI(TAG, "I2S STD RX channel initialized @16kHz mono");

    // Init high-pass filter at 120 Hz cutoff
    hp_filter_init(&hp, 120.0f, 16000.0f);
}

// ------------------------
// Read mic + apply HPF + Noise Gate
// Returns bytes written into buf, or 0 if gated (silence)
// ------------------------
int i2s_mic_read(void *buf, int len) {
    if (!rx_chan) {
        ESP_LOGE(TAG, "I2S channel not initialized");
        return -1;
    }

    // Calculate how many 32-bit samples we need
    int samples_needed = len / sizeof(int16_t);
    size_t raw_buffer_size = samples_needed * sizeof(int32_t);
    
    // Allocate temporary buffer for raw 32-bit data
    int32_t *raw_buffer = malloc(raw_buffer_size);
    if (!raw_buffer) {
        ESP_LOGE(TAG, "Failed to allocate raw buffer");
        return -1;
    }

    size_t bytes_read = 0;
    esp_err_t err = i2s_channel_read(rx_chan, raw_buffer, raw_buffer_size, &bytes_read, portMAX_DELAY);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2s_channel_read error %d", err);
        free(raw_buffer);
        return -1;
    }

    // Convert raw 32-bit samples to 16-bit and store in output buffer
    int16_t *samples = (int16_t *)buf;
    int sample_count = bytes_read / sizeof(int32_t);
    
    float peak = 0.0f;

    for (int i = 0; i < sample_count; i++) {
        // Convert 32-bit to 16-bit
        int16_t sample16 = (int16_t)(raw_buffer[i] >> 14);
        
        // Apply HPF
        float y = hp_filter_apply(&hp, (float)sample16);
        samples[i] = (int16_t)y;

        // Track peak for noise gate decision
        float abs_val = fabsf(y);
        if (abs_val > peak) {
            peak = abs_val;
        }
    }

    free(raw_buffer);

    // Noise gate logic
    int output_bytes = sample_count * sizeof(int16_t);
    if (gate_open) {
        if (peak < NOISE_GATE_THRESHOLD * NOISE_GATE_HYSTERESIS) {
            gate_open = 0;
            memset(buf, 0, output_bytes);
            return 0;
        }
    } else {
        if (peak > NOISE_GATE_THRESHOLD) {
            gate_open = 1;
        } else {
            memset(buf, 0, output_bytes);
                return 0;
        }
    }

    return output_bytes;
}