/**
   Copyright 2024 Achim Pieters | StudioPieters®

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   for more information visit https://www.studiopieters.nl
 **/

#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define DHT_START_SIGNAL_TIME_MS 20
#define DHT_SIGNAL_TIMEOUT_US   1000

static gpio_num_t dht_gpio = CONFIG_ESP_TEMP_SENSOR_GPIO;
static dht_type_t dht_type =
#if CONFIG_EXAMPLE_TYPE_DHT11
        DHT_TYPE_DHT11;
#elif CONFIG_EXAMPLE_TYPE_AM2301
        DHT_TYPE_DHT22;
#else
        DHT_TYPE_DHT22; // Default fallback
#endif

static esp_err_t dht_wait_for_level(int level, uint32_t timeout_us) {
        uint32_t elapsed_time = 0;
        while (gpio_get_level(dht_gpio) != level) {
                if (elapsed_time++ > timeout_us) {
                        return ESP_ERR_TIMEOUT;
                }
                ets_delay_us(1);
        }
        return ESP_OK;
}

esp_err_t dht_init() {
        gpio_set_direction(dht_gpio, GPIO_MODE_INPUT_OUTPUT);
        gpio_set_level(dht_gpio, 1);

        ESP_LOGI("DHT", "DHT sensor initialized on GPIO %d, Type: %s", dht_gpio,
                 dht_type == DHT_TYPE_DHT11 ? "DHT11" : "DHT22");
        return ESP_OK;
}

esp_err_t dht_read(float *temperature, float *humidity) {
        if (!temperature || !humidity) {
                return ESP_ERR_INVALID_ARG;
        }

        uint8_t data[5] = {0};

        // Send start signal
        gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
        gpio_set_level(dht_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(DHT_START_SIGNAL_TIME_MS));
        gpio_set_level(dht_gpio, 1);
        ets_delay_us(40);
        gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);

        // Wait for sensor response
        if (dht_wait_for_level(0, DHT_SIGNAL_TIMEOUT_US) != ESP_OK ||
            dht_wait_for_level(1, DHT_SIGNAL_TIMEOUT_US) != ESP_OK) {
                ESP_LOGE("DHT", "Sensor response timed out");
                return ESP_ERR_TIMEOUT;
        }

        // Read 40 bits (5 bytes)
        for (int i = 0; i < 40; ++i) {
                if (dht_wait_for_level(0, DHT_SIGNAL_TIMEOUT_US) != ESP_OK) {
                        ESP_LOGE("DHT", "Timeout waiting for bit %d", i);
                        return ESP_ERR_TIMEOUT;
                }
                uint32_t pulse_time = 0;
                while (gpio_get_level(dht_gpio)) {
                        if (pulse_time++ > DHT_SIGNAL_TIMEOUT_US) {
                                ESP_LOGE("DHT", "Timeout measuring bit %d", i);
                                return ESP_ERR_TIMEOUT;
                        }
                        ets_delay_us(1);
                }
                data[i / 8] <<= 1;
                if (pulse_time > 50) {
                        data[i / 8] |= 1;
                }
        }

        // Verify checksum
        if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
                ESP_LOGE("DHT", "Checksum failed");
                return ESP_ERR_INVALID_CRC;
        }

        if (dht_type == DHT_TYPE_DHT11) {
                *humidity = data[0] + data[1] * 0.1;
                *temperature = data[2] + data[3] * 0.1;
        } else {
                *humidity = ((data[0] << 8) | data[1]) * 0.1;
                *temperature = ((data[2] << 8) | data[3]) * 0.1;
        }

        ESP_LOGI("DHT", "Temperature: %.1f°C, Humidity: %.1f%%", *temperature, *humidity);
        return ESP_OK;
}
