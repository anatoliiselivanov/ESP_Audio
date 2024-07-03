#include "pdm_player.hpp"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

// #define PDM_TX_FREQ_HZ 44100    // I2S PDM TX frequency
#define PDM_TX_FREQ_HZ 8000     // I2S PDM TX frequency
#define WAVE_AMPLITUDE (1000.0) // 1~32767
#define CONST_PI (3.1416f)
#define SINE_WAVE_LEN(tone) (uint32_t)((PDM_TX_FREQ_HZ / (float)tone) + 0.5) // The sample point number per sine wave to generate the tone
#define TONE_LAST_TIME_MS 500
#define BYTE_NUM_EVERY_TONE (TONE_LAST_TIME_MS * PDM_TX_FREQ_HZ / 1000)

PDM_Player::PDM_Player(gpio_num_t clk_io, gpio_num_t data_io)
{
    m_pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(PDM_TX_FREQ_HZ),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = clk_io,
            .dout = data_io,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };

    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &m_tx_chan, NULL));

    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(m_tx_chan, &m_pdm_tx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(m_tx_chan));
}

void PDM_Player::play(const uint8_t *buff, size_t size)
{
    size_t bytes_sent = 0;
    for (size_t to_sent = 0; to_sent < size;)
    {
        ESP_ERROR_CHECK(i2s_channel_write(m_tx_chan, (void *)(buff + to_sent), 100 * sizeof(uint16_t), &bytes_sent, portMAX_DELAY));
        to_sent += bytes_sent;
    }
}

void PDM_Player::play(const std::vector<int16_t> &buff)
{
    for (const auto &val : buff)
    {
        ESP_ERROR_CHECK(i2s_channel_write(m_tx_chan, &val, sizeof(uint16_t), NULL, portMAX_DELAY));
    }
}
