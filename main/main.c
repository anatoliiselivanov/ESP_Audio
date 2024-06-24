#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdm.h"

#define MHZ (1000000)
#define SIGMA_DELTA_GPIO_NUM (10)
#define OVER_SAMPLE_RATE (10 * MHZ)

static sdm_channel_handle_t init_sdm(void)
{
    /* Allocate sdm channel handle */
    sdm_channel_handle_t sdm_chan = NULL;
    sdm_config_t config = {
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .gpio_num = SIGMA_DELTA_GPIO_NUM,
        .sample_rate_hz = OVER_SAMPLE_RATE,
    };
    ESP_ERROR_CHECK(sdm_new_channel(&config, &sdm_chan));
    /* Enable the sdm channel */
    ESP_ERROR_CHECK(sdm_channel_enable(sdm_chan));

    return sdm_chan;
}

void app_main(void)
{
    /* Initialize sigma-delta modulation on the specific GPIO */
    sdm_channel_handle_t sdm_chan = init_sdm();
}
