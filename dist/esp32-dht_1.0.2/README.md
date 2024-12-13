# Example for `dht` Driver

## What it Does

This example demonstrates how to configure and interact with a DHT sensor (DHT11, AM2301/DHT22, or Si7021) using the ESP-IDF framework. The DHT sensor provides temperature and humidity data, which are read and displayed every 2 seconds.

The GPIO pin and DHT sensor type can be configured via **menuconfig**. Additional options include enabling an internal pull-up resistor for the data line.

## Configuration

To configure the example:

1. Run `idf.py menuconfig`.
2. Navigate to **Example Configuration**:
   - Select the sensor type (`DHT11`, `AM2301/DHT22`, or `Si7021`).
   - Set the GPIO number connected to the DHT sensor data pin.
   - Optionally, enable the internal pull-up resistor if no external pull-up is available.

### Default Configuration

| Setting               | Description                                 | Default       |
|-----------------------|---------------------------------------------|---------------|
| **Sensor Type**       | Type of DHT sensor (`DHT11`, `AM2301/DHT22`, `Si7021`) | `AM2301/DHT22` |
| **Data GPIO Number**  | GPIO number connected to the DHT sensor     | `17`          |
| **Internal Pull-Up**  | Enable internal pull-up resistor            | `Disabled`    |

## Wiring

Connect the DHT sensor as follows:

| DHT Sensor Pin | ESP32 Connection         |
|-----------------|--------------------------|
| **VCC**        | 3.3V                     |
| **GND**        | Ground                   |
| **Data**       | Configured GPIO (e.g., `17`) with a pull-up resistor |

> **Note:** A pull-up resistor (typically 10kΩ) is required between the data pin and the 3.3V line. If your sensor is on a breakout board with an integrated pull-up resistor, you can enable the internal pull-up resistor via `menuconfig`.

### Example Wiring Diagram

| DHT Sensor Pin | ESP32 Pin |
|-----------------|-----------|
| VCC            | 3.3V      |
| GND            | GND       |
| Data           | GPIO 17   |

## Example Code

Here’s how to read data from the DHT sensor in your project:

```c
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht.h"

static const char *TAG = "example_main";

void app_main(void)
{
    // Select sensor type and GPIO pin from menuconfig
    dht_sensor_type_t sensor_type = CONFIG_EXAMPLE_TYPE_AM2301 ? DHT_TYPE_AM2301 :
                                    CONFIG_EXAMPLE_TYPE_DHT11 ? DHT_TYPE_DHT11 : DHT_TYPE_SI7021;
    gpio_num_t gpio_num = CONFIG_ESP_TEMP_SENSOR_GPIO;

    // Enable internal pull-up resistor if specified in menuconfig
    if (CONFIG_EXAMPLE_INTERNAL_PULLUP) {
        gpio_pullup_en(gpio_num);
    } else {
        gpio_pullup_dis(gpio_num);
    }

    while (1)
    {
        float humidity = 0, temperature = 0;

        // Read data from the sensor
        esp_err_t result = dht_read_float_data(sensor_type, gpio_num, &humidity, &temperature);
        if (result == ESP_OK)
        {
            ESP_LOGI(TAG, "Humidity: %.1f%% Temperature: %.1f°C", humidity, temperature);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read sensor data: %s", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
    }
}
```

## Building and Running

Build the Project:
```sh
idf.py build
```
Flash the ESP32:
```sh
idf.py flash
```
Monitor Logs:
```sh
idf.py monitor
```

## Example Output

```
I (12345) example_main: Humidity: 50.0% Temperature: 22.5°C
I (14345) example_main: Humidity: 50.1% Temperature: 22.6°C
```

## Notes

- Ensure the DHT sensor is connected properly, and the GPIO pin matches the configuration in menuconfig.
- Use an external pull-up resistor if your breakout board does not have one.
- For stable readings, ensure the sensor is in a stable environment, free from rapid temperature or humidity changes.

StudioPieters® | Innovation and Learning Labs | https://www.studiopieters.nl
