#include "Remote.h"

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

void Remote::read() 
{
    this->data.xPosition   = analogRead(this->xPositionPin);
    this->data.yPosition   = analogRead(this->yPositionPin);
    this->data.accelerator = analogRead(this->acceleratorPin);
    this->data.ignition    = digitalRead(this->ignitionPin);
}

control_t Remote::get(bool read = true) {
    if (read) this->read();
    return this->data;
} 

