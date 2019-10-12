#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <SPI.h>

/*---VARIABLES ENCODER---*/
int sw = 8;                //Boton del encoder
int dt = 3;               //Salida 'A' del encoder rotativo
int clk = 4;              //Salida 'B' del encoder rotativo
int est_encoder = 0;
int ult_est_encoder = 0;
int ultimo_est_encoder = 0;

Encoder encoder (dt,clk);

LiquidCrystal_I2C lcd(0x27,16,2);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

float acelerador = 0;
float acelerador_pin = 9;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  pinMode(acelerador_pin,OUTPUT); 
}

void loop() {
  est_encoder = encoder.read();
  if(est_encoder > 1024){
    est_encoder = 1024;
    }
  if(est_encoder < 0){
    est_encoder = 0;
    }
  
  if( (est_encoder != ult_est_encoder)){
    acelerador = map(est_encoder,0,1024,0,1024);
    lcd.clear();          
    lcd.setCursor(0,0);
    lcd.print(est_encoder);
    lcd.print(" - ");
    lcd.print(acelerador);
  }  
  
  ult_est_encoder = est_encoder;
  
  if(digitalRead(sw) == 0){
    acelerador = 0;
    lcd.clear();          
    lcd.setCursor(0,0);
    lcd.print(est_encoder);
    lcd.print(" - ");
    lcd.print(acelerador);
  }
  
  analogWrite(acelerador_pin,acelerador);
}
