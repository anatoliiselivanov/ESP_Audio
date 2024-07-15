#include "speaker/pdm_player.hpp"
#include "wav_file/wav_file_reader.hpp"
#include "mp3_file/mp3_file_reader.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"



extern const uint8_t wav_file_start asm("_binary_test_wav_start");
extern const size_t wav_file_len asm("test_wav_length");
static const char* const wav_file_format{"wav"};

extern const uint8_t mp3_file_start asm("_binary_test_mp3_start");
extern const size_t mp3_file_len asm("test_mp3_length");
static const char* const mp3_file_format{"mp3"};


#define PDM_TX_CLK_IO GPIO_NUM_4  // I2S PDM TX clock io number
#define PDM_TX_DOUT_IO GPIO_NUM_6 // I2S PDM TX data out io number

#define I2C_MASTER_SCL_IO GPIO_NUM_2 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_1 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0     /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100'000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0  /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0  /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000
// #define VOLUME_CONTROL 1

#ifdef VOLUME_CONTROL

#define AMP_ADDRESS_W 0b1001011
#define MAX_VOLUME 63

static esp_err_t amplifier_set_volume(const uint8_t val)
{
    uint8_t reg_val{val};
    return i2c_master_write_to_device(I2C_MASTER_NUM, AMP_ADDRESS_W, &reg_val, 1U, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

static esp_err_t i2c_master_init(void)
{
    i2c_port_t i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ},
        .clk_flags = 0};

    i2c_param_config(I2C_MASTER_NUM, &conf);

    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
#endif

void i2s_pdm_tx_task(void *args)
{
#ifdef VOLUME_CONTROL
    int16_t volume{MAX_VOLUME};
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI("I2C", "I2C initialized successfully");
#endif

    SoundFile *sound_file_wav = new WavFile(&wav_file_start, wav_file_len);
    SoundFile *sound_file_mp3 = new Mp3File(&mp3_file_start, mp3_file_len);

    PDM_Player pdm_player(PDM_TX_CLK_IO, PDM_TX_DOUT_IO);
    while (1)
    {
#ifdef VOLUME_CONTROL
        printf("Playing the example file with volume = %d\n", (int)volume);
        ESP_ERROR_CHECK(amplifier_set_volume(volume));
        volume -= 5;
        if (volume <= 0)
        {
            volume = MAX_VOLUME;
        }
#else
        printf("Start playing wav sound file!\r\n");
        pdm_player.play(sound_file_wav);
        printf("File is finished\r\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
        
        printf("Start playing mp3 sound file!\r\n");
        pdm_player.play(sound_file_mp3);
        printf("File is finished\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
#endif
    }
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    printf("Start the program\r\n");
    xTaskCreate(i2s_pdm_tx_task, "i2s_pdm_tx_task", 120000, NULL, 5, NULL);
}
