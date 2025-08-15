#include <Arduino.h>
#include <Servo.h>
#include <Driver.h>
#include <Remote.h>

#define ZERO_DELTA    40  // Диапазон нулевого положения - в котором считается что нет отклонений
#define MAX_DELTA     20  // Диапазон крайних положений - в котором считается что отклонение максимальное

#define LEFT_PWM_PIN  5   // PWN для левого двигателя
#define LEFT_IN1_PIN  3   // IN1 IN2 направление движения леыого двигателя. 
#define LEFT_IN2_PIN  4   // 01 10 - вперед назад   00 11 - стоп

#define RIGHT_PWM_PIN 6  // PWN для правого двигателя
#define RIGHT_D1_PIN  7  // D1 D2 направление движения правого двигателя. 
#define RIGHT_D2_PIN  8  // 01 10 - вперед назад   00 11 - стоп
#define IGN_PIN       9 // Пин управления переключателем зажигания
#define ACC_PIN       2  // PWM пин для управления сервоприводом тросика газа

// Объект класса управления двигателями
Driver driver(LEFT_PWM_PIN, LEFT_IN1_PIN, LEFT_IN2_PIN, RIGHT_PWM_PIN, RIGHT_D1_PIN, RIGHT_D2_PIN, ACC_PIN, IGN_PIN);

#define Y_PIN         A2 // Аналоговый ввод для потенциометора по вертикали 
#define X_PIN         A1 // Аналоговый ввод для потенциометора по горизонтали 
#define IGN_PIN       A3 // Пин управления переключателем зажигания
#define ACC_PIN       A0 // Аналоговый ввод для потенциометора ручки газа 

// Объект класса пульта управления
Remote remote(IGN_PIN, X_PIN, Y_PIN, ACC_PIN);
control_t control; // Текущий вектор параметров управления

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  driver.setMaxDelta(MAX_DELTA);
  driver.setZeroDelta(ZERO_DELTA);
}

void loop() {

  driver.drive(remote.get());

  #ifdef DEBUG
    delay(1000);
  #else
    delay(100);
  #endif    
}