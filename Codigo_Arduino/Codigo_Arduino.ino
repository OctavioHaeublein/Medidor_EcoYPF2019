#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <EEPROM.h>

#define PI 3.1415926535897932384626433832795

/*---VARIABLES MODULO SD---*/
int cs = 53;
int posicion_principal = 0;
int posicion_secundario = 0;
String datos_principal = "datos_principal_";
String datos_secundario = "datos_secundario_";

/*---VARIABLES SENSORES---*/
float revoluciones = 0;
float tension_divisor = 0;
float tension_hall = 0;


/*---VARIABLES PROGRAMA---*/
float tiempo = 0;               //Tiempo transcurrido del programa en horas
float tiempo_inicio = 0;
float tiempo_actual = 0;        //Variable auxiliar para el calculo del tiempo
float tiempo_previo = 0;        //Variable auxiliar para el calculo del tiempo

String archivo = "";            //Nombre del archivo al que deben ser guardados los datos en la tarjeta SD
int muestreo = 1000;            //Frecuencia con la que se almacenan los datos en la tarjeta SD (ms)
bool doble = false;             //Booleano que comprueba si los datos deberan guardarse en un archivo a parte

float tension = 0;              //Nivel de tension de las baterias
float corriente = 0;            //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             //Potencia del motor
float energia = 0;              //Energia consumida por el motor

float frecuencia = 0;           //Revoluciones por segundo del eje
float diametro = 0;             //Diametro del eje (mm)
float velocidad = 0;            //Velocidad tangencial de la rueda (Km/h)

bool modo_debug = true;
bool constantes = true;

/*---ERRORES---*/
String error = "";

/*
 * ERROR 1 -> ERROR COMUNICANDO CON EL MODULO SD
 * ERROR 2 -> ERROR ABRIENDO ARCHIVO ESPECIFICADO EN SD
 * ERROR 3 -> ERROR LEYENDO CONSTANTES DESDE ARCHIVO
*/

void setup() {
  Serial.begin(9600);           //Inicia una comunicacion serial para edicion de codigo
  tiempo_inicio = millis();     //Toma el tiempo de inicio del programa en milisegundos
  attachInterrupt(digitalPinToInterrupt(2), blink, LOW);
  attachInterrupt(digitalPinToInterrupt(3), blink, LOW);
  
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

  /*---LECTURA DE CONSTANTES EN SD---*/
  
  
}

void loop() {
  /*---CALCULO DE TIEMPO---*/
  tiempo_actual = millis();
  tiempo = ((tiempo_actual-tiempo_inicio)/3600000);      //Calcula el tiempo transcurrido en horas para la energia

  /*---COMPROBACION DE TIEMPO---*/
  if((tiempo_actual - tiempo_previo) >= muestreo){
    
    tiempo_previo = tiempo_actual;
    
    detachInterrupt(digitalPinToInterrupt(2));                //Elimina temporalmente las interrupciones para poder grabar los datos
    detachInterrupt(digitalPinToInterrupt(3));
    
    calcular();
    
    datos_principal.concat(posicion_principal);
    if(modo_debug){debug();}
    SD_guardar(datos_principal);
    datos_principal = "datos_principal_";
    posicion_principal++;
    
    if(doble){
      datos_secundario.concat(posicion_secundario);
      SD_guardar(datos_secundario);
      datos_secundario = "datos_secundario_";
      posicion_secundario++;
    }
    
    attachInterrupt(digitalPinToInterrupt(2), blink, LOW);    //Vuelve a habilitar las interrupciones para que funcione el programa
    attachInterrupt(digitalPinToInterrupt(3), blink, LOW);
  }

  if(!constantes){
    SD_constantes();
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

void SD_guardar (String archivo){
  
  File datos;                             //Crea el objeto "datos" para trabajar con la librería
  
  if(!SD.begin(cs)){
    Serial.println("E1");
    LCD_error("E1");
    return;
  }
  
  datos = SD.open(archivo,FILE_WRITE);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                             //Comprueba si puede escribir a la SD, envía error si no fuese así
    Serial.println("E2");
    LCD_error("E2");
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

void SD_constantes(){
  
 if(SD.begin(cs)){
  
    File archivo;
    archivo = SD.open("constantes.txt");
    if(archivo){
      archivo.seek(0);
      posicion_principal = archivo.read();
      posicion_secundario = archivo.read();
      constantes = true;
      archivo.close();
    }else{
      LCD_error("E3");
    }
  }else{
    LCD_error("E3");
  }
  
}
float calcular (){
  /*---LECTURA DE SENSORES---*/
  tension_divisor = analogRead(A0);
  tension_hall = analogRead(A1);

  /*---CALCULO DE TENSION, CORRIENTE, POTENCIA Y ENERGIA---*/
  tension = map(tension_divisor, 0,1023,0,60);  //Mapea los niveles de tension recibidos de 0 a 1023, para 0 a 60v
  
  if(tension_hall >= 511.5){
    
    corriente = map(tension_hall, 511.5 ,1023, 0, 50);
    
  }else{if(tension_hall < 2.5){
    
      corriente = -(map(tension_hall, 511.5, 1023, 0, 50));
      
    }
   }

  potencia = (tension * corriente);
  energia += (potencia * tiempo);

  /*---CALCULO DE VELOCIDAD TANGENCIAL---*/
  frecuencia = (revoluciones/((tiempo_actual-tiempo_previo)/1000));
  revoluciones = 0;
  velocidad = ((PI*diametro*frecuencia)/1000);
  
  return tension, corriente, potencia, energia, velocidad;
}

void debug (){
  Serial.print("Tiempo transcurrido: ");
  Serial.println(tiempo);
  Serial.print("Tension: ");
  Serial.println(tension);
  Serial.print("Corriente: ");
  Serial.println(corriente);
  Serial.print("Potencia: ");
  Serial.println(potencia);
  Serial.print("Energia: ");
  Serial.println(energia);
  Serial.print("Velocidad: ");
  Serial.println(velocidad);
  Serial.println(datos_principal);
  Serial.println(datos_secundario);
  Serial.println(error);
}

void blink(){
  if(digitalRead(2)==0){
    doble = !doble;
  }else{
    revoluciones++;
  }

}
