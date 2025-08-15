#include "Remote.h"
#include "printf.h"

Remote::Remote(int ignitionPin, int xPositionPin, int yPositionPin, int acceleratorPin)
    : ignitionPin(ignitionPin),
      acceleratorPin(acceleratorPin),
      xPositionPin(xPositionPin),
      yPositionPin(yPositionPin) 
{
    pinMode(ignitionPin, INPUT_PULLUP);
    pinMode(acceleratorPin, INPUT);
    pinMode(xPositionPin, INPUT);
    pinMode(yPositionPin, INPUT);

    this->read();

}

void Remote::setupRadio() {
    RF24 radio(9, 10);

    if(radio.begin()) {
        Serial.println("OK");
    }else{
        Serial.println("Error");
    }
    delay(3000);
    radio.openWritingPipe(0xF0F0F0F0F0);
    //radio.setPALevel(RF24_PA_MIN);
    radio.setChannel(0x5A);
    radio.stopListening();

    //Serial.println(radio.begin());
    //radio.setPALevel(RF24_PA_MIN);
    //radio.setDataRate(RF24_250KBPS);
    //radio.setChannel(80);
    //radio.openWritingPipe(pipe);
}

void Remote::send(bool read = true) {
     if (read) this->read();
     int d = 109;
  bool res = radio.write(&d, 3);
  if (res) {
    Serial.println("t OK");
  } else {
     Serial.println("t FAILED");
  }
    //radio.write(&data, sizeof(data));
}

void Remote::read() 
{
    data.xPosition   = analogRead(xPositionPin);
    data.yPosition   = analogRead(yPositionPin);
    data.accelerator = analogRead(acceleratorPin);
    data.ignition    = digitalRead(ignitionPin);
}

control_t Remote::get(bool read = true) {
    if (read) this->read();
    return this->data;
} 

