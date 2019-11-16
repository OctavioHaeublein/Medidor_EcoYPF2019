#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <SPI.h>
#include <SD.h>

/*---VARIABLES ENCODER---*/
int sw = 8;                                 //Boton del encoder
int dt = 3;                                 //Salida 'A' del encoder rotativo
int clk = 4;                                //Salida 'B' del encoder rotativo
int est_encoder = 0;
int ultimo_est_encoder = 0;
Encoder encoder (dt,clk);

LiquidCrystal_I2C lcd(0x27,20,4);           //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

  int posicion_principal = 0;               //Numero de archivo principal
  int posicion_secundario = 0;              //Numero de archivo secundario
  const int cs = 8;
  File datos;
  String datos_principal = "DPRIM_";        //Nombre del archivo principal de adquisición de datos
  String datos_secundario = "DSEC_";        //Nombre del archivo secundario
  String extension = ".txt";                //Extensión del archivo para concatenarlos
  bool guardar_secundario = false;          //Indica si deberan guardarse en archivo a parte los valores obtenidos

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.print("Cargando...");

  Serial.begin(9600);

}

void loop() {

  if(SD.begin(8)){
    Serial.println("Exito");
  }else{
      Serial.println("Fail");
    }

    delay(2000);
}
