#include "speaker_i2s.h"
#include "driver/i2s_std.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <portmacro.h>

#define SAMPLE_RATE     16000
#define I2S_PORT        0
#define PI              3.14159265

static i2s_chan_handle_t tx_handle = NULL;

void i2s_speaker_init(void) {
    // Allocate TX channel
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER); 
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    // Standard mode configuration
    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = I2S_STD_SLOT_LEFT,
            .ws_width = 16,
            .ws_pol = false,
            .bit_shift = true,
        },
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = 10,
            .ws = 11,
            .dout = 12,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
}

void i2s_speaker_play_sine_wave() {
    const int samples = 200;
    int16_t buffer[samples];

    for (int i = 0; i < samples; ++i) {
        float sample = sinf((2.0f * PI * i) / samples);
        buffer[i] = (int16_t)(sample * 32767);
    }

    for (int i = 0; i < 100; ++i) {
        size_t bytes_written = 0;
        i2s_channel_write(tx_handle, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    }
}
