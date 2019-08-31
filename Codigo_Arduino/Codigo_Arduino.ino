#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

#define PI 3.1415926535897932384626433832795

/*---VARIABLES MODULO SD---*/
int cs = 53;						    //pin al que se conecta el chip select
int posicion_principal = 0;			    //Numero de archivo principal
int posicion_secundario = 0;		    //Numero de archivo secundario
String datos_principal = "dprim_";	    //Nombre del archivo principal de adquisición de datos
String datos_secundario = "dsec_";	    //Nombre del archivo secundario
String datos_constantes = "const.txt"   //Nombre del archivo con las constantes asignadas
String extension = ".txt";			    //Extensión del archivo para concatenarlos

/*---VARIABLES SENSORES---*/
float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto
float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
float tension_hall = 0; 				//Valor de tension de entrada del sensor hall

/*---VARIABLES PROGRAMA---*/
float tiempo_transcurrido = 0;          //Tiempo transcurrido del programa en horas
float tiempo_inicio = 0;
float tiempo_actual = 0;        	    //Variable auxiliar para el calculo del tiempo
float tiempo_previo = 0;        	    //Variable auxiliar para el calculo del tiempo
float tiempo_programa = 0;

float tension = 0;              	    //Nivel de tension de las baterias
float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             	    //Potencia del motor
float energia = 0;              	    //Energia consumida por el motor
float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)
float frecuencia = 0;           	    //Revoluciones por segundo del eje
float potencia_objetivo = 0;            //Potencia calculada que debera mantener para alcanzar el timepo objetivo

float diametro = 0;             	    //Diametro del eje (mm)
int muestreo = 200;            		    //Frecuencia con la que se almacenan los datos en la tarjeta SD (ms)
float tiempo_objetivo = 0;			    //Tiempo de funcionamiento necesario
float tension_minima = 41.6;		    //Tension minima a la que puede funcionar el auto
float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)

bool doble = false;             	    //Booleano que comprueba si los datos deberan guardarse en un archivo a parte
bool modo_debug = true;				    //Booleano para pasar variables por monitor serie
bool constantes = true;				    //Booleano para cargar constantes desde la SD

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
 * Calcular tiempo de vida estimado en funcion de la potencia
 * Calcular potencia optima para completar el tiempo estimado de funcionamiento
 * Calcular distanica recorrida de la rueda en funcion de la velocidad y tiempo en python
*/

LiquidCrystal_I2C lcd(0x27,16,2);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

void setup() {

  /*---INICIO DE PROGRAMA---*/
	
  Serial.begin(9600);           	//Inicia una comunicacion serial para edicion de codigo
  tiempo_inicio = millis();     	//Toma el tiempo de inicio del programa en milisegundos
  tiempo_previo = tiempo_inicio;	//Iguala el tiempo a esta variable para controlar el programa
  
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
  
  if(!constantes){
    SD_constantes();
  }
  
  attachInterrupt(digitalPinToInterrupt(2), blink, LOW);
  attachInterrupt(digitalPinToInterrupt(3), blink, LOW);
  
}

void loop() {
  
  /*---CALCULO DE TIEMPO---*/
  
  tiempo_actual = millis();
  tiempo_programa = (tiempo_actual - tiempo_previo);
  tiempo_transcurrido = ((tiempo_actual-tiempo_inicio)/3600000);     //Calcula el tiempo transcurrido en horas para la energia
  
  if(tiempo_objetivo > 0){
  tiempo_objetivo -= tiempo_transcurrido;
  }else{
    tiempo_objetivo = 0;
  }
  
  /*---COMPROBACION DE TIEMPO---*/
  
  if( tiempo_programa >= muestreo ){
  
    tiempo_previo = tiempo_actual;						      //Vuelve a tomar el tiempo para comenzar nuevamente
    
    detachInterrupt(digitalPinToInterrupt(2));         	      //Elimina temporalmente las interrupciones para poder grabar los datos
    detachInterrupt(digitalPinToInterrupt(3));
    
    calcular();											      //Llama a la funcion calcular() para obtener los datos
    leds(tension, potencia_objetivo);                         //Llama a la funcion leds() para encender la cantidad de leds necesarios y controlar el rgb
    SD_guardar(datos_principal);                              //Para guardar los datos en el txt

    if(doble){											      //Si el boton fue oprimido, se guardaran dos veces las variables
      
      SD_guardar(datos_secundario);
    
    }
    
    if(modo_debug){debug();}							      //Funcion para ver los datos en el puerto serie
    
    attachInterrupt(digitalPinToInterrupt(2), blink, LOW);    //Vuelve a habilitar las interrupciones para que funcione el programa
    attachInterrupt(digitalPinToInterrupt(3), blink, LOW);
  }

  
  
}

void LCD_datos (float tension,float corriente, float potencia, float energia, float velocidad){

  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la segunda fila
  lcd.print("Hola Mundo");            //Escribe eso donde debería estar el cursor      
  
  }

void LCD_display (String palabra){
  lcd.clear();                        //Limpia el LCD de todo lo que tenga escrito
  lcd.setCursor(0,0);                 //Pone el cursor en el primer casillero de la primer fila
  lcd.print(palabra);                   //Escribe el error donde debería estar el cursor
  Serial.println(palabra);   
  return;
}

void SD_guardar (String archivo){
  
  File datos;                             //Crea el objeto "datos" para trabajar con la librería
  
  if(!SD.begin(cs)){
    LCD_display("E1");
    return;
  }
  
  datos = SD.open(archivo,FILE_WRITE);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                             //Comprueba si puede escribir a la SD, envía error si no fuese así
    LCD_display("E2");
    return;
  }
  
  if(archivo == datos_constantes){
    datos.seek(0);
    datos.print(posicion_principal);
    datos.print(",");
    datos.println(posicion_secundario);
  }
                                          //Almacena las distintas variables separadas por una coma
  datos.print(tiempo_transcurrido);
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
  datos.print(",");
  datos.println(potencia_objetivo);
  
  datos.close();                          //Cierra el archivo y vuelve al programa
  return;
}

void SD_constantes_VIEJO(){
  
 if(SD.begin(cs)){
	
    File datos;
	
    datos = SD.open(datos_constantes);
	
    if(archivo){
      datos.seek(0);
      posicion_principal = datos.read();
      posicion_secundario = datos.read();
      tiempo_transcurrido = datos.read();
      tension = datos.read();
      corriente = datos.read();
      potencia = datos.read();
      energia = datos.read();
      velocidad = datos.read();
      
      datos.close();
      
      if((tiempo_transcurrido != 0) && (tension != 0) && (corriente != 0) && (potencia != 0) && (energia != 0)){
        Serial.println("Se han encontrado datos almacenados previamente");
        Serial.println("Desea cargarlos? (Y/N)");
        while(Serial.available()==0){}
        char respuesta = Serial.read();
        if(respuesta == 'n'){
          float tiempo_transcurrido = 0;
          float tension = 0;              	
          float corriente = 0;            
          float potencia = 0;             
          float energia = 0;             
          float velocidad = 0;            
        }else{SD_guardar(datos_principal);}
      }
      
      constantes = true;
      
    }else{
      LCD_display("E2, E3");
    }
  }else{
    LCD_display("E1, E3");
  }
  
  datos_principal.concat(posicion_principal);
  datos_principal.concat(extension);			//Forma el nombre del archivo de datos principal
  posicion_principal++;                         //Suma 1 a la posicion principal para seguir el conteo
  
  return;
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

void leds(float tension, float potencia_objetivo){

  int cantidad_leds = map(tension,tension_minima,50,0,9);
  
  int leds[] = {23,25,27,29,31,33,35,37,39,41};
  
  for(int i = 0; i<=9; i++){
    digitalWrite(leds[i],LOW);
  }
  for(int i = 0; i <= cantidad_leds; i++){
    digitalWrite(leds[i],HIGH);
  }
  
  float diferencia = (potencia_objetivo - potencia);
  int verde = map(diferencia,0,200,255,0);      //Mapea la diferencia de potencia en funcion del nivel de tension aplicable al PWM
  int rojo = (255 - verde);                     //Hace la diferencia entre el nivel de tension que deberia tener el led verde, y el restante se va al rojo
  analogWrite(4, rojo);                         //AnalogWrite correspondiente a los niveles de tension
  analogWrite(5, verde);
  analogWrite(6, 0);
}

void debug (){
  Serial.print("Tiempo transcurrido: ");
  Serial.println(tiempo_transcurrido);
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
}

void blink(){

  if( (digitalRead(2) == 0 ) && (doble)){			//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	
	doble = false;
	posicion_secundario++;
    datos_secundario = "dsec_";
    datos_secundario.concat(posicion_secundario);
    datos_secundario.concat(extension);
	
  }else{if( (digitalRead(2) == 0 ) && (!doble)){
  
          doble = true;
          
		}else{									//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
			revoluciones++;		
			}
  }
}

void SD_constantes(){

  File datos;                             //Crea el objeto "datos" para trabajar con la librería
  
  if(!SD.begin(cs)){
    LCD_display("E1");
    return;
  }
  
  for (int i = 0; i<= 20; i++){
    datos_principal.concat(i);
    if(SD.exists(datos_principal)){
      posicion_principal = (i+1);
      break;
    }else{
    datos_principal = "dprim_";
    }
  }
  for (int i = 0; i<= 20; i++){
    datos_secundario.concat(i);
    if(SD.exists(datos_secundario)){
      datos_secundario = "dsec_";
      posicion_secundario = (i+1);
      datos_secundario.concat(posicion_secundario);
      datos_secundario.concat(extension);
      break;
    }else{
    datos_secundario = "dsec_";
    }
  }
  
  if(datos_secundario == "dsec_"){
    datos_secundario = "dsec_0";
  }
  
  if(datos_principal == "dprim_"){
    datos_principal = "dprim_0";
    constantes = true;
    return;
  }else{
    tiempo_previo = millis();
    
    while((digitalRead(2) != 0) && (tiempo_programa <= 3000)){
    
      tiempo_programa = (tiempo_actual-tiempo_previo);
      LCD_display("Cargar datos?" + String(3 - (tiempo_programa/1000)));
      
      if((tiempo_programa >= 2900) && (digitalRead(2) != 0)){
        constantes = true;
        datos_principal = "dprim_";
        datos_principal.concat(posicion_principal);
        datos_principal.concat(extension);
        return;
      }
    }
  }

  datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
    LCD_display("E2");
    return;
  }
  int posicion = 0;
  
  for(int i = 0; i <= datos.size() ; i++){       //Busca por la ultima linea para poder leer exclusivamente esa
    datos.seek(datos.size()-i);
    if(datos.read() == 10){
      posicion = (datos.position()+1);          //Devuelve la posicion del primer caracter de la ultima linea
      break;
    }
  }
  
  datos.close();
  
  datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
    LCD_display("E2");
    return;
  }
  
  datos.seek(posicion);
  
  if(datos.available()){
    tiempo_transcurrido = datos.read();
    tension = datos.read();
    corriente = datos.read();
    potencia = datos.read();
    energia = datos.read();
    velocidad = datos.read();
    potencia_objetivo = datos.read();
    constantes = true;
  }else{
    LCD_display("E3");
    datos.close();
    return;
  }
  
  datos.close();
  return;
}