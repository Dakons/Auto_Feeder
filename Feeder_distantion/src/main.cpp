#include "Adafruit_VL53L0X.h"               
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
 
void setup() 
{
  Serial.begin(115200);                     
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Подключение модуля");
  if (!lox.begin()) 
  {
    Serial.println("Попытка.");
    while(1);
  }
  Serial.println("Подключение успешно");
}
 
void loop()
 {
  VL53L0X_RangingMeasurementData_t measure;
  
  
  lox.rangingTest(&measure, false); 
 
  if (measure.RangeStatus != 4) 
  {  
    Serial.print("Дистанция(мм): "); Serial.println(measure.RangeMilliMeter);
  } 
  else 
  {
    Serial.println(" out of range ");
  }
    
  delay(100);
}