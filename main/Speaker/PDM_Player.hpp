#pragma once

#include "wav_file/wav_file_reader.hpp"

#include "driver/i2s_pdm.h"
#include "driver/gpio.h"

class PDM_Player
{
public:
    PDM_Player(gpio_num_t clk_io, gpio_num_t data_io);
    void play(uint8_t *buff, size_t size);
    void play(const char *file_path);

private:
    i2s_pdm_tx_config_t m_pdm_tx_cfg;
    i2s_chan_handle_t m_tx_chan; // I2S tx channel handler

    void init(uint32_t sample_rate, uint32_t bit_width, uint32_t channel_num);
    void play(uint16_t *buff, size_t size);
};