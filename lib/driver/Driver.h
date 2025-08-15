#ifndef Driver_h
#define Driver_h

#include <Arduino.h>
#include <Control.h>

#include <nRF24L01.h>
#include <RF24.h>

class Driver {
    public: 
        Driver(
            int leftPwmPin,     // PWN для левого двигателя
            int leftD1Pin,      // D1 D2 направление движения леыого двигателя. 
            int leftD2Pin,      // 01 10 - вперед назад   00 11 - стоп
            int rightPwmPin,    // PWN для правого двигателя
            int rightD1Pin,     // D1 D2 направление движения правого двигателя. 
            int rightD2Pin,     // 01 10 - вперед назад   00 11 - стоп
            int acceleratorPin, // Пин управления газом    
            int ignitionPin     // Пин управления зажиганием
        );
        void drive(control_t data);
        void remoteDrive();
        void setZeroDelta(int v);
        void setMaxDelta(int v);

        void setupRadio();
        
    private:
        RF24 radio;
        //byte pipe[6] = "00001";
        control_t data;

        int acceleratorPin;
        int ignitionPin;

        int accValue = 0;
        int ignition = LOW;

        int leftPwmPin = 5;      
        int leftValue  = 0;      
        int leftD1Pin  = 3;      
        int leftD2Pin  = 4;      
        int leftD1     = LOW;
        int leftD2     = LOW;

        int rightPwmPin = 6;
        int rightValue  = 0;
        int rightD1Pin  = 7;
        int rightD2Pin  = 8;
        int rightD1     = LOW;
        int rightD2     = LOW;

        int delta_zero = 0;
        int delta_max = 0;
        
        void calculate(int x, int y);
        void write();
        bool receiveData();
};
#endif