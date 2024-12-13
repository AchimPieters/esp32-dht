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

 #include <freertos/FreeRTOS.h>
 #include <freertos/task.h>
 #include <string.h>
 #include <esp_log.h>
 #include <esp_rom_sys.h>
 #include <driver/gpio.h>

// DHT timer precision in microseconds
 #define DHT_TIMER_INTERVAL 2
 #define DHT_DATA_BITS 40
 #define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

static const char *TAG = "dht";

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
 #define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
 #define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

 #define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)

 #define CHECK_LOGE(x, msg, ...) do { \
                esp_err_t __err = (x); \
                if (__err != ESP_OK) { \
                        PORT_EXIT_CRITICAL(); \
                        ESP_LOGE(TAG, msg, ## __VA_ARGS__); \
                        return __err; \
                } \
} while (0)

static esp_err_t dht_await_pin_state(gpio_num_t pin, uint32_t timeout, int expected_pin_state, uint32_t *duration)
{
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        for (uint32_t i = 0; i < timeout; i += DHT_TIMER_INTERVAL)
        {
                esp_rom_delay_us(DHT_TIMER_INTERVAL);
                if (gpio_get_level(pin) == expected_pin_state)
                {
                        if (duration) *duration = i;
                        return ESP_OK;
                }
        }
        return ESP_ERR_TIMEOUT;
}

static esp_err_t dht_fetch_data(dht_sensor_type_t sensor_type, gpio_num_t pin, uint8_t data[DHT_DATA_BYTES])
{
        uint32_t low_duration, high_duration;

        gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
        gpio_set_level(pin, 0);
        esp_rom_delay_us(sensor_type == DHT_TYPE_SI7021 ? 500 : 20000);
        gpio_set_level(pin, 1);

        CHECK_LOGE(dht_await_pin_state(pin, 40, 0, NULL), "Phase 'B' timeout");
        CHECK_LOGE(dht_await_pin_state(pin, 88, 1, NULL), "Phase 'C' timeout");
        CHECK_LOGE(dht_await_pin_state(pin, 88, 0, NULL), "Phase 'D' timeout");

        for (int i = 0; i < DHT_DATA_BITS; i++)
        {
                CHECK_LOGE(dht_await_pin_state(pin, 65, 1, &low_duration), "Low duration timeout");
                CHECK_LOGE(dht_await_pin_state(pin, 75, 0, &high_duration), "High duration timeout");

                uint8_t byte_index = i / 8;
                uint8_t bit_index = i % 8;
                if (!bit_index) data[byte_index] = 0;

                data[byte_index] |= (high_duration > low_duration) << (7 - bit_index);
        }

        return ESP_OK;
}

static inline int16_t dht_convert_data(dht_sensor_type_t sensor_type, uint8_t msb, uint8_t lsb)
{
        int16_t result;
        if (sensor_type == DHT_TYPE_DHT11)
        {
                result = msb * 10;
        }
        else
        {
                result = (msb & 0x7F) << 8 | lsb;
                if (msb & 0x80) result = -result;
        }
        return result;
}

esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin, int16_t *humidity, int16_t *temperature)
{
        CHECK_ARG(humidity || temperature);

        uint8_t data[DHT_DATA_BYTES] = { 0 };

        gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
        gpio_set_level(pin, 1);

        PORT_ENTER_CRITICAL();
        esp_err_t result = dht_fetch_data(sensor_type, pin, data);
        PORT_EXIT_CRITICAL();

        gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
        gpio_set_level(pin, 1);

        if (result != ESP_OK) return result;

        if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
        {
                ESP_LOGE(TAG, "Checksum failed");
                return ESP_ERR_INVALID_CRC;
        }

        if (humidity) *humidity = dht_convert_data(sensor_type, data[0], data[1]);
        if (temperature) *temperature = dht_convert_data(sensor_type, data[2], data[3]);

        ESP_LOGD(TAG, "Humidity: %d, Temperature: %d", *humidity, *temperature);

        return ESP_OK;
}

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin, float *humidity, float *temperature)
{
        CHECK_ARG(humidity || temperature);

        int16_t int_humidity, int_temperature;
        esp_err_t result = dht_read_data(sensor_type, pin, humidity ? &int_humidity : NULL, temperature ? &int_temperature : NULL);
        if (result != ESP_OK) return result;

        if (humidity) *humidity = int_humidity / 10.0f;
        if (temperature) *temperature = int_temperature / 10.0f;

        return ESP_OK;
}
