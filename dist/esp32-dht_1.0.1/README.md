# Example for `dht` Driver

## What it Does

This example demonstrates how to configure and interact with a DHT sensor (DHT11 or DHT22) using the ESP-IDF framework. The DHT sensor provides temperature and humidity data, which are read and displayed every 2 seconds.

To use this example, ensure the appropriate GPIO pin and DHT sensor type are configured using `menuconfig`. These settings can be found under **DHT Sensor Configuration**.

## Wiring

Connect the data pin of the DHT sensor to the ESP32 GPIO pin defined in `menuconfig`. Make sure to add a pull-up resistor (typically 10kΩ) between the data pin and the 3.3V line for reliable operation.

| Name       | Description                       | Default  |
|------------|-----------------------------------|----------|
| `DHT_GPIO` | GPIO number connected to the DHT  | `4`      |
| `DHT_TYPE` | Type of DHT sensor (11 or 22)     | `DHT22`  |

### Example Wiring for DHT Sensor

| DHT Sensor Pin | Connection              |
|-----------------|-------------------------|
| VCC            | 3.3V                   |
| GND            | Ground                 |
| Data           | Configured GPIO (e.g., 4) with a pull-up resistor |

<img src="https://github.com/AchimPieters/ESP32-SmartPlug/blob/main/images/MIT%7C%20SOFTWARE%20WHITE.svg" alt="MIT Licence Logo" width="150">

## Include the Library in Your Code

In your project's source code, include the DHT driver header:

```c
#include "dht.h"
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define DHT_GPIO GPIO_NUM_4  // GPIO number for DHT sensor
#define DHT_TYPE DHT_TYPE_DHT22  // Type of DHT sensor

void app_main(void)
{
    float temperature = 0.0f, humidity = 0.0f;

    // Initialize the DHT sensor
    if (dht_init(DHT_GPIO, DHT_TYPE) == ESP_OK) {
        printf("DHT sensor initialized successfully.\n");
    } else {
        printf("Failed to initialize DHT sensor.\n");
        return;
    }

    // Read temperature and humidity
    while (1) {
        if (dht_read(&temperature, &humidity) == ESP_OK) {
            printf("Temperature: %.1f°C, Humidity: %.1f%%\n", temperature, humidity);
        } else {
            printf("Failed to read data from DHT sensor.\n");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);  // Delay for 2 seconds
    }
}
```

## Configuration

The GPIO pin and sensor type can be configured in `menuconfig`:

1. Run `idf.py menuconfig`.
2. Navigate to **DHT Sensor Configuration**.
3. Set the GPIO number connected to the DHT sensor.
4. Select the sensor type (DHT11 or DHT22).

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
I (12345) DHT_Example: DHT sensor initialized successfully.
I (12346) DHT_Example: Temperature: 25.1°C, Humidity: 60.2%
I (14346) DHT_Example: Temperature: 25.2°C, Humidity: 60.3%
```
StudioPieters® | Innovation and Learning Labs | https://www.studiopieters.nl
