//Версия Arduino IDE 1.6.5

/* Таймер для инсектария ИСиЭЖ 
 *  Вохмин Евгений, 2021 год
 *  Версия 1.0 (2021)  
 */

#define dimPin 4 //пин для диммера
#define zeroPin 2 //пин детектора нуля
#include <CyberLib.h> // шустрая библиотека для таймера
volatile int tic, Dimmer;
#include <Wire.h> //библиотека onewire
#include <DS3231.h> // библиотека для часов реального времени

int sss; //секунды, минуты и часы, сделаю немного по-козлячьи
int hhh;
int mmm;
int level_light; // переменная для уровня освещенности
DS3231  rtc(SDA, SCL); //часы подключим по I2C

Time t; // не помню, но надо


void setup() {
  rtc.begin(); 
  Serial.begin(9600);
  pinMode(dimPin, OUTPUT);
  digitalWrite(dimPin, 0);
  pinMode(zeroPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, detect_up, FALLING);  // настроить срабатывание прерывания interrupt0 на pin 2 на низкий уровень

  StartTimer1(timer_interrupt, 40);        // время для одного разряда ШИМ
  StopTimer1();                            // остановить таймер

  Serial.println("Start");

}

void loop() {
  t = rtc.getTime();
  sss = t.sec, DEC;
  mmm = t.min, DEC;
  hhh = t.hour, DEC;

  if (hhh == 7 && mmm > 1) { //первый час рассвета
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 60, 3600, 255, 127);
    level_light = constrain (level_light, 127, 255);
    Dimmer = level_light;
  }
  
  if (hhh == 8 && mmm > 1) { // второй час рассвета
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 60, 3600, 127, 1);
    level_light = constrain (level_light, 1, 127);
    Dimmer = level_light;
  }

    if (hhh > 8 || hhh < 19 ) {
    Dimmer = 0;
  }

  if (hhh == 19 && mmm > 1) {
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 60, 3600, 1, 127);
    level_light = constrain (level_light, 1, 127);
    Dimmer = level_light;
  }
  
  if (hhh == 20 && mmm > 1) {
    level_light = (mmm * 60 + sss);
    level_light = map(level_light, 60, 3600, 127, 255);
    level_light = constrain (level_light, 127, 255);
    Dimmer = level_light;
  }

      if (hhh > 20 ) { //вечер после 20 - темнота
    Dimmer = 255;
  }

  if (hhh < 7 ) { //утро до 7 - темнота
    Dimmer = 255;
  }

delay (20);
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
void timer_interrupt() {       // прерывания таймера срабатывают каждые 40 мкс
  tic++;                       // счетчик
  if (tic > Dimmer)            // если настало время включать ток
    digitalWrite(dimPin, 1);   // врубить ток
}

void  detect_up() {    // обработка внешнего прерывания на пересекание нуля снизу
  tic = 0;                                  // обнулить счетчик
  ResumeTimer1();                           // перезапустить таймер
  attachInterrupt(0, detect_down, RISING);  // перенастроить прерывание
}

void  detect_down() {  // обработка внешнего прерывания на пересекание нуля сверху
  tic = 0;                                  // обнулить счетчик
  StopTimer1();                             // остановить таймер
  digitalWrite(dimPin, 0);                  // вырубить ток
  attachInterrupt(0, detect_up, FALLING);   // перенастроить прерывание
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
