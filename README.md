# WiFi-Gate Server Application

**WiFi-Gate** is a lightweight, ESP8266 or ESP32-based server application for wireless garage door control. It provides a web interface for opening and closing your garage door, with added safety features to detect obstructions using an infrared sensor.

[![License: MIT](https://img.shields.io/badge/License-MIT-g.svg)](https://opensource.org/licenses/MIT)

## Features
- **Remote Control**: Open and close your garage door via a web interface accessible on your local Wi-Fi network.
- **Safety Check**: Prevents closing the garage door if an obstruction is detected.
- **Responsive UI**: Built-in HTML and JavaScript-based web page for user-friendly interaction.
- **Compact Design**: Runs entirely on an ESP8266 / ESP32 microcontroller.

## Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/BogdanBargaoanu/wifi-gate
   ```
2. Open the project in the [Arduino IDE](https://www.arduino.cc/en/software/).
3. Install the necessary libraries:
   - [ESP8266WiFi](https://github.com/esp8266/Arduino)
   - [Servo](https://www.arduino.cc/reference/en/libraries/servo/)
   - [ESP32WiFi](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
    
4. Update the Wi-Fi credentials in the code:
   ```cpp
   const char *ssid = "YourSSID";
   const char *password = "YourPassword";
   ```
5. Upload the code to your ESP8266 / ESP32 board.

## Usage
1. Connect your ESP8266 / ESP32 device to the Wi-Fi network.
2. Open the serial monitor to retrieve the device's IP address.
3. Access the web interface:
   - Open a browser and navigate to `http://<device_ip>`.
   - Use the provided buttons to control the garage door.

### API Endpoints
- `GET /Open`: Opens the garage door.
- `GET /Close`: Closes the garage door (if no obstruction is detected).
- `GET /status`: Returns the infrared sensor state as JSON.

## Hardware Requirements
- ESP8266 / ESP32 microcontroller.
- Servo motor (connected to GPIO12).
- Infrared sensor (connected to GPIO4).
- Wi-Fi network.

## Web Interface
The built-in web interface allows users to control the garage door with simple buttons. It also dynamically updates the button states based on the infrared sensor readings.

## Safety Notes
- Ensure the infrared sensor is properly calibrated to detect obstructions effectively.
- Test the servo motor and door mechanism before full deployment.
