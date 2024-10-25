#include <Adafruit_VL53L0X.h>      // Библиотека для работы с датчиком VL53L0X
#include <FastBot.h>               // Библиотека для работы с Telegram-ботом

#define BOT_TOKEN "7507860606:AAGDnTC9aiX3WBj6efz9z9rxC4bDSnUm_5c"
#define WIFI_SSID "pppp"//"pppp"
#define WIFI_PASS "44444444"//"44444444"
#define CHAT_ID "-4587588507"
#define NUM_MEASUREMENTS 11 // Количество замеров
byte HOUR_SEND[2]= {6, 19};
float minDistance = 50;
float maxDistance = 800;
int POROG = 20;

int distances[11] = {0};

Adafruit_VL53L0X lox = Adafruit_VL53L0X();


FastBot bot(BOT_TOKEN);

// Пороговые значения
const byte CRITICAL_FEED_LEVEL = 20;     // Критический уровень в процентах
const unsigned long CHECK_INTERVAL = 10*60*1000;      // Интервал проверки уровня (каждые 10 минут)
const unsigned long CHECK_SOS_INTERVAL = 1*60*1000; // Интервал уведомлений для отлова сос режима

// Временные метки и другие переменные
unsigned long lastCheckTime = 0;
unsigned long lastNotificationTime = 0;
bool criticalMode = false;
int feedLevel = 100;                    // Начальный уровень корма (условно 100%)
bool FLAG_SOS = 0;
unsigned int START_TIME, START_TIME_SOS = 0;
byte LAST_HOUR, NOW_HOUR = 0;
byte LAST_SEND;


void connectWiFi();
float Get_Distance();
byte get_time();
void SOS_CONTROL();
long Now_level();
float Get_Distance();
void send_info();
void connectWiFi();

void setup() 
{
  Serial.begin(115200);
  //EEPROM.begin(512);                    // Инициализация EEPROM для хранения данных

  
  Serial.println("Подключение модуля инфракрасного дальномера");

  while(lox.Status == 0)
  {
    Serial.println("Попытка подключения.");
    delay(500);
  }
  Serial.println("Подключение модуля инфракрасного дальномера успешно");

  
  connectWiFi();

  bot.setChatID(CHAT_ID);
  //bot.attach(newMsg);

  bot.sendMessage("Кормушка подключена");
  START_TIME = millis();
  START_TIME_SOS = millis();
  send_info();
  

}

void loop()
{
  
  if (millis()-START_TIME>CHECK_INTERVAL)
  {
    START_TIME = millis();
    send_info();
  }

  if (millis() - START_TIME_SOS > CHECK_SOS_INTERVAL)
  {
    START_TIME_SOS = millis();
    SOS_CONTROL();
  }

  bot.tick();
  
}

byte get_time()
{
  FB_Time t(bot.getUnix(), 3);
  return t.hour;
}


void SOS_CONTROL()
{
  if (Now_level() < POROG)
  {
    FLAG_SOS = 1;
    Serial.println("Мало корма!");
    bot.sendMessage("Уровень корма в кормушке:" +String(Now_level()));
    bot.sendMessage("Мало корма!");
  }
  else FLAG_SOS = 0;
}

long Now_level()
{
  float now_dist = Get_Distance();
  float level = map(now_dist, minDistance, maxDistance, 0, 100);
  return level;
  
}

float Get_Distance()
{
  return 150;
  
    VL53L0X_RangingMeasurementData_t measure;

    // Делаем 10 замеров
    for (byte i = 0; i < NUM_MEASUREMENTS; i++) 
    {
        lox.rangingTest(&measure, false); 
        distances[i] = measure.RangeMilliMeter;
        delay(50); // Задержка между измерениями
    }

    // Сортировка массива для нахождения медианы
    // Сортируем массив расстояний
    for (int i = 0; i < NUM_MEASUREMENTS - 1; i++) {
        for (int j = 0; j < NUM_MEASUREMENTS - i - 1; j++) {
            if (distances[j] > distances[j + 1]) {
                // Обмен значениями
                int temp = distances[j];
                distances[j] = distances[j + 1];
                distances[j + 1] = temp;
            }
        }
    }

    // Нахождение медианы
    float median;
    if (NUM_MEASUREMENTS % 2 == 0) // Четное количество элементов
    {
        median = (distances[NUM_MEASUREMENTS / 2 - 1] + distances[NUM_MEASUREMENTS / 2]) / 2.0; // Среднее значение двух центральных
    } 
    else // Нечетное количество элементов
    {
        median = distances[NUM_MEASUREMENTS / 2]; // Центральное значение
    }

  return median;
  
}


void send_info()
{
  NOW_HOUR = get_time(); 
  
  for (byte i = 0; i < sizeof(HOUR_SEND); i++)
  {
    if ((i != LAST_SEND)&&(NOW_HOUR==HOUR_SEND[i]))
    {
      LAST_SEND = i;
      Serial.println("Уровень корма в кормушке:" +String(Now_level()));
      bot.sendMessage("Уровень корма в кормушке:" +String(Now_level()));
    }
  }

}

void connectWiFi()
{
  delay(2000);

  //Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (millis() > 15000)
      break;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Not connected");
  }
  else
  {
    bot.setChatID(-837409212);
    //bot.attach(newMsg);
    bot.sendMessage("FARM AEROPONIC IS CONNECTED.");
    Serial.println("Connected");
  }

  // digitalWrite(LED_BUILTIN, HIGH);
}