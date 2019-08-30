#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

#define PI 3.1415926535897932384626433832795

/*---VARIABLES MODULO SD---*/
int cs = 53;					//pin al que se conecta el chip select
int posicion_principal = 0;
int posicion_secundario = 0;
String datos_principal = "dprim_";
String datos_secundario = "dsec_";
String extension = ".txt";

/*---VARIABLES SENSORES---*/
float revoluciones = 0;			//Contador de revoluciones dadas por la rueda del auto
float tension_divisor = 0;		//Valor de tension de entrada del divisor resistivo 
float tension_hall = 0;			//Valor de tension de entrada del sensor hall

/*---VARIABLES PROGRAMA---*/
float tiempo = 0;               //Tiempo transcurrido del programa en horas
float tiempo_inicio = 0;
float tiempo_actual = 0;        //Variable auxiliar para el calculo del tiempo
float tiempo_previo = 0;        //Variable auxiliar para el calculo del tiempo

float tension = 0;              //Nivel de tension de las baterias
float corriente = 0;            //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             //Potencia del motor
float energia = 0;              //Energia consumida por el motor
float velocidad = 0;            //Velocidad tangencial de la rueda (Km/h)
float frecuencia = 0;           //Revoluciones por segundo del eje

float diametro = 0;             //Diametro del eje (mm)
int muestreo = 200;            	//Frecuencia con la que se almacenan los datos en la tarjeta SD (ms)
float tiempo_objetivo = 0;		//Tiempo de funcionamiento necesario
float tension_minima = 0;		//Tension minima a la que puede funcionar el auto

bool doble = false;             //Booleano que comprueba si los datos deberan guardarse en un archivo a parte
bool modo_debug = true;			//Booleano para pasar variables por monitor serie
bool constantes = true;			//Booleano para cargar constantes desde la SD

String error = "";				//Texto a mostrar en display/puerto serie cuando haya un error

/***POSBLES ERRORES***
 * ERROR 1 -> ERROR COMUNICANDO CON EL MODULO SD
 * ERROR 2 -> ERROR ABRIENDO ARCHIVO ESPECIFICADO EN SD
 * ERROR 3 -> ERROR LEYENDO CONSTANTES DESDE ARCHIVO
 * ERROR 4 -> NO MIDE TENSION
 * ERROR 5 -> NO MIDE CORRIENTE
 * ERROR 6 -> NO MIDE VELOCIDAD
*/

/***TO-DO***
 * Corregir mapeo de corriente de acuerdo a los limites del sensor hall utilizado
 * Cambiar configuracion del display al que sera utilizado
 * Agregar variables al display para que se vean
 * Agregar constantes a definir sobre el tiempo que debe funcionar, tension minima
 * Calcular tiempo de vida estimado en funcion de la potencia
 * Calcular potencia optima para completar el tiempo estimado de funcionamiento
 * Retomar lectura de archivos desde la ultima lectura (chequear y preguntar por display / puerto serie)
 * Calcular distanica recorrida de la rueda en funcion de la velocidad y tiempo en python
 * Actualizar numeros de archivos de datos principal y secundario
*/

LiquidCrystal_I2C lcd(0x27,16,2);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

void setup() {

	/*---INICIO DE PROGRAMA---*/
	
  Serial.begin(9600);           	//Inicia una comunicacion serial para edicion de codigo
  tiempo_inicio = millis();     	//Toma el tiempo de inicio del programa en milisegundos
  tiempo_previo = tiempo_inicio;	//Iguala el tiempo a esta variable para controlar el programa
  
  attachInterrupt(digitalPinToInterrupt(2), blink, LOW);
  attachInterrupt(digitalPinToInterrupt(3), blink, LOW);

  /*---CONFIGURACION LCD---*/  
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
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
  tiempo_actual = millis();
  tiempo_transcurrido = ((tiempo_actual-tiempo_inicio)/3600000);     //Calcula el tiempo transcurrido en horas para la energia
  tiempo_objetivo -= tiempo_transcurrido
  
  /*---COMPROBACION DE TIEMPO---*/
  if((tiempo_actual - tiempo_previo) >= muestreo){
  
    tiempo_previo = tiempo_actual;						//Vuelve a tomar el tiempo para comenzar nuevamente
    
    detachInterrupt(digitalPinToInterrupt(2));         	//Elimina temporalmente las interrupciones para poder grabar los datos
    detachInterrupt(digitalPinToInterrupt(3));
    
    calcular();											//Llama a la funcion calcular() para obtener los datos
    
    SD_guardar(datos_principal);

    if(doble){											//Si el boton fue oprimido, se guardaran dos veces las variables
      
      SD_guardar(datos_secundario);
      Serial.println(datos_secundario);
    
    }
    
    if(modo_debug){debug();}							//Funcion para ver los datos en el puerto serie
    
    attachInterrupt(digitalPinToInterrupt(2), blink, LOW);    //Vuelve a habilitar las interrupciones para que funcione el programa
    attachInterrupt(digitalPinToInterrupt(3), blink, LOW);
  }

  if(!constantes){
    SD_constantes();
  }
  
}

void LCD_datos (float tension,float corriente, float potencia, float energia, float velocidad){

  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la segunda fila
  lcd.print("Hola Mundo");            //Escribe eso donde debería estar el cursor      
  
  }

void LCD_error (String error){
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
  datos.println(velocidad);

  datos.close();                          //Cierra el archivo y vuelve al programa
  return;
}

void SD_constantes(){
  
 if(SD.begin(cs)){
	
    File archivo;
	
    archivo = SD.open("const.txt");
	
    if(archivo){
      archivo.seek(0);
      posicion_principal = archivo.read();
      posicion_secundario = archivo.read();
      constantes = true;
      archivo.close();
    }else{
      LCD_error("E2, E3");
    }
  }else{
    LCD_error("E1, E3");
  }
  
  datos_principal.concat(posicion_principal);
  datos_principal.concat(extension);			//Forma el nombre del archivo de datos principal
}

float calcular (){

  /*---LECTURA DE SENSORES---*/
  
  tension_divisor = analogRead(A0);
  tension_hall = analogRead(A1);

  /*---CALCULO DE TENSION, CORRIENTE, POTENCIA Y ENERGIA---*/
  
  tension = map(tension_divisor, 0,1023,0,60);  					//Mapea los niveles de tension recibidos de 0 a 1023, para 0 a 60v
  
  if(tension_hall >= 511.5){
    
    corriente = map(tension_hall, 511.5 ,1023, 0, 50);				//Si la circulacion de corriente es positiva, la mapea asi
    
  }else{if(tension_hall < 2.5){
    
      corriente = -(map(tension_hall, 511.5, 1023, 0, 50));			//Si la circulacion de corriente es contraria (cargando bateria) se expresa negativa
      
    }
   }

  potencia = (tension * corriente);									//Calcula la potencia en funcion de la corriente, y la tension medida en las baterias
  energia += (potencia * tiempo_transcurrido);						//Calcula la energia consumida hasta ese punto en funcion de la potencia y tiempo (watt - hora)

  /*---CALCULO DE VELOCIDAD TANGENCIAL---*/
  
  frecuencia = (revoluciones/((tiempo_actual-tiempo_previo)/1000));	//Calcula la frecuencia (c/s)
  revoluciones = 0;
  velocidad = (((PI*diametro*frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)
  
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
  //Serial.println(datos_secundario);
  Serial.println(error);
}

void blink(){

  if(digitalRead(2) == 0 && doble){			//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	
	doble = false;
	posicion_secundario++;
	datos_secundario = "dsec_";
	
  }else{
  
	if(digitalRead(2) == 0 && !doble){
		datos_secundario.concat(posicion_secundario);
		datos_secundario.concat(extension);
		doble = true;
	
		}else{									//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
			
			revoluciones++;
			
			}
  }
  
}
