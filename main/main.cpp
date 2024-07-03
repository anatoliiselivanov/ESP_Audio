#include "speaker/pdm_player.hpp"
#include "wav_file/wav_file_reader.hpp"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>

#define PDM_TX_CLK_IO GPIO_NUM_4  // I2S PDM TX clock io number
#define PDM_TX_DOUT_IO GPIO_NUM_5 // I2S PDM TX data out io number

extern const uint8_t example_wav_start asm("_binary_example_wav_start");
extern const size_t example_wav_size asm("example_wav_length");

void i2s_pdm_tx_task(void *args)
{
    PDM_Player pdm_player(PDM_TX_CLK_IO, PDM_TX_DOUT_IO);

    const uint8_t *ptr_to_example_wav_start = &example_wav_start;
    WavFileReader wav_file_reader(ptr_to_example_wav_start, example_wav_size);

    while (1)
    {
        printf("Playing the example wav file!\n"); // TODO: Replace this with ESP_LOGI
        pdm_player.play(wav_file_reader.data(), wav_file_reader.data_size());

        vTaskDelay(15);
    }

    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    xTaskCreate(i2s_pdm_tx_task, "i2s_pdm_tx_task", 4096, NULL, 5, NULL);
}
