#include <Arduino.h>

// библиотека для работы с датчиками MQ (Troyka-модуль)
#include <TroykaMQ.h>
#include <iarduino_OLED_txt.h>                               // Подключаем библиотеку iarduino_OLED_txt.
iarduino_OLED_txt myOLED(0x3C);                              // Объявляем объект myOLED, указывая адрес дисплея на шине I2C: 0x3C или 0x3D.

extern uint8_t MediumFontRus[];                              // Подключаем шрифт MediumFontRus.
extern uint8_t SmallFontRus[];                                            // Подключаем шрифт SmallFontRus.
 
// имя для пина, к которому подключен датчик
#define PIN_MQ2         A0
 
// создаём объект для работы с датчиком
// и передаём ему номер пина выходного сигнала
MQ2 mq2(PIN_MQ2);

void setup()
{
  // открываем последовательный порт
  Serial.begin(115200);
  // запускаем термоцикл
  // в течении 60 секунд на нагревательный элемент подаётся 5 вольт
  // в течении 90 секунд — 1,5 вольта
  mq2.cycleHeat();

    myOLED.begin();                                          // Инициируем работу с дисплеем.
    myOLED.setFont(MediumFontRus);                           // Указываем шрифт который требуется использовать для вывода цифр и текста.

    myOLED.print(F("Датчик MQ2"), 15,     1);                  // Выводим текст начиная с 15 столбца 4 строки (высота шрифта 2 строки, он займёт строки 3 и 4).

    myOLED.setFont(SmallFontRus);
}
 
void loop()
{
  // если прошёл интервал нагрева датчика
  // и калибровка не была совершена
  if (!mq2.isCalibrated() && mq2.atHeatCycleEnd()) {
    // выполняем калибровку датчика на чистом воздухе
    mq2.calibrate();
    // выводим сопротивление датчика в чистом воздухе (Ro) в serial-порт
    Serial.print("Ro = ");
    Serial.println(mq2.getRo());
    // запускаем термоцикл
    mq2.cycleHeat();
  }
  // если прошёл интервал нагрева датчика
  // и калибровка была совершена
  if (mq2.isCalibrated() && mq2.atHeatCycleEnd()) {
    // выводим отношения текущего сопротивление датчика
    // к сопротивлению датчика в чистом воздухе (Rs/Ro)
    int val = analogRead(PIN_MQ2);

    Serial.print("Ratio: ");
    float ratio = mq2.readRatio();
    Serial.print(ratio);
    // выводим значения газов в ppm
    Serial.print("LPG: ");
    Serial.print(mq2.readLPG());
    Serial.print(" ppm ");
    Serial.print(" Methane: ");
    float val_methane = mq2.readMethane();
    Serial.print(val_methane);
    Serial.print(" ppm ");
    Serial.print(" Smoke: ");
    Serial.print(mq2.readSmoke());
    Serial.print(" ppm ");
    Serial.print(" Hydrogen: ");
    Serial.print(mq2.readHydrogen());
    Serial.println(" ppm ");

    myOLED.clrScr();                                         // Чистим экран.
    myOLED.setCursor(15,1);
    myOLED.print(F("Ratio "));
    myOLED.print(ratio);
    myOLED.setCursor(15,2);
    myOLED.print(val_methane);
    myOLED.print(F(" ppm "));
    myOLED.setCursor(15,3);
    myOLED.print(val);
    delay(100);
    // запускаем термоцикл
    mq2.cycleHeat();
  }
}