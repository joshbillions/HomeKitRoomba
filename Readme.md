# HomeKitRoomba

![Pinout](http://joshbillions.users.sonic.net/github/roomba_pinout.png)

## What is this?
Connect your Roomba to HomeKit using the HomeSpan library, allowing for easy on/off behavior from your iOS or macOS device.

After getting online the ESP32 will sync the Roomba's internal clock with an NTP server. This prevents the annoying case where your Roomba runs out of battery after getting stuck on something, you stick it back on the charger, and it starts cleaning in the middle of the night because the RTC has been reset.

## Parts
- [TTGO T-Display (ESP32)](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- Voltage translator, if you you can do homemade [store-bought](https://www.sparkfun.com/products/11771) is fine. 
- [5V Regulator](https://www.sparkfun.com/products/107)