#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include "pdm_player.hpp"

#define PDM_TX_FREQ_HZ 44100    // I2S PDM TX frequency
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
}

void PDM_Player::play(const uint8_t *buff, size_t size)
{
    WavFileReader wav_file(buff, size);
    if (!wav_file.is_valid())
    {
        ESP_LOGE("PDM_Player", "Invalid wav file");
        return;
    }
    init(wav_file.sample_rate(), wav_file.bits_per_sample(), wav_file.num_of_channels());
    play(wav_file.data(), wav_file.data_size());
}

void PDM_Player::init(uint32_t sample_rate = PDM_TX_FREQ_HZ, uint32_t bit_width = I2S_DATA_BIT_WIDTH_16BIT, uint32_t channel_num = I2S_SLOT_MODE_MONO)
{
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &m_tx_chan, NULL));

    m_pdm_tx_cfg.clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(sample_rate);
    m_pdm_tx_cfg.slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(i2s_data_bit_width_t(bit_width), i2s_slot_mode_t(channel_num));

    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(m_tx_chan, &m_pdm_tx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(m_tx_chan));
}

void PDM_Player::play(const uint16_t *buff, size_t size)
{
    ESP_ERROR_CHECK(i2s_channel_write(m_tx_chan, buff, size, NULL, portMAX_DELAY));
}
