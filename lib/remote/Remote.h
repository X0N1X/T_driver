#include <Arduino.h>
#include <Control.h>

class Remote {
private:
    int ignitionPin;
    int xPositionPin;
    int yPositionPin;
    int acceleratorPin;
    control_t data;
    void read();
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
};

