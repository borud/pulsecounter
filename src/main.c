#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_attr.h"
#include "esp_log.h"


#define PCNT_UNIT           PCNT_UNIT_0
#define PCNT_H_LIM_VAL      10
#define PCNT_L_LIM_VAL     -10
#define PCNT_THRESH1_VAL    5
#define PCNT_THRESH0_VAL   -5

#define ANEMOMETER_PIN 4

/* Initialize PCNT functions:
 *  - configure and initialize PCNT
 *  - set up the input filter
 *  - set up the counter events to watch
 */
static void pcnt_init(void)
{
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config;
    
    // Set PCNT input signal and control GPIOs
    pcnt_config.pulse_gpio_num = ANEMOMETER_PIN;
    pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = PCNT_UNIT;

    pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;

    // What to do on the positive / negative edge of pulse input?
    pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
    pcnt_config.neg_mode = PCNT_COUNT_DIS;   // Keep the counter value on the negative edge

    // Set the maximum and minimum limit values to watch
    pcnt_config.counter_h_lim = 32767;
    pcnt_config.counter_l_lim = -32768;
    
    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);

    /* Configure and enable the input filter */
    pcnt_set_filter_value(PCNT_UNIT, 1000);
    pcnt_filter_enable(PCNT_UNIT);


    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(PCNT_UNIT);
}

void app_main(void)
{
    pcnt_init();
    
    while (1) {
        int16_t c;
        pcnt_get_counter_value(PCNT_UNIT, &c);
        printf("Counter: %d\n", c);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    };
}
