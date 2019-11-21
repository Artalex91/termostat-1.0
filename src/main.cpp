#include <Arduino.h>
//-------encoder-------------
const int SWPin=3;
const int CLKPin=2;
const int DTPin=11;
volatile int encCounter;
volatile boolean state0, lastState, turnFlag;
//-------encoder-------------
//-------rele----------------
const int relePin=13;
bool rele=false;
//-------rele----------------
//-------termometr-----------
float tempC;
int reading;
uint32_t mill=0;
int tempVar=0;
int var1=0;
int var2=0;
int var3=0;
//int var4=0;
//-------termometr-----------
//-------display-------------
bool dot=false; // отображение точки
//Пин подключен к SH_CP входу 74HC595
const int clockPin = 8;
//Пин подключен к ST_CP входу 74HC595
const int latchPin = 9;
//Пин подключен к DS входу 74HC595
const int dataPin = 10;
// Пины разрядов цифер
const int pins_numbers[4] = {4, 5, 6, 7};
// Биты для отображения цифер от 0-9, минуса и символ градуса цельсия
byte numbers_array[14] = {
    B00111111, B00000110, B01011011, B01001111, // 0 1 2 3
    B01100110, B01101101, B01111101, B00000111, // 4 5 6 7
    B01111111, B01101111, B01000000, B01100011, // 8 9 - о
    B01100010, B00011100                        // _ ~
  };
  // Биты для отображения цифер от 0-9, минуса и символ градуса цельсия с точкой
byte numbers_arrayDot[14] = {
    B10111111, B10000110, B11011011, B11001111, // 0. 1. 2. 3.
    B11100110, B11101101, B11111101, B10000111, // 4. 5. 6. 7.
    B11111111, B11101111, B11000000, B11100011, // 8. 9. -. о.
    B11100010, B10011100                        // _ ~
  };
//-------display-------------

void showNumber(int numNumber, int number){
  // зажигаем нужные сегменты 
  digitalWrite(latchPin, LOW);
  if(dot) shiftOut(dataPin, clockPin, MSBFIRST, numbers_arrayDot[number]); 
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

void encoder() {
state0 = digitalRead(CLKPin);
if (state0 != lastState) {
encCounter += (digitalRead(DTPin) != lastState) ? -1 : 1;

lastState = state0;
}
}

  
void setup() {
  Serial.begin(9600);
//-------encoder--------------
  pinMode(SWPin, INPUT_PULLUP);
  pinMode(CLKPin, INPUT);
  pinMode(DTPin, INPUT);
  attachInterrupt(0, encoder, CHANGE);
//-------encoder--------------
//-------termometr------------
  analogReference(INTERNAL);        // включаем внутрений источник опорного 1,1 вольт
//-------termometr------------
//-------rele-----------------
    pinMode(relePin, OUTPUT);
//-------rele-----------------
//-------display--------------
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    for(int i; i<4; i++){
    pinMode(pins_numbers[i], OUTPUT);
    }
//--------display-------------
}
  
void loop() {

    if(millis()-mill>1000){//события кажую секунду
      mill=millis();
      //----------termometr-----------------
      reading = analogRead(A0);        // получаем значение с аналогового входа A0 (0-110 градусов)
      tempC = reading / 9.31;          // переводим в цельсии 
      Serial.print(tempC);            // отправляем в монитор порта
      Serial.println(" C");
      tempVar=tempC*100;              //переводим float в int(4 значное число)
      //----------termometr-------------------

      var1 = tempVar / 1000;
      var2 = tempVar % 1000 / 100;
      var3 = tempVar % 100 / 10;
      //var4 = tempVar % 10;

    if     (tempVar>3300) rele=true;
    else if(tempVar<2900) rele=false;
    digitalWrite(relePin, rele); //в реальном реле управление по минусу(временно 13 диод по +)
    }




  // включить сразу несколько цифр нельзя, поэтому очень быстро показываем по одной
    //showNumber(1, 11);   // 4я
    showNumber(2, var3); // 3я
    dot=true;
    showNumber(3, var2); // 2я
    dot=false;
    showNumber(4, var1); // 1я

  showNumber(1, encCounter);   // 4я
  


}
  



  
