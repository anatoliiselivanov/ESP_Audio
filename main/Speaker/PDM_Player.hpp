#pragma once

#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "sound_file.hpp"

#include <vector>

class PDM_Player
{
public:
    PDM_Player(gpio_num_t clk_io, gpio_num_t data_io);
    void play(SoundFile *file);
private:
    static constexpr const char* const TAG{"PDM_Player"};

    void playMp3File(SoundFile *file);
    void playWavFile(SoundFile *file);
    
    void play(const int16_t *samples, size_t size);
    void play(const std::vector<int16_t> &samples);
    void initialize(const uint32_t freq);
    void deinitialize();
    uint32_t freq_{};
    const gpio_num_t clk_gpio_{};
    const gpio_num_t dout_gpio_{};

    i2s_pdm_tx_config_t pdm_tx_cfg_;
    i2s_chan_handle_t tx_chan_handler_;
};
