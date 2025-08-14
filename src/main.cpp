#include <Arduino.h>
#include<Servo.h>

#define DEBUG

int normalize(int value);

Servo engine; // Объект для управлением сервоприводом газа 

int enginePin = 2;  // PWM пин для управления сервоприводом тросика газа
int engineValue = 0;  // Текущее положение серво газа

int accPin = A0;      // Аналоговый ввод для потенциометора ручки газа 
int accValue = 0;     // Текущее значение ручки раза

int yPin = A2;      // Аналоговый ввод для потенциометора по вертикали 
int yValue = 0;     // Текущее значение потенциометора по вертикали

int xPin = A1;      // Аналоговый ввод для потенциометора по горизонтали 
int xValue = 0;     // Текущее значение потенциометора по горизонтали

int zero = 40;      // Диапазон нулевого положения - в котором считается что нет отклонений
int minmax = 20;    // Диапазон крайних положений - в котором считается что отклонение максимальное

int leftPwmPin = 5;     // PWN для левого двигателя
int leftValue = 0;      // Текущая мощьность на левом двигателе
int leftD1Pin = 3;      // D1 D2 направление движения леыого двигателя. 
int leftD2Pin = 4;      // 01 10 - вперед назад   00 11 - стоп
int leftD1 = LOW;
int leftD2 = LOW;

int rightPwmPin = 6;     // PWN для правого двигателя
int rightValue = 0;      // Текущая мощьность на правом двигателе
int rightD1Pin = 7;      // D1 D2 направление движения правого двигателя. 
int rightD2Pin = 8;      // 01 10 - вперед назад   00 11 - стоп
int rightD1 = LOW;
int rightD2 = LOW;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  engine.attach(enginePin);
  Serial.println(TCCR1B);

  //digitalWrite(leftD1Pin, leftD1);
  //digitalWrite(leftD2Pin, leftD2);
}

void loop() {
  // Управление газом
  accValue = analogRead(accPin);
  engineValue = map(accValue, 0, 1023, 0, 180);
  engine.write(engineValue);
#ifdef DEBUG  
  Serial.println(engineValue);
#endif

  // Управление двигателями
  xValue = normalize(analogRead(xPin));
  yValue = normalize(analogRead(yPin));
#ifdef DEBUG
  Serial.print("nX: "); Serial.print(xValue); Serial.print("   nY: "); Serial.println(yValue);
#endif
  // Определение направления скоторости движения
  if (yValue == 0 && xValue == 0) {
    leftD1  = LOW; leftD2  = LOW;  // стоит
    rightD1 = LOW; rightD2 = LOW;  // стоит
  } else if (abs(abs(yValue) - abs(xValue)) < 10) { // диагонали один из двигалей стоит на месте
    if (xValue > 0) {
      rightD1 = LOW; rightD2 = LOW;  // стоит
      rightValue = 0;
      if (yValue > 0) {
        leftD1  = HIGH; leftD2  = LOW;  // вперед
        leftValue = yValue;
      } else if (yValue < 0) {
        leftD1  = LOW; leftD2  = HIGH;  // назад
        leftValue = abs(yValue);
      }
    } else {
      leftD1 = LOW; leftD2  = LOW;  // соит
      leftValue = 0;
      if (yValue > 0) {
        rightD1 = HIGH; rightD2 = LOW;  // вперед
        rightValue = yValue;
      } else if (yValue < 0) {
        rightD1 = LOW; rightD2 = HIGH;  // назад
        rightValue = abs(yValue);
      } 
    }
  } else if (abs(yValue) > abs(xValue)) { // движение вперед/назад c диф скоростями
    if (yValue > 0) {
      leftD1  = HIGH; leftD2  = LOW;  // вперед
      rightD1 = HIGH; rightD2 = LOW;  // вперед
      if (xValue > 0) {
        leftValue = yValue;
        rightValue = yValue - xValue;
      } else {
        rightValue = yValue;
        leftValue = yValue - abs(xValue);
      }
    } else if (yValue < 0) {
      leftD1  = LOW; leftD2  = HIGH;  // назад
      rightD1 = LOW; rightD2 = HIGH;  // назад
      if (xValue > 0) {
        leftValue = abs(yValue);
        rightValue = abs(yValue) - xValue;
      } else {
        rightValue = abs(yValue);
        leftValue = abs(yValue) - abs(xValue);
      }
    } 
  } else { // движение с бортовым разворотом
    if (yValue >= 0) {
      if (xValue > 0) {
        leftD1  = HIGH; leftD2  = LOW;  // вперед
        rightD1 = LOW;  rightD2 = HIGH;  // зазад
        leftValue = max(xValue, yValue);
        rightValue = xValue - yValue;
      } else {
        leftD1  = LOW; leftD2  = HIGH;  // назад
        rightD1 = HIGH; rightD2 = LOW;  // вперед
        rightValue = max(abs(xValue), yValue);
        leftValue = abs(xValue) - yValue;
      } 
    } else if (yValue < 0) {
      if (xValue > 0) {
        leftD1  = LOW; leftD2  = HIGH;  // назад
        rightD1 = HIGH; rightD2 = LOW;  // вперед
        leftValue = max(xValue, abs(yValue));
        rightValue = xValue - abs(yValue);
      } else {
        leftD1  = HIGH; leftD2  = LOW;  // вперед
        rightD1 = LOW;  rightD2 = HIGH;  // зазад
        rightValue = max(abs(xValue), abs(yValue));
        leftValue = abs(xValue) - abs(yValue);
      } 
    } 
  }

#ifdef DEBUG
  if (leftD1 == leftD2) {
    Serial.print("L O 000");
  } else if (leftD1 > leftD2) {
    Serial.print("L A "); Serial.print(leftValue);
  } else {
    Serial.print("L V "); Serial.print(leftValue);
  }

  if (rightD1 == rightD2) {
    Serial.println("   R O 000");
  } else if (rightD1 > rightD2) {
    Serial.print("   R A "); Serial.println(rightValue);
  } else {
    Serial.print("   R V "); Serial.println(rightValue);
  }
#endif

// Задаем вращение левого двигателя
digitalWrite(leftD1Pin, leftD1);
digitalWrite(leftD2Pin, leftD2);
analogWrite(rightPwmPin, rightValue);

//Задаем вращение правого двигателя
digitalWrite(rightD1Pin, rightD1);
digitalWrite(rightD2Pin, rightD2);
analogWrite(rightPwmPin, rightValue);

#ifdef DEBUG
  delay(1000);
#else
  delay(100);
#endif    
}

// Нормализация значений с потенциометра
int normalize(int value) {
  int v = map(value, 0, 1023, -255, 255);
  if (abs(v) < zero ) { 
    return 0;
  } else if (abs(v) > 255 - minmax) {
    if (value < 0) {
      return -255;
    } 
    return 255;
  } 
  return v;
}