HomeKitRoomba
--

Connect your Roomba to HomeKit using the HomeSpan library, allowing for easy on/off behavior from your iOS device or Mac.

After getting online the ESP32 will sync the Roomba's RTC with an NTP server. This prevents the annoying case where your Roomba runs out of battery after getting stuck on something, you stick it back on the charger, and it starts cleaning in the middle of the night because the RTC has been reset.
