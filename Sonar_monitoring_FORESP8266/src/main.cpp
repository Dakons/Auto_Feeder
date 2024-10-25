
//#include <NewPing.h>

#include "NewPingESP8266.h"
#define TRIGGER_PIN  1//17 
#define ECHO_PIN    3//16  
#define MAX_DISTANCE 450 

#define LENGTH_LOWEST 2500
#define LENGTH_HIGHEST 15000
NewPingESP8266 sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
float DIST_MASSIVE_1[3] = {0.0, 0.0, 0.0};   // массив для хранения трёх последних измерений
float dist, dist_filtered;
byte i_MASSIVE_1;


byte i, delta;

byte zerocount = 0;

unsigned long sensTimer = 0; //Таймер 

float ReadAndFilterUS(float dist);
float middle_of_3(float a, float b, float c);

float linearInterpolation(float x, float x_min, float x_max, float y_min, float y_max);
void setup() 
{
  Serial.begin(115200); 
}

void loop() 
{
  if (millis() - sensTimer > 50) 
  {
  //delay(100);
  float distantion = (float)sonar.ping();// / 57.5;
  Serial.print(">Первичный сигнал:");
  Serial.println(distantion);
 //float distantion_filtered = ReadAndFilterUS(distantion);

float distantion_filtered = linearInterpolation(ReadAndFilterUS(distantion), LENGTH_LOWEST, LENGTH_HIGHEST, 0.0, 100.0);
 Serial.print(">Отфильтрованный сигнал в процентах:");

  Serial.println(distantion_filtered);
  sensTimer = millis();  
  }
}



// Функция линейной интерполяции
float linearInterpolation(float x, float x_min, float x_max, float y_min, float y_max) 
{
    // Проверка на выход за границы диапазона x
    if (x <= x_min) return y_min;
    if (x >= x_max) return y_max;

    // Линейная интерполяция
    return y_min + (y_max - y_min) * (x - x_min) / (x_max - x_min);
}


float ReadAndFilterUS(float dist) //фильтрация полученных данных с датчика ткоа
{
    float k = 0.7;
    float delta = abs(dist_filtered - dist);

    if ((dist_filtered > 25) && (dist == 0))
    {
      dist = dist_filtered;
      zerocount++;
    }
    else
    {
 zerocount = 0;
    }   

    if (zerocount > 15)
    {
      zerocount = 0;
      dist = 0;
      dist_filtered = 0;
      //k = 0;
    }

    if (delta > 100)
    {
      k = 0.01;
    }
     if (delta > 50)
    {
      k = 0.1;
    }

    if (i_MASSIVE_1 > 1)
            i_MASSIVE_1 = 0;
        else
            i_MASSIVE_1++;
        DIST_MASSIVE_1[i_MASSIVE_1] = dist;
        dist = middle_of_3(DIST_MASSIVE_1[0], DIST_MASSIVE_1[1], DIST_MASSIVE_1[2]);

    dist_filtered += (dist - dist_filtered) * k; // фильтр "бегущее среднее"


    return dist_filtered; //+0.43
}

float middle_of_3(float a, float b, float c)
 {
  float middle;
  if ((a <= b) && (a <= c)) {
    middle = (b <= c) ? b : c;
  }
  else {
    if ((b <= a) && (b <= c)) {
      middle = (a <= c) ? a : c;
    }
    else {
      middle = (a <= b) ? a : b;
    }
  }
  return middle;
}