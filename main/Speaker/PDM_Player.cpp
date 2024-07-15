#include "pdm_player.hpp"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

#define DEFAULT_FREQ 48'000
#define TX_CHUNK_LENGTH_MS 5
#define TX_CHUNK_LENGTH(FREQ) (TX_CHUNK_LENGTH_MS * FREQ / 1000)
#define TX_CHUNK_LENGTH_IN_BYTES(FREQ) (TX_CHUNK_LENGTH_MS * FREQ / 1000) * (sizeof(int16_t))

PDM_Player::PDM_Player(gpio_num_t clk_io, gpio_num_t data_io) : clk_gpio_{clk_io}, dout_gpio_{data_io} {
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan_handler_, NULL));
    pdm_tx_cfg_ = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(DEFAULT_FREQ),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = clk_gpio_,
            .dout = dout_gpio_,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(tx_chan_handler_, &pdm_tx_cfg_));
}

void PDM_Player::play(SoundFile *file) {
    assert(file);

    switch (file->soundFileType()) {
        case eSoundFiles::SOUND_FILE_WAV:
            playWavFile(file);
            break;
        case eSoundFiles::SOUND_FILE_MP3:
            playMp3File(file);
            break;
        default:
            assert(false);
            break;
    }
}

void PDM_Player::playMp3File(SoundFile *file) {
    assert(file);
    SampleChunk chunk{file->getNextSampleChunk()};
    initialize(chunk.sample_rate);
    while(chunk.length > 0) {
        play(chunk.samples, chunk.length);
        chunk = file->getNextSampleChunk();
    }
    deinitialize();
    file->reset();
}

void PDM_Player::playWavFile(SoundFile *file) {
    assert(file);
    SampleChunk chunk{file->getNextSampleChunk()};
    initialize(chunk.sample_rate);
    play(chunk.samples, chunk.length);
    file->reset();
    deinitialize();
}

void PDM_Player::play(const int16_t *samples, size_t size) {
    size_t bytes_sent{};
    const size_t bytes_to_sent{size * sizeof(int16_t)};
    const uint8_t *data_to_send{reinterpret_cast<const uint8_t *>(samples)};
    while (bytes_sent != bytes_to_sent)
    {
        size_t bytes_written{};
        const size_t length{(bytes_to_sent - bytes_sent) >= TX_CHUNK_LENGTH_IN_BYTES(freq_) ? TX_CHUNK_LENGTH_IN_BYTES(freq_) : (bytes_to_sent - bytes_sent)};
        ESP_ERROR_CHECK(i2s_channel_write(tx_chan_handler_, (void *)(data_to_send + bytes_sent), length, &bytes_written, portMAX_DELAY));
        bytes_sent += bytes_written;
    }
}

void PDM_Player::play(const std::vector<int16_t> &buff) {
    play(buff.data(), buff.size());
}

void PDM_Player::initialize(const uint32_t freq) {
    ESP_LOGI(TAG, "initializing with freq = %lu", freq);
    freq_ = freq;
    pdm_tx_cfg_ = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(freq),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = clk_gpio_,
            .dout = dout_gpio_,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };

    i2s_pdm_tx_clk_config_t clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(freq);

    ESP_ERROR_CHECK(i2s_channel_reconfig_pdm_tx_clock(tx_chan_handler_, &clk_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan_handler_));
}

void PDM_Player::deinitialize() {
    ESP_ERROR_CHECK(i2s_channel_disable(tx_chan_handler_));
}
