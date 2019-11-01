#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <SPI.h>
//#define LR1 23
/*---VARIABLES ENCODER---*/
int sw = 8;                //Boton del encoder
int dt = 3;               //Salida 'A' del encoder rotativo
int clk = 4;              //Salida 'B' del encoder rotativo
int est_encoder = 0;
int ultimo_est_encoder = 0;

Encoder encoder (dt,clk);

LiquidCrystal_I2C lcd(0x27,16,2);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

float acelerador = 0;
float acelerador_map = 0;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();

  pinMode (A0, INPUT);
  pinMode (9, INPUT_PULLUP);
  
}

void loop() {

  acelerador = analogRead(A0);
  acelerador_map = map(acelerador, 0,1024,0.0,5.0);

  lcd.clear();                        //Limpia el LCD de todo lo que tenga escrito
  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la primer fila
  lcd.print(acelerador_map);
  lcd.print("V: (");
  lcd.print(acelerador);
  lcd.print(")");

  if(digitalRead(9) == 0){
    lcd.setCursor(0,1);
    lcd.print("SI");
    }else{
      lcd.setCursor(0,1);
      lcd.print("NO");
    }
  
  delay(200);

}
