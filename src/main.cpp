#include <Arduino.h>
#include <avr/eeprom.h>

//-------mode----------------
int mode=0;
volatile bool SWFlag=false;
volatile uint32_t SWFlagMill=0;
uint8_t modeBlink=0;
volatile uint32_t modeMill=0;
bool modeMillFlag=false;
//-------mode----------------
//-------encoder-------------
const int SWPin=3;
const int CLKPin=2;
const int DTPin=11;
volatile int16_t encCounter1;
volatile int16_t encCounter2;
volatile int8_t  encCounter3;
volatile boolean state0, lastState, turnFlag;
//-------encoder-------------
//-------rele----------------
const int relePin=12;
const int relePin13=13;
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
int var4=0;
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
byte numbers_array[21] = {
    B00111111, B00000110, B01011011, B01001111, // 0 1 2 3
    B01100110, B01101101, B01111101, B00000111, // 4 5 6 7
    B01111111, B01101111, B01000000, B01100011, // 8 9 - о
    B10011000, B10100001, B01101101, B00111110, // ,, " S V 
    B01111001, B00000000, B01110111, B01101110, // E   A Y
    B11010100                                   // n 
  };
  // Биты для отображения цифер от 0-9, минуса и символ градуса цельсия с точкой
byte numbers_arrayDot[21] = {
    B10111111, B10000110, B11011011, B11001111, // 0. 1. 2. 3.
    B11100110, B11101101, B11111101, B10000111, // 4. 5. 6. 7.
    B11111111, B11101111, B11000000, B11100011, // 8. 9. -. о.
    B10011000, B10100001, B11101101, B10111110, // ,,. ". S. V. 
    B11111001, B10000000, B11110111, B11101110, // E.  . A. Y.
    B11010100                                   // n.
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
modeMill=millis();
state0 = digitalRead(CLKPin);
if (state0 != lastState) {
  switch (mode){
  case 1: encCounter1 += (digitalRead(DTPin) != lastState) ? -5 : 5; break;
  case 2: encCounter2 += (digitalRead(DTPin) != lastState) ? -5 : 5; break;
  case 3: encCounter3 += (digitalRead(DTPin) != lastState) ? -1 : 1; break;
  }
  lastState = state0;
}
}

void SWencoder() {
    SWFlag=true;
    SWFlagMill=millis();
    modeMill=millis();
    }

  
void setup() {
//  Serial.begin(9600);
//-------encoder--------------
  pinMode(SWPin, INPUT_PULLUP);
  pinMode(CLKPin, INPUT);
  pinMode(DTPin, INPUT);
  attachInterrupt(0, encoder, CHANGE);
  attachInterrupt(1, SWencoder, FALLING);
//-------encoder--------------
//-------termometr------------
  analogReference(INTERNAL);        // включаем внутрений источник опорного 1,1 вольт
//-------termometr------------
//-------rele-----------------
    pinMode(relePin, OUTPUT);
    pinMode(relePin13, OUTPUT);
//-------rele-----------------
//-------display--------------
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    for(int i; i<4; i++){
    pinMode(pins_numbers[i], OUTPUT);
    }

    encCounter1 = eeprom_read_word(0);
    delay(10);
    encCounter2 = eeprom_read_word(2);
    delay(10);
//--------display-------------
}
  
void loop() {
  

  if(digitalRead(SWPin)==LOW && SWFlag==true && millis()-SWFlagMill>100){
    SWFlag=false;
    mode++;
  }

  if(mode!=0 && modeMillFlag==false) {
    modeMill=millis(); 
    modeMillFlag=true;
    }
  if(millis()-modeMill>20000 && modeMillFlag==true){
    encCounter1 = eeprom_read_word(0);
    delay(10);
    encCounter2 = eeprom_read_word(2);
    delay(10);
    mode=0; 
    modeMillFlag=false;
    }

  switch (mode){
    case 0:
      if(millis()-mill>500){//события каждые полсекунды
        mill=millis();
        //----------termometr-----------------
        reading = analogRead(A0);        // получаем значение с аналогового входа A0 (0-110 градусов)
        tempC = reading / 9.31;          // переводим в цельсии 
       // Serial.print(tempC);            // отправляем в монитор порта
       // Serial.println(" C");
        tempVar=tempC*10;              //переводим float в int(4 значное число)
        //----------termometr-------------------

        var1 = tempVar / 100;
        var2 = tempVar % 100 / 10;
        var3 = tempVar % 10;
        
        

      if     (tempVar>encCounter1) rele=true;
      else if(tempVar<encCounter2) rele=false;
      digitalWrite(relePin, !rele); //само реле по минусу + выносная индикация
      digitalWrite(relePin13, rele); // индикация работы реле на плате
      }


    // включить сразу несколько цифр нельзя, поэтому очень быстро показываем по одной
      showNumber(1, 11);   // 4я
      showNumber(2, var3); // 3я
      dot=true;
      showNumber(3, var2); // 2я
      dot=false;
      showNumber(4, var1); // 1я
  break;

  case 1:
      if      (encCounter1<0) encCounter1=995;
      else if (encCounter1>995) encCounter1=0;


        var1 = encCounter1 / 100;
        var2 = encCounter1 % 100 / 10;
        var3 = encCounter1 % 10;


      showNumber(1, var3); // 4я
      dot=true;
      showNumber(2, var2); // 3я
      dot=false;
      showNumber(3, var1); // 2я

      if(millis()-mill>500){
        mill=millis();
        if     (modeBlink==0) modeBlink=1;
        else if(modeBlink==1) modeBlink=0;
      }
      switch (modeBlink){
        case 0: showNumber(4, 13); break; // 1я
        case 1: showNumber(4, 17); break; // 1я
      }

  break;

  case 2:
      if      (encCounter2<0) encCounter2=encCounter1;
      else if (encCounter2>encCounter1) encCounter2=0;
        var1 = encCounter2 / 100;
        var2 = encCounter2 % 100 / 10;
        var3 = encCounter2 % 10;

      showNumber(1, var3); // 4я
      dot=true;
      showNumber(2, var2); // 3я
      dot=false;
      showNumber(3, var1); // 2я

      if(millis()-mill>500){
        mill=millis();
        if     (modeBlink==0) modeBlink=1;
        else if(modeBlink==1) modeBlink=0;
      }
      switch (modeBlink){
        case 0: showNumber(4, 12); break; // 1я
        case 1: showNumber(4, 17); break; // 1я
      }
  break;

  case 3:
      
      if      (encCounter3<0) encCounter3=2;
      else if (encCounter3>2) encCounter3=0;

      if(millis()-mill>500){
        mill=millis();
        if     (modeBlink==0) modeBlink=1;
        else if(modeBlink==1) modeBlink=0;
      }
      switch (modeBlink) {
        case 0:
          switch (encCounter3){
            case 0://save
              showNumber(1, 16); // 4я
              showNumber(2, 15); // 3я
              showNumber(3, 18);  // 2я
              showNumber(4, 14); // 1я
              break;

            case 1://no
              showNumber(1, 17); // 4я
              showNumber(2, 0); // 3я
              showNumber(3, 20 ); // 2я
              showNumber(4, 17); // 1я
              break;
            case 2://yes
              showNumber(1, 14); // 4я
              showNumber(2, 16); // 3я
              showNumber(3, 19); // 2я
              showNumber(4, 17); // 1я
              break;
          }
        break;

        case 1:
        showNumber(1, 17); // 4я
        showNumber(2, 17); // 3я
        showNumber(3, 17); // 2я
        showNumber(4, 17); // 1я
        break;
      }
  break;

  case 4:
    switch (encCounter3){
      case 0:
        encCounter3=0;
        var1 = tempVar / 100;
        var2 = tempVar % 100 / 10;
        var3 = tempVar % 10;
        mode=0;
      break;
      
      case 1:
        encCounter3=0;
        var1 = tempVar / 100;
        var2 = tempVar % 100 / 10;
        var3 = tempVar % 10;
        mode=0;
      break;

      case 2:
        showNumber(4, 10); // 1я
        delay(200);
        showNumber(3, 10); // 2я
        delay(200);
        showNumber(2, 10); // 3я
        delay(200);
        showNumber(1, 10); // 4я
        delay(200);
        
        
        eeprom_update_word(0, encCounter1);
        delay(10);
        eeprom_update_word(2, encCounter2);
        delay(10);
        encCounter3=0;
        mode=0;
      break;
     }
     break;
   }
}
  



  
