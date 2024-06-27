#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "speaker/pdm_player.hpp"

#define BUFF_SIZE 2048

#define PDM_TX_CLK_IO GPIO_NUM_4  // I2S PDM TX clock io number
#define PDM_TX_DOUT_IO GPIO_NUM_5 // I2S PDM TX data out io number

extern const uint8_t *example_wav_start asm("_binary_example_wav_start");
// extern const uint8_t example_wav_end[] asm("_binary_example_wav_end");
extern const size_t example_wav_size asm("example_wav_length");

void i2s_pdm_tx_task(void *args)
{

    PDM_Player pdm_player(PDM_TX_CLK_IO, PDM_TX_DOUT_IO);

    while (1)
    {

        pdm_player.play((uint16_t *)example_wav_start, example_wav_size);

        vTaskDelay(15);
    }

    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    xTaskCreate(i2s_pdm_tx_task, "i2s_pdm_tx_task", 4096, NULL, 5, NULL);
}
