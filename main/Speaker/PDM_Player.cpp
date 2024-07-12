#include "pdm_player.hpp"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define DEFAULT_FREQ 48'000
#define TX_CHUNK_LENGTH_MS 5
#define TX_CHUNK_LENGTH(FREQ) (TX_CHUNK_LENGTH_MS * FREQ / 1000)
#define TX_CHUNK_LENGTH_IN_BYTES(FREQ) (TX_CHUNK_LENGTH_MS * FREQ / 1000) * (sizeof(int16_t))

PDM_Player::PDM_Player(gpio_num_t clk_io, gpio_num_t data_io) : m_clk_gpio{clk_io}, m_cdout_gpio{data_io}
{

    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &m_tx_chan, NULL));
    m_pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(DEFAULT_FREQ),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = m_clk_gpio,
            .dout = m_cdout_gpio,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(m_tx_chan, &m_pdm_tx_cfg));
}

void PDM_Player::play(SoundFile *file)
{
    assert(file);

    initialize(file->sample_rate());

    size_t buff_size = TX_CHUNK_LENGTH_IN_BYTES(file->sample_rate());
    SoundFile::SampleT *buff = new SoundFile::SampleT[buff_size];
    size_t samples_read = 0;

    while (samples_read >= buff_size)
    {
        samples_read = file->read(buff, buff_size);
        play(buff, samples_read);
    }

    deinitialize();
}

void PDM_Player::play(const int16_t *samples, size_t size)
{
    size_t bytes_sent{};
    const size_t bytes_to_sent{size * sizeof(int16_t)};
    const uint8_t *data_to_send{reinterpret_cast<const uint8_t *>(samples)};
    while (bytes_sent != bytes_to_sent)
    {
        size_t bytes_written{};
        const size_t length{(bytes_to_sent - bytes_sent) >= TX_CHUNK_LENGTH_IN_BYTES(m_freq) ? TX_CHUNK_LENGTH_IN_BYTES(m_freq) : (bytes_to_sent - bytes_sent)};
        ESP_ERROR_CHECK(i2s_channel_write(m_tx_chan, (void *)(data_to_send + bytes_sent), length, &bytes_written, portMAX_DELAY));
        bytes_sent += bytes_written;
    }
}

void PDM_Player::play(const std::vector<int16_t> &buff)
{
    play(buff.data(), buff.size());
}

void PDM_Player::initialize(const uint32_t freq)
{

    m_freq = freq;
    m_pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(freq),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = m_clk_gpio,
            .dout = m_cdout_gpio,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };

    i2s_pdm_tx_clk_config_t clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(freq);

    ESP_ERROR_CHECK(i2s_channel_reconfig_pdm_tx_clock(m_tx_chan, &clk_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(m_tx_chan));
}

void PDM_Player::deinitialize()
{
    ESP_ERROR_CHECK(i2s_channel_disable(m_tx_chan));
}
