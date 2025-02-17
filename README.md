# WeatherSense - ESP8266 Weather Station

WeatherSense is a simple weather monitoring system built using an ESP8266, a DHT11 sensor for temperature and humidity readings, and a local PHP server to store and display the data. The ESP8266 acts as an access point and runs a web server to handle data requests.

![WeatherSense Overview](https://your-image-link.com/overview.jpg)

## Features

- Reads temperature and humidity data from a DHT11 sensor.
- Hosts a web server that responds with the temperature and humidity in JSON format.
- Sends temperature and humidity data to a local PHP server using a POST request.
- The data is sent once every minute to the server.
- ESP8266 is set up in Access Point (AP) mode, enabling local access to the device.

## Hardware Requirements

- **ESP8266** (NodeMCU, Wemos D1 Mini, etc.)
- **DHT11** Temperature & Humidity Sensor
- Jumper wires and breadboard

![ESP8266 Wiring Diagram](https://your-image-link.com/wiring-diagram.jpg)

## Software Requirements

- Arduino IDE with ESP8266 board support
- DHT sensor library (`DHT.h`)
- ESP8266WiFi, ESP8266WebServer, ESP8266HTTPClient libraries

## Setup Instructions

1. **Install Required Libraries:**
   - In the Arduino IDE, go to **Sketch** > **Include Library** > **Manage Libraries**.
   - Search for and install the following libraries:
     - `DHT sensor library`
     - `ESP8266WiFi`
     - `ESP8266WebServer`
     - `ESP8266HTTPClient`

2. **Wiring the DHT11 Sensor:**
   - Connect the DHT11 sensor to the ESP8266 as follows:
     - VCC to 3V3
     - GND to GND
     - Data pin to D4 (GPIO2)

   ![DHT11 Wiring](https://your-image-link.com/dht11-wiring.jpg)

3. **Upload the Code:**
   - Open the Arduino IDE and select the correct board (ESP8266).
   - Connect the ESP8266 to your computer.
   - Upload the code to the ESP8266.

4. **Configure Local PHP Server:**
   - Set up a local PHP server (e.g., using XAMPP, WAMP, or a Raspberry Pi).
   - Create a PHP file (`index.php`) in the `Wsense` directory to handle POST requests and store/display the temperature and humidity data.

   ![PHP Server Setup](https://your-image-link.com/php-server-setup.jpg)

5. **Access the Web Interface:**
   - Once the ESP8266 is powered on, it will create a Wi-Fi access point with the SSID "WeatherSense_AP" and password "12345678".
   - Connect to this Wi-Fi network using your device and access the web server at the IP address provided by the ESP8266 (printed in the Serial Monitor).

6. **Viewing Data:**
   - The ESP8266 will send temperature and humidity data to your local PHP server once every minute.
   - You can view the stored data by navigating to the PHP page on your local server.

   ![Web Interface](https://your-image-link.com/web-interface.jpg)

## Code Overview

- **Web Server:**
  The ESP8266 runs a web server on port 80 that handles GET requests for `/store_data.php`. It responds with the current temperature and humidity in JSON format.

- **DHT Sensor:**
  The DHT11 sensor reads temperature and humidity once every minute.

- **Sending Data:**
  Every minute, the temperature and humidity data is sent to a local server via a POST request.

- **AP Mode:**
  The ESP8266 is set to Access Point (AP) mode, which allows devices to connect directly to it without needing an existing Wi-Fi network.

## Example Response

When accessing the web server via a GET request, the response is a JSON object with temperature and humidity values:

```json
{
  "temperature": 22.5,
  "humidity": 60.0
}
