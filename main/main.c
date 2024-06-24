#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"

#define BUFF_SIZE 2048

#define PDM_TX_CLK_IO GPIO_NUM_4  // I2S PDM TX clock io number
#define PDM_TX_DOUT_IO GPIO_NUM_5 // I2S PDM TX data out io number

#define PDM_TX_FREQ_HZ 44100    // I2S PDM TX frequency
#define WAVE_AMPLITUDE (1000.0) // 1~32767
#define CONST_PI (3.1416f)
#define SINE_WAVE_LEN(tone) (uint32_t)((PDM_TX_FREQ_HZ / (float)tone) + 0.5) // The sample point number per sine wave to generate the tone
#define TONE_LAST_TIME_MS 500
#define BYTE_NUM_EVERY_TONE (TONE_LAST_TIME_MS * PDM_TX_FREQ_HZ / 1000)

/* The frequency of tones: do, re, mi, fa, so, la, si, in Hz. */
static const uint32_t tone[3][7] = {
    {262, 294, 330, 349, 392, 440, 494},        // bass
    {523, 587, 659, 698, 784, 880, 988},        // altoexam
    {1046, 1175, 1318, 1397, 1568, 1760, 1976}, // treble
};
/* Numbered musical notation of 'twinkle twinkle little star' */
static const uint8_t song[28] = {1, 1, 5, 5, 6, 6, 5,
                                 4, 4, 3, 3, 2, 2, 1,
                                 5, 5, 4, 4, 3, 3, 2,
                                 5, 5, 4, 4, 3, 3, 2};
/* Rhythm of 'twinkle twinkle little star', it's repeated in four sections */
static const uint8_t rhythm[7] = {1, 1, 1, 1, 1, 1, 2};

static const char *tone_name[3] = {"bass", "alto", "treble"};

static i2s_chan_handle_t i2s_init_pdm_tx(void)
{
    i2s_chan_handle_t tx_chan; // I2S tx channel handler
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    tx_chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));

    i2s_pdm_tx_config_t pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(PDM_TX_FREQ_HZ),
        /* The data bit-width of PDM mode is fixed to 16 */
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = PDM_TX_CLK_IO,
            .dout = PDM_TX_DOUT_IO,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(tx_chan, &pdm_tx_cfg));

    /* Step 3: Enable the tx channel before writing data */
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));

    return tx_chan;
}

void i2s_pdm_tx_task(void *args)
{
    int16_t *w_buf = (int16_t *)calloc(1, BUFF_SIZE);
    assert(w_buf);
    i2s_chan_handle_t tx_chan = i2s_init_pdm_tx();

    size_t w_bytes = 0;

    uint8_t cnt = 0;         // The current index of the song
    uint8_t tone_select = 0; // To selecting the tone level

    printf("Playing %s `twinkle twinkle little star`\n", tone_name[tone_select]);
    while (1)
    {
        int tone_point = SINE_WAVE_LEN(tone[tone_select][song[cnt] - 1]);
        /* Generate the tone buffer */
        for (int i = 0; i < tone_point; i++)
        {
            w_buf[i] = (int16_t)((sin(2 * (float)i * CONST_PI / tone_point)) * WAVE_AMPLITUDE);
        }
        for (int tot_bytes = 0; tot_bytes < BYTE_NUM_EVERY_TONE * rhythm[cnt % 7]; tot_bytes += w_bytes)
        {
            /* Play the tone */
            if (i2s_channel_write(tx_chan, w_buf, tone_point * sizeof(int16_t), &w_bytes, 1000) != ESP_OK)
            {
                printf("Write Task: i2s write failed\n");
            }
        }
        cnt++;
        /* If finished playing, switch the tone level */
        if (cnt == sizeof(song))
        {
            cnt = 0;
            tone_select++;
            tone_select %= 3;
            printf("Playing %s `twinkle twinkle little star`\n", tone_name[tone_select]);
        }
        /* Gap between the tones */
        vTaskDelay(15);
    }
    free(w_buf);
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(i2s_pdm_tx_task, "i2s_pdm_tx_task", 4096, NULL, 5, NULL);
}
