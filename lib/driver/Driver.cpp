#include "Driver.h"

// Нормализация значений с потенциометра
static int normalize(int value, int zero, int max) {
  int v = map(value, 0, 1023, -255, 255);
  if (abs(v) < zero ) { 
    return 0;
  } else if (abs(v) > 255 - max) {
    if (v < 0) {
      return -255;
    } 
    return 255;
  } 
  return v;
}

Driver::Driver(
    int leftPwmPin, 
    int leftD1Pin, 
    int leftD2Pin, 
    int rightPwmPin, 
    int rightD1Pin,     
    int rightD2Pin,     
    int acceleratorPin,
    int ignitionPin) 
    : 
    ignitionPin(ignitionPin),
    acceleratorPin(acceleratorPin),
    leftPwmPin(leftPwmPin),
    leftD1Pin(leftD1Pin),
    leftD2Pin(leftD2Pin),
    rightPwmPin(rightPwmPin),
    rightD1Pin(rightD1Pin),
    rightD2Pin(rightD2Pin)  
{
    pinMode(leftPwmPin,     OUTPUT);
    pinMode(leftD1Pin,      OUTPUT);
    pinMode(leftD2Pin,      OUTPUT);
    pinMode(rightPwmPin,    OUTPUT);
    pinMode(rightD1Pin,     OUTPUT);
    pinMode(rightD2Pin,     OUTPUT);
    pinMode(acceleratorPin, OUTPUT);
    pinMode(ignitionPin,    OUTPUT);

    this->write();
}

void Driver::setMaxDelta(int v) {
    this->delta_max = v;
}

void Driver::setZeroDelta(int v) {
    this->delta_zero = v;
}

void Driver::setupRadio() {
    RF24 radio(9, 10);
    if (radio.begin()) {
        Serial.println("radio begin");
    } else {
        Serial.println("radio not begin");
    }
    delay(3000);
    radio.openReadingPipe(0, 0xF0F0F0F0F0);
    radio.setChannel(0x5A);
    //radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    
    //radio.setPALevel(RF24_PA_MIN);
    //radio.setDataRate(RF24_250KBPS);
    //radio.setChannel(80);
    //radio.startListening();
}

bool Driver::receiveData() {
    if (radio.available()) {
        Serial.println("ava");
        radio.read(&data, sizeof(data));
        Serial.println(data.xPosition);
        return true;
    }
    Serial.println("not ava");
    return false;
}

void Driver::write() {
    digitalWrite(this->ignitionPin, this->ignition);
    analogWrite(this->acceleratorPin, this->accValue);
    // Задаем вращение левого двигателя
    digitalWrite(this->leftD1Pin, this->leftD1);
    digitalWrite(this->leftD2Pin, this->leftD2);
    analogWrite(this->leftPwmPin, this->leftValue);

    //Задаем вращение правого двигателя
    digitalWrite(this->rightD1Pin, this->rightD1);
    digitalWrite(this->rightD2Pin, this->rightD2);
    analogWrite(this->rightPwmPin, this->rightValue);
}

void Driver::remoteDrive() {
    if (this->receiveData()) {
        this->drive(this->data);
    }
}

void Driver::drive(control_t data) {
    this->calculate(
        normalize(data.xPosition, this->delta_zero, this->delta_max), 
        normalize(data.yPosition, this->delta_max, this->delta_max)
    );
    this->ignition = data.ignition;
    this->accValue = data.accelerator;
    #ifdef DEBUG
        Serial.print("ign: "); Serial.print(data.ignition); Serial.print("  acc: "); Serial.println(data.accelerator);
    #endif
    
    this->write();
}

void Driver::calculate(int x, int y) {
#ifdef DEBUG
  Serial.print("nX: "); Serial.print(x); Serial.print("  nY: "); Serial.println(y);
#endif

    if (y == 0 && x == 0) {
        this->leftD1  = LOW; this->leftD2  = LOW;  // стоит
        this->rightD1 = LOW; this->rightD2 = LOW;  // стоит
    } else if (abs(abs(y) - abs(x)) < 10) { // диагонали один из двигалей стоит на месте
        if (x > 0) {
        this->rightD1 = LOW; this->rightD2 = LOW;  // стоит
        this->rightValue = 0;
        if (y > 0) {
            this->leftD1  = HIGH; this->leftD2  = LOW;  // вперед
            this->leftValue = y;
        } else if (y < 0) {
            this->leftD1  = LOW; this->leftD2  = HIGH;  // назад
            this->leftValue = abs(y);
        }
    } else {
        this->leftD1 = LOW; this->leftD2  = LOW;  // соит
        this->leftValue = 0;
        if (y > 0) {
            this->rightD1 = HIGH; this->rightD2 = LOW;  // вперед
            this->rightValue = y;
        } else if (y < 0) {
            this->rightD1 = LOW; this->rightD2 = HIGH;  // назад
            this->rightValue = abs(y);
        } 
        }
    } else if (abs(y) > abs(x)) { // движение вперед/назад c диф скоростями
        if (y > 0) {
        this->leftD1  = HIGH; this->leftD2  = LOW;  // вперед
        this->rightD1 = HIGH; this->rightD2 = LOW;  // вперед
        if (x > 0) {
            this->leftValue = y;
            this->rightValue = y - x;
        } else {
            this->rightValue = y;
            this->leftValue = y - abs(x);
        }
        } else if (y < 0) {
        this->leftD1  = LOW; this->leftD2  = HIGH;  // назад
        this->rightD1 = LOW; this->rightD2 = HIGH;  // назад
        if (x > 0) {
            this->leftValue = abs(y);
            this->rightValue = abs(y) - x;
        } else {
            this->rightValue = abs(y);
            this->leftValue = abs(y) - abs(x);
        }
        } 
    } else { // движение с бортовым разворотом
        if (y >= 0) {
            if (x > 0) {
                this->leftD1  = HIGH; this->leftD2  = LOW;  // вперед
                this->rightD1 = LOW;  this->rightD2 = HIGH;  // зазад
                leftValue = max(x, y);
                rightValue = x - y;
            } else {
                this->leftD1  = LOW; this->leftD2  = HIGH;  // назад
                this->rightD1 = HIGH; this->rightD2 = LOW;  // вперед
                this->rightValue = max(abs(x), y);
                this->leftValue = abs(x) - y;
            } 
        } else if (y < 0) {
            if (x > 0) {
                this->leftD1  = LOW; this->leftD2  = HIGH;  // назад
                this->rightD1 = HIGH; this->rightD2 = LOW;  // вперед
                this->leftValue = max(x, abs(y));
                this->rightValue = x - abs(y);
            } else {
                this->leftD1  = HIGH; this->leftD2  = LOW;  // вперед
                this->rightD1 = LOW;  this->rightD2 = HIGH;  // зазад
                this->rightValue = max(abs(x), abs(y));
                this->leftValue = abs(x) - abs(y);
            } 
        } 
    }

#ifdef DEBUG
    if (this->leftD1 == this->leftD2) {
        Serial.print("L O 000");
    } else if (this->leftD1 > this->leftD2) {
        Serial.print("L A "); Serial.print(this->leftValue);
    } else {
        Serial.print("L V "); Serial.print(this->leftValue);
    }

    if (this->rightD1 == this->rightD2) {
        Serial.println("   R O 000");
    } else if (this->rightD1 > this->rightD2) {
        Serial.print("   R A "); Serial.println(this->rightValue);
    } else {
        Serial.print("   R V "); Serial.println(this->rightValue);
    }
#endif    
}