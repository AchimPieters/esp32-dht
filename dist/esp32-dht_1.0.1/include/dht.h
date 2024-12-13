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

#ifndef DHT_H
#define DHT_H

#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
        DHT_TYPE_DHT11,
        DHT_TYPE_DHT22
} dht_type_t;

/**
 * @brief Initialize the DHT sensor
 *
 * @param gpio_num GPIO pin connected to the DHT sensor
 * @param type Type of DHT sensor (DHT11 or DHT22)
 * @return esp_err_t ESP_OK on success, or an appropriate error code
 */
esp_err_t dht_init(gpio_num_t gpio_num, dht_type_t type);

/**
 * @brief Read temperature and humidity from the DHT sensor
 *
 * @param temperature Pointer to store the temperature value in Celsius
 * @param humidity Pointer to store the humidity percentage
 * @return esp_err_t ESP_OK on success, or an appropriate error code
 */
esp_err_t dht_read(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif // DHT_H
