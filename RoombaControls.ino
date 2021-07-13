void roomba_start() {
  Serial2.begin(115200, SERIAL_8N1, rx_pin, tx_pin);
  Serial2.write(128);
  delay(1000);
}

void roomba_safe_mode() {
  Serial2.write(131);
  delay(1000);
}

void roomba_passive_mode() {
  Serial2.write(128);
  delay(500);
}

void roomba_reset() {
  Serial2.write(7);
  delay(1000);
}

void roomba_toggle_cleaning() {
  Serial2.write(135);
}

void roomba_sync_time() {
  Serial.println("Syncing roomba time...");
  
  roomba_safe_mode();
  roomba_passive_mode();

  Serial2.write(168);
  Serial2.write(rtc.getDayofWeek());
  Serial2.write(rtc.getHour(true));
  Serial2.write(rtc.getMinute());
  
  delay(500);
}
