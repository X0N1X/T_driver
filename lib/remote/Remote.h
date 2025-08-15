#ifndef Remote_h
#define Remote_H

#include <Arduino.h>
#include <Control.h>
#include <nRF24L01.h>
#include <RF24.h>

class Remote {
private:
    int ignitionPin;
    int xPositionPin;
    int yPositionPin;
    int acceleratorPin;
    control_t data;
    void read();
    RF24 radio;
    //byte pipe[6] = "00001";
public:
    // Конструктор
    Remote(
        int ignitionPin,    // Пин для подключения выключателя зажигания
        int xPositionPin,   // Пин для подключения потенциометра оси X
        int yPositionPin,   // Пин для подключения потенциометра ocb Y
        int acceleratorPin  // Пин для подключения потенциометра ручки газа
    );
    
    /** 
     * Получение текущих значений огранов управления 
     * @param read - предварительное чтение значений
     */ 
    control_t get(bool read = true); 
    void send(bool read = true); 
    void setupRadio();
};

#endif
