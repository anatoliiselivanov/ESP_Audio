#ifndef PDM_PLAYER_HPP
#define PDM_PLAYER_HPP

#include "driver/i2s_pdm.h"
#include "driver/gpio.h"

#include <vector>

class PDM_Player
{
public:
    PDM_Player(gpio_num_t clk_io, gpio_num_t data_io);
    void play(const uint8_t *buff, size_t size);
    void play(const std::vector<int16_t> &buff);

private:
    i2s_pdm_tx_config_t m_pdm_tx_cfg;
    i2s_chan_handle_t m_tx_chan;
};

#endif // PDM_PLAYER_HPP