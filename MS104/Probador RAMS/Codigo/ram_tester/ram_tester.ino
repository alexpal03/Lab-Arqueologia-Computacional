
#include "LiquidCrystal_DTWI.h"

DTWI1 myI2C2;
LiquidCrystal_DTWI lcd(0x27, 16, 2, myI2C2);

// Distintas direcciones para probar
enum add_e {
  ADD1,
  ADD2 = 0x2000,
  ADD3 = 0x4000,
  ADD4 = 0x6000,
  ADD5 = 0x8000,
  ADD6 = 0xA000,
  ADD7 = 0xC000,
  ADD8 = 0xE000,
};


// Declaracion de pines
//                       A0 A1 A2  A3  A4  A5 A6 A7 A8  A9 A10 A11 A12  A13 A14 A15
const char ADD_PINS[] = {0, 1, 2, 13, 14, 15, 3, 4, 5, 10, 9, 8,  11,   6,  7,  12};

//                        D0  D1  D2  D3  D4  D5  D6  D7
const char DATA_PINS[] = {22, 23, 16, 17, 21, 20, 19, 18};


const int OE  = 30;
const int WE  = 31;

const char LEDS[] = {37, 38, 39, 40, 41, 42, 43, 44};

const int BTN = 36;

// Cantidad de direcciones con error
unsigned long int errorCount = 0;

// Informacion sobre los bis que fallan en grupo de 1024 direcciones
byte errorBits[8];


// Configuracion
enum add_e initialAddress = ADD1;


void setAddress(unsigned long add) {
  for (int i = 0; i < sizeof(ADD_PINS); ++i) {
    digitalWrite(ADD_PINS[i], (add >> i) & 0x01);
  }
}

void setWriteMode() {
  
  delayMicroseconds(50);
  digitalWrite(OE, HIGH);
  delayMicroseconds(50);

  for (int i = 0; i < sizeof(DATA_PINS); ++i) {
    pinMode(DATA_PINS[i], OUTPUT);
  }
  
  delayMicroseconds(50);
  digitalWrite(WE, LOW);
  delayMicroseconds(50);
}

void setReadMode() {


  delayMicroseconds(50);
  digitalWrite(WE, HIGH);
  delayMicroseconds(50);

  for (int i = 0; i < sizeof(DATA_PINS); ++i) {
    pinMode(DATA_PINS[i], INPUT);
  }
  
  delayMicroseconds(50);
  digitalWrite(OE, LOW);
  delayMicroseconds(50);
}


byte waitClick() {
  byte clickCounter = 0;
  
  // Esperar primer click
  // Anti bounce
  unsigned long in;
  do {
    in = millis();
  
    while (digitalRead(BTN) == 0);
    // Esperar soltar boton
    while (digitalRead(BTN) == 1);

  } while (millis() - in < 200);

  
  ++clickCounter;

  

  // Esperar segundo click
  byte clickTimeOut = 6;
  do {
    --clickTimeOut;
    delay(100);
  } while (digitalRead(BTN) == 0 && clickTimeOut);

  // Si el loop terminó por un segundo click
  if (clickTimeOut) {
    ++clickCounter;
  }

  return clickCounter;

}

// Funcion encargada de realizar la logica de escribir un dato,
// volver a leerlo y retornar el valor leido
byte testValue(byte targetData) {

  setWriteMode();
  
  for (int i = 0; i < sizeof(DATA_PINS); ++i) {
    digitalWrite(DATA_PINS[i], (targetData >> i) & 0x01);
  }

  setReadMode();

  byte readData = 0;

  for (int i = 0; i < sizeof(DATA_PINS); ++i) {
    readData |= (digitalRead(DATA_PINS[i]) & 0x01) << i;
  }
  
  return readData;
}

void setup() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RAM TESTER V1.0");

  // Configurar pines
  for (int i = 0; i < sizeof(ADD_PINS); ++i) {
    pinMode(ADD_PINS[i], OUTPUT);
  }

  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);

  pinMode(BTN, INPUT);

  for (int i = 0; i < sizeof(LEDS); ++i) {
    pinMode(LEDS[i], OUTPUT);
  }


  delay(2000);
}

void loop() {

  errorCount = 0;
  for (int i = 0; i < sizeof(errorBits); ++i) {
    errorBits[i] = 0;
  }


  for (int i = 0; i < sizeof(LEDS); ++i) {
    digitalWrite(LEDS[i], LOW);
  }


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1 click = TEST");
  lcd.setCursor(0, 1);
  lcd.print("2 click = CONF");

  byte clickCounter = waitClick();

  switch(clickCounter){
    case 1:{
  
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Testeando...");

      byte readData, targetData;

      // Recorrer todas las direcciones hasta la direccion maxima
      unsigned long add = initialAddress;
      unsigned long maxAdd = initialAddress + 8 * 1024;
      for (; add < maxAdd; ++add) {

        
        lcd.setCursor(0, 1);
        lcd.print((add-initialAddress)*100/(maxAdd-initialAddress));
        lcd.print("%");
        
        setAddress(add);

        byte currentCol = (add >> 10) & 0b000111;

        // Escribir todos 1 en cada celda
        
        targetData = 0xff;
        readData = testValue(targetData);
        errorBits[currentCol] |= readData ^ targetData;

        // Escribir todos 0 en cada celda

        targetData = 0x00;
        readData = testValue(targetData);
        errorBits[currentCol] |= readData ^ targetData;

        // Si hay algun error en el bloque de 1024 encender el led correspondiente
        if (errorBits[currentCol]) {
          errorCount++;
          digitalWrite(LEDS[currentCol], HIGH);
        }
        
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Finalizado: ");
      lcd.setCursor(0, 1);
      lcd.print("errC: ");
      lcd.print(errorCount);
      
      // Esperar que se presione el boton
      waitClick();

      // Mostrar bits con error
      lcd.clear();
      lcd.setCursor(0, 0);
      for (int i = 0; i < 4; ++i) {
        lcd.print(String(errorBits[i], HEX) + " ");
      }
      lcd.setCursor(0, 1);
      for (int i = 4; i < 8; ++i) {
        lcd.print(String(errorBits[i], HEX) + " ");
      }

    
      // Esperar que se presione el boton
      waitClick();
      }
      break;

      
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CONFIGURACION");

      while(1){
      
        lcd.setCursor(0, 1);
        lcd.print("add: ");
        switch (initialAddress) {
          case ADD1:
          
            lcd.print("0000");
            break;
          case ADD2:
          
            lcd.print("2000");
            break;

          case ADD3:
          
            lcd.print("4000");
            break;
            
          case ADD4:
          
            lcd.print("6000");
            break;
            
          case ADD5:
          
            lcd.print("8000");
            break;
            
          case ADD6:
          
            lcd.print("A000");
            break;
            
          case ADD7:
          
            lcd.print("C000");
            break;

            
          case ADD8:
          
            lcd.print("E000");
            break;
        }
  
        clickCounter = waitClick();
  
        if (clickCounter == 1) {
          // Siguiente opcion
          initialAddress = initialAddress == ADD8 ? ADD1 : (enum add_e)(initialAddress + 0x2000);
        }
        // Siguiente menu
        else break;
      }

      break;
  }

  
  
}
