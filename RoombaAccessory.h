#include <NTPClient.h>

struct Roomba : Service::LightBulb {
  SpanCharacteristic *power;
  NTPClient *ntp_client;
  

  Roomba(NTPClient &ntp_client) : Service::LightBulb() {
    this->ntp_client = &ntp_client;
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
