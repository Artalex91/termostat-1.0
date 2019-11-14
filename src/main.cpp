#include <Arduino.h>

float tempC;
int reading;

uint32_t mill=0;
int tempVar=0;

int var1=0;
int var2=0;
int var3=0;
//int var4=0;


//Пин подключен к SH_CP входу 74HC595
int clockPin = 6;
//Пин подключен к ST_CP входу 74HC595
int latchPin = 7;
//Пин подключен к DS входу 74HC595
int dataPin = 8;
 
// Пины разрядов цифер
int pins_numbers[4] = {2, 3, 4, 5};
// Биты для отображения цифер от 0-9, минуса и символ градуса цельсия
byte numbers_array[12] = {
    B00111111, B00000110, B01011011, B01001111, // 0 1 2 3
    B01100110, B01101101, B01111101, B00000111, // 4 5 6 7
    B01111111, B01101111, B01000000, B01100011 // 8 9 - о
  };
  // Биты для отображения цифер от 0-9, минуса и символ градуса цельсия с точкой
byte numbers_arrayDot[12] = {
    B10111111, B10000110, B11011011, B11001111, // 0. 1. 2. 3.
    B11100110, B11101101, B11111101, B10000111, // 4. 5. 6. 7.
    B11111111, B11101111, B11000000, B11100011 // 8. 9. -. о.
  };


void showNumber(int numNumber, int number){
  // зажигаем нужные сегменты 
  digitalWrite(latchPin, LOW);
  if(numNumber==3) shiftOut(dataPin, clockPin, MSBFIRST, numbers_arrayDot[number]); 
  else             shiftOut(dataPin, clockPin, MSBFIRST, numbers_array[number]); 
  digitalWrite(latchPin, HIGH);
 
  // включаем нужный разряд(одну из четырех цифр)
  int num_razryad = pins_numbers[numNumber-1];
  for(int i; i<4; i++){
    // выключаем все
    digitalWrite(pins_numbers[i], HIGH);
  }
  // включаем нужную
  digitalWrite(num_razryad, LOW);
     
  delay(5);
}

  
void setup() {
  analogReference(INTERNAL);        // включаем внутрений источник опорного 1,1 вольт
  Serial.begin(9600);

    //устанавливаем режим OUTPUT
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    for(int i; i<4; i++){
    pinMode(pins_numbers[i], OUTPUT);
    }
}
  
void loop() {
if(millis()-mill>1000){
    mill=millis();
  reading = analogRead(A0);        // получаем значение с аналогового входа A0
  tempC = reading / 9.31;          // переводим в цельсии 
  Serial.print(tempC);            // отправляем в монитор порта
  Serial.print(" C   ");

  tempVar=tempC*100;
  var1 = tempVar / 1000;
  var2 = tempVar % 1000 / 100;
  var3 = tempVar % 100 / 10;
  //var4 = tempVar % 10;


}



 // включить сразу несколько цифр нельзя, поэтому очень быстро показываем по одной
  showNumber(1, 11);
  showNumber(2, var3);
  showNumber(3, var2);
  showNumber(4, var1);
}
  



  
