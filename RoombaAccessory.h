struct Roomba : Service::LightBulb {
  SpanCharacteristic *power;

  Roomba() : Service::LightBulb() {
    power = new Characteristic::On();
  }

  boolean update() {
    // Toggle cleaning
    Serial2.write(135);

    if (power->getNewVal() == false) {
      Serial.println("Off");
    } else {
      Serial.println("On");
    }

    return true;
  }

};
