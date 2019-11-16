#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

Encoder encoder(0,1);
LiquidCrystal_I2C lcd(0x27,16,2);
String posicion = "";

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(3, INPUT_PULLUP);
  
}

void loop() {
  lcd.clear();                        //Limpia el LCD de todo lo que tenga escrito
  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la primer fila
  lcd.print(posicion);                   //Escribe el error donde debería estar el cursor

  posicion = String(encoder.read());
  if(digitalRead(3)== 0){
    posicion = "0";
    }
  delay(100);
}
