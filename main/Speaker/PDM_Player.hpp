#ifndef PDM_PLAYER_HPP
#define PDM_PLAYER_HPP

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
    void play(const int16_t *samples, size_t size);
    void play(const std::vector<int16_t> &samples);
    void initialize(const uint32_t freq);
    void deinitialize();
    uint32_t m_freq{};
    const gpio_num_t m_clk_gpio{};
    const gpio_num_t m_cdout_gpio{};

    i2s_pdm_tx_config_t m_pdm_tx_cfg;
    i2s_chan_handle_t m_tx_chan;
};

#endif // PDM_PLAYER_HPP