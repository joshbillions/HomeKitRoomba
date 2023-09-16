# HomeKitRoomba

![Pinout](http://joshbillions.users.sonic.net/github/roomba_pinout.png)

## What is this?
Connect your Roomba to HomeKit using the HomeSpan library, allowing for easy on/off behavior from your iOS or macOS device.

After getting online the ESP32 will sync the Roomba's internal clock with an NTP server. This prevents the annoying case where your Roomba runs out of battery after getting stuck on something, you stick it back on the charger, and it starts cleaning in the middle of the night because the RTC has been reset.

## Parts
- [TTGO T-Display (ESP32)](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- Voltage translator, if you you can do homemade [store-bought](https://www.sparkfun.com/products/11771) is fine. 
- [5V Regulator](https://www.sparkfun.com/products/107)

## Wiring
- Connect the voltage regulator positive and ground to the Roomba's battery via the serial connector.
- Connect the ESP32's VIN pin to the 5V regulator output
- Connect the high voltage side of the translator to the 5V regulator output
- Connect the low voltage side of the translator to the ESP32's 3.3V output pin.
- Connect the high voltage side of the translator to pins 3 (RX) and 4 (TX) of the roomba's serial connector
- Connect the low voltage side of the translator to pins 32 (TX) and 33 (RX) of the ESP32.
