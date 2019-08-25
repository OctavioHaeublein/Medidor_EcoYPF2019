#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

/*---VARIABLES MODULO SD---*/
int cs = 53;

/*---VARIABLES SENSORES---*/
int frecuencia_rueda = 0;
float tension_divisor = 0;
float tension_hall = 0;


/*---VARIABLES PROGRAMA---*/
float tiempo = 0;               //Tiempo transcurrido del programa en horas
float tiempo_actual = 0;        //Variable auxiliar para el calculo del tiempo
float tiempo_previo = 0;        //Variable auxiliar para el calculo del tiempo

String archivo = "";            //Nombre del archivo al que deben ser guardados los datos en la tarjeta SD
int muestreo = 1000;            //Frecuencia con la que se almacenan los datos en la tarjeta SD
bool doble = false;             //Booleano que comprueba si los datos deberan guardarse en un archivo a parte

float tension = 0;              //Nivel de tension de las baterias
float corriente = 0;            //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             //Potencia del motor
float energia = 0;              //Energia consumida por el motor
float velocidad = 0;            //Velocidad (Km/h) del vehículo
//bool carga = false;             //Booleano que indica el estado de carga de las baterias

void setup() {
  Serial.begin(9600);           //Inicia una comunicacion serial para edicion de codigo
  tiempo_actual = millis();     //Toma el tiempo de inicio del programa en milisegundos
  attachInterrupt(digitalPinToInterrupt(2), blink, LOW);
  
  /*---DIGITAL OUTPUTS---*/
  pinMode(23,OUTPUT);   //LED Rojo1
  pinMode(25,OUTPUT);   //LED Rojo
  pinMode(27,OUTPUT);   //LED Amarillo
  pinMode(29,OUTPUT);   //LED Amarillo1
  pinMode(31,OUTPUT);   //LED Amarillo2
  pinMode(33,OUTPUT);   //LED Verde
  pinMode(35,OUTPUT);   //LED Verde1
  pinMode(37,OUTPUT);   //LED Verde2
  pinMode(39,OUTPUT);   //LED Verde3
  pinMode(41,OUTPUT);   //LED Verde4
  
  /*---DIGITAL PWM OUTPUTS---*/
  pinMode(4,OUTPUT);    //RGB Rojo
  pinMode(5,OUTPUT);    //RGB Verde
  pinMode(6,OUTPUT);    //RGB Azul
  
  /*---DIGITAL INPUTS---*/
  pinMode(2,INPUT);     //Primer switch
  pinMode(3, INPUT);    //Señal cuadrada para velocidad de la rueda
  
  /*---ANALOG INPUTS---*/
  pinMode(A0,INPUT);    //Nivel de tension de las baterias
  pinMode(A1, INPUT);   //Nivel de tension del sensor Hall
  
}

void loop() {
  /*---CALCULO DE TIEMPO---*/
  tiempo_previo = tiempo_actual;
  tiempo_actual = millis();
  tiempo = ((tiempo_actual-tiempo_previo)/3600000);      //Calcula el tiempo transcurrido en horas para la energia
  /*---LECTURA DE SENSORES---*/
  tension_divisor = analogRead(A0);
  tension_hall = analogRead(A1);

  /*---CALCULO DE PARAMETROS---*/
  tension = map(tension_divisor, 0,1023,0,60);  //Mapea los niveles de tension recibidos de 0 a 1023, para 0 a 60v
  
  if(tension_hall >= 511.5){
    
    corriente = map(tension_hall, 511.5 ,1023, 0, 50);
    //carga = false;
    
  }else{if(tension_hall < 2.5){
    
      corriente = -(map(tension_hall, 511.5, 1023, 0, 50));
      //carga = true;   
    }
   }

  potencia = (tension * corriente);
  energia += (potencia * tiempo);

  /*---COMPROBACION DE TIEMPO---*/
  if((tiempo_actual - tiempo_previo) >= muestreo){
    SD_guardar(tiempo, tension, corriente, potencia, energia, velocidad, archivo);
    if(doble){
      SD_guardar(tiempo, tension, corriente, potencia, energia, velocidad, archivo);
    }
  }
}

void LCD_datos (float tension,float corriente, float potencia, float energia, float velocidad){

  LiquidCrystal_I2C lcd(0x3F,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
  lcd.init();                         //Inicia el objeto "lcd"
  lcd.backlight();                    //Enciende el backlight
  lcd.setCursor(0,1);                 //Pone el cursor en el primer casillero de la segunda fila
  lcd.print("Hola Mundo");            //Escribe eso donde debería estar el cursor      
  
  }

void LCD_error (String error){

  LiquidCrystal_I2C lcd(0x3F,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
  lcd.init();                         //Inicia el objeto "lcd"
  lcd.backlight();                    //Enciende el backlight
  lcd.clear();                        //Limpia el LCD de todo lo que tenga escrito
  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la primer fila
  lcd.print(error);                   //Escribe el error donde debería estar el cursor   
  return;
  
}

void SD_guardar (float tiempo, float tension,float corriente, float potencia, float energia, float velocidad, String archivo){
  
  File datos;                             //Crea el objeto "datos" para trabajar con la librería
  
  if(!SD.begin(cs)){
    Serial.println("Error conectando con SD");
    LCD_error("Error conectando con SD");
    return;
  }
  
  datos = SD.open(archivo,FILE_WRITE);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                             //Comprueba si puede escribir a la SD, envía error si no fuese así
    Serial.println("Error escribiendo a SD");
    LCD_error("Error escribiendo a SD");
    return;
  }
                                          //Almacena las distintas variables separadas por una coma
  datos.print(tiempo);
  datos.print(",");
  datos.print(tension);
  datos.print(",");
  datos.print(corriente);
  datos.print(",");
  datos.print(potencia);
  datos.print(",");
  datos.print(energia);
  datos.print(",");
  datos.print(velocidad);

  datos.close();                          //Cierra el archivo y vuelve al programa
  return;
}

void blink(){
  doble = !doble;
}
