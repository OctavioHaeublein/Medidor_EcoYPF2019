#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <Encoder.h>

#define PI 3.1415926535897932384626433832795

#define LA1 34
#define LA2 36
#define LA3 38
#define LR1 40
#define LR2 42

#define LV1 43
#define LV2 41
#define LV3 39
#define LV4 37
#define LV5 35
  
#define RGB_R 4
#define RGB_V 5
#define RGB_A 6
#define si    2

#define cs 9

#define sw 3
#define dt 19
#define clk 18

#define enable 8

/*---VARIABLES PID---*/
float pwm_acelerador = 0;
float kp = 0;
float ki = 0;
float kd = 0;
float salida_pid = 0;
float error = 0;
float error_previo = 0;
float tiempo_previo_pid = 0;
float sum_error = 0;
float freq_error = 0;
float limite_pid = 0;

/*---VARIABLES MODULO SD---*/
int posicion_principal = 0;			    //Numero de archivo principal
int posicion_secundario = 0;		    //Numero de archivo secundario
String datos_principal = "DPRIM_";	    //Nombre del archivo principal de adquisición de datos
String datos_secundario = "DSEC_";	    //Nombre del archivo secundario
String extension = ".txt";			    //Extensión del archivo para concatenarlos

/*---VARIABLES ENCODER---*/
int est_encoder = 0;
int ultimo_est_encoder = 0;
Encoder encoder (dt,clk);

/*---VARIABLES SENSORES---*/
float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto
float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
float tension_hall = 0; 				//Valor de tension de entrada del sensor hall

/*---VARIABLES PROGRAMA---*/
float tiempo_transcurrido = 0;          //Tiempo transcurrido del programa en horas
float tiempo_previo = 0;        	    //Variable auxiliar para el calculo del tiempo
float tiempo_programa = 0;              //Tiempo desde el ultimo calculo de variables
float tiempo_objetivo = 0;			    //Tiempo de funcionamiento necesario
float tiempo_energia = 0;
float tiempo_inicio = 0;

float tension = 0;              	    //Nivel de tension de las baterias
float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
float sum_potencia = 0;					//Sumatoria de las potencias a ser multiplicado por el tiempo_transcurrido
float potencia = 0;             	    //Potencia del motor
float energia = 0;              	    //Energia consumida por el motor
float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)
float frecuencia = 0;           	    //Revoluciones por segundo del eje
float corriente_objetivo = 0;           //Corriente estimada que debera mantener para alcanzar el timepo objetivo

float SOC_inicial = 1;					//Estado de carga inicial de las baterias 	(Medido con tension en vacio)
float SOC_calculado = 0;				//Estado de bateria calculado por suma de corrientes
float SOC = 0;							//Estado de carga actual de las baterias (SOC_calculado - SOC_inicial)
float sum_corriente = 0;				//Variable para la sumatoria de corriente en el tiempo
float contador_SOC = 0;					//Contador para el calculo del SOC 
float n_guardado = 0;					//Contador de cantidad de veces guardado para la energia

float verde = 0;
float rojo = 0;
float azul = 0;

float diametro = 0;             	    //Diametro del eje (mm)
float muestreo = 1000;            		//Frecuencia con la que se almacenan los datos en la tarjeta SD (ms)

float tension_minima = 48;		    	//Tension minima a la que puede funcionar el auto
float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)

float entrada_acelerador = 0;
float salida_acelerador = 0;
float nivel_acelerador = 0;
char estado_acelerador = 'm';

bool doble = false;             	    //Booleano que comprueba si los datos deberan guardarse en un archivo a parte
bool modo_debug = true;				    //Booleano para pasar variables por monitor serie
bool constantes = false;				//Booleano para cargar constantes desde la SD

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
 * Calcular distanica recorrida de la rueda en funcion de la velocidad y tiempo en python
*/

LiquidCrystal_I2C lcd(0x27,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

void setup() {

  /*---INICIO DE PROGRAMA---*/
  Serial.begin(9600);          		//Inicia una comunicacion serial para edicion de codigo
  
  /*---CONFIGURACION LCD---*/  
  lcd.init();						//Inicio de la funcion lcd
  lcd.clear();						//Pone en blanco el lcd
  lcd.backlight();					//Enciende el backlight del display
  
  /*---DIGITAL OUTPUTS---*/
  pinMode(LR1 , OUTPUT);   			//LED Rojo1
  pinMode(LR2 , OUTPUT);   			//LED Rojo
  pinMode(LA1 , OUTPUT);   			//LED Amarillo
  pinMode(LA2 , OUTPUT);   			//LED Amarillo1
  pinMode(LA3 , OUTPUT);   			//LED Amarillo2
  pinMode(LV1 , OUTPUT);   			//LED Verde
  pinMode(LV2 , OUTPUT);   			//LED Verde1
  pinMode(LV3 , OUTPUT);   			//LED Verde2
  pinMode(LV4 , OUTPUT);   			//LED Verde3
  pinMode(LV5 , OUTPUT);   			//LED Verde4
  
  /*---DIGITAL PWM OUTPUTS---*/
  pinMode(RGB_R , OUTPUT);    		//RGB Rojo
  pinMode(RGB_V , OUTPUT);    		//RGB Verde
  pinMode(RGB_A , OUTPUT);    		//RGB Azul
  
  /*---DIGITAL INPUTS---*/
  pinMode(si  , INPUT);    			//Señal cuadrada para velocidad de la rueda
  pinMode(sw  , INPUT_PULLUP);		//Señal de entrada del boton
  pinMode(dt  , INPUT);				//Señal de entrada 'A' del encoder
  pinMode(clk , INPUT);				//Señal de entrada 'B' del encoder
  
  /*---ANALOG INPUTS---*/
  pinMode(A0,INPUT);    			//Nivel de tension de las baterias
  pinMode(A1, INPUT);   			//Nivel de tension del sensor Hall
    
  if(!constantes){
    SD_constantes();
  }
  
  //attachInterrupt(digitalPinToInterrupt(sw), blink, FALLING);
  //attachInterrupt(digitalPinToInterrupt(si), blink, FALLING);
  
}

void loop() {														//Loop principal del programa
  
  acelerador();														//Funcion que controla el acelerador de puño del auto

  if( (tiempo_programa >= muestreo)){
	  
    //detachInterrupt(digitalPinToInterrupt(sw));         	      	//Elimina temporalmente las interrupciones para poder grabar los datos
    //detachInterrupt(digitalPinToInterrupt(si));
  
	tiempo();                                                   	//Actualiza los valores de tiempo_programa, tiempo_transcurrido, y tiempo_objetivo
	calcular();											      		//Llama a la funcion calcular() para obtener los datos
	leds();                           								//Llama a la funcion leds() para encender la cantidad de leds necesarios y controlar el rgb
    
	contador_SOC++;
	
	if(contador_SOC >= 60){
		if(corriente == 0){calcular_capacidad(true);};				//Puede que nunca lea 0 por temas de ruido
		if(corriente != 0){calcular_capacidad(false);}
	}
	
	if( (velocidad != 0) || (corriente != 0) ){
		SD_guardar(datos_principal);                              	//Para guardar los datos en el txt
		if(doble){SD_guardar(datos_secundario);}					//Si el boton fue oprimido, se guardaran dos veces las variables
		n_guardado++;
	}
	
    //attachInterrupt(digitalPinToInterrupt(sw), blink, FALLING);    	//Vuelve a habilitar las interrupciones para que funcione el programa
    //attachInterrupt(digitalPinToInterrupt(si), blink, FALLING);
  
  }
  
}

void tiempo(){							//Tiempo_programa, Tiemppo_Transcurrido, y Tiempo_objetivo

  tiempo_programa = (millis() - tiempo_previo);         				//Tiempo desde el ultimo muestreo para el programa
  tiempo_transcurrido = ((millis() - tiempo_inicio)/60000);             //Tiempo transcurrido del programa en horas para la energia
  tiempo_energia = (n_guardado * muestreo);

  if( (tiempo_objetivo - tiempo_transcurrido) > 0 ){    				//Vuelve a calcular el valor del tiempo objetivo en funcion del transcurrido
  tiempo_objetivo -= tiempo_transcurrido;
  }else{
    tiempo_objetivo = 0;
	prueba();
  }
  
  tiempo_previo = millis();					              		//Vuelve a tomar el tiempo para comenzar nuevamente
}

void LCD_datos (){						//Layout principal para el display LCD

  lcd.setCursor(0,0);                 	//Pone el cursor en el primer casillero de la segunda fila
  lcd.print("Hola Mundo");            	//Escribe eso donde debería estar el cursor      
  
  }

void LCD_display (String palabra){		//Errores y otras cosas para mostrar en el display
  lcd.clear();                        	//Limpia el LCD de todo lo que tenga escrito
  lcd.setCursor(0,0);                 	//Pone el cursor en el primer casillero de la primer fila
  lcd.print(palabra);                   //Escribe el error donde debería estar el cursor
  Serial.println(palabra);   
  return;
}

void SD_constantes(){					//Al inicio del programa busca datos anteriores para cargar

  File datos;                           //Crea el objeto "datos" para trabajar con la librería
  LCD_display("Cargando...");			//Intenta comunicarse con el modulo SD
  if(!SD.begin(cs)){
    LCD_display("E3, E1");				//Devuelve un error de no conectarse
    return;
  }
  
  for (int i = 0; i<= 20; i++){			//Comienza a buscar el ultimo archivo principal de datos
    datos_principal.concat(i);
    datos_principal.concat(extension);
	
    if( (!SD.exists(datos_principal)) && (i > 0) ){
      datos_principal = "DPRIM_";
	  datos_principal.concat(i-1);
      datos_principal.concat(extension);
	  if( SD.exists(datos_principal) ){
		LCD_display(datos_principal);
		posicion_principal = i;
	    delay(1000);
        break;
	  }else{
		  datos_principal = "DPRIM_";
	    }
    }else{
      datos_principal = "DPRIM_";
    }
  }
  
  for (int i = 0; i<= 20; i++){			//Comienza a buscar el ultimo archivo secundario de datos
    datos_secundario.concat(i);
    datos_secundario.concat(extension);
    
    if( (!SD.exists(datos_principal)) && (i > 0) ){
      datos_secundario = "DPRIM_";
	  datos_secundario.concat(i-1);
      datos_secundario.concat(extension);
	  if( SD.exists(datos_secundario) ){
		LCD_display(datos_secundario);
		posicion_secundario = i;
	    delay(1000);
        break;
	  }else{
		  datos_principal = "DSEC_";
	    }
    }else{
      datos_principal = "DSEC_";
    }
  }
  
  if(datos_secundario == "DSEC_"){
    datos_secundario = "DSEC_0.txt";	//Si no encuentra un archivo secundario, lo nombra como el primero
  }
  
  
  
  if(datos_principal == "DPRIM_"){		//Si no encuentra un archivo principal de datos, se nombra como el primero
    datos_principal = "DPRIM_0.txt";
    constantes = true;
    return;
  }else{								//De encontrarse un archivo, intenta cargar la ultima linea de datos
	  
	bool cargar = false;
    String decision = "NO";
  
    while(true){
	lcd.clear();          
	lcd.setCursor(0,0);             
	lcd.print("Desea cargar?");                  
	lcd.setCursor(0,1);
	lcd.print(decision);
	est_encoder = encoder.read();
	if(est_encoder != ultimo_est_encoder){
		if(cargar){decision = "NO";}
		if(!cargar){decision = "SI";}
		cargar = !cargar;
	}
	ultimo_est_encoder = est_encoder;
	if(digitalRead(sw) == 0){
		if(cargar){
			LCD_display("Cargando datos...");
			break;
			}
		if(!cargar){return;}
	}
	delay(150);
	}
  }

  datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
    LCD_display("E3, E2");
    return;
  }
  
  int posicion = 0;
  
  for(int i = 1; i <= datos.size() ; i++){       //Busca por la ultima linea para poder leer exclusivamente esa
    datos.seek(datos.size()-i);
    if(datos.read() == 10){                     //PUEDE SER QUE HAYA QUE CAMBIAR A i = 1 POR EL TEMA DE NUEVA LINEA
      posicion = (datos.position()+1);          //Devuelve la posicion del primer caracter de la ultima linea
      break;
    }
  }
  
  datos.close();
  
  datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
  
  if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
    LCD_display("E3, E2");
    return;
  }
  
  datos.seek(posicion);                          //Se desplaza hasta la posicion obtenida anteriormente
  
  if(datos.available()){                         //Comienza a leer la ultima fila para obtener los datos guardados
    tiempo_transcurrido = datos.read();
    tension = datos.read();
    corriente = datos.read();
    potencia = datos.read();
    energia = datos.read();
    velocidad = datos.read();
    corriente_objetivo = datos.read();
    constantes = true;
  }else{
    LCD_display("E3");
    datos.close();
    return;
  }
  
  datos.close();
  LCD_display("Cargado con exito!");
  return;
}

void SD_guardar (String archivo){		//Guardar datos en archivo txt
  
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
  datos.println(corriente_objetivo);
  
  datos.close();                          //Cierra el archivo y vuelve al programa
  return;
}

float calcular (){						//Funcion para calculos principal del programa	

  /*---LECTURA DE SENSORES---*/
  for(int i = 0; i < 5; i++){					//Se realizan 5 lecturas seguidas y se promedia
  tension_divisor += analogRead(A0);
  tension_hall    += analogRead(A1);
  }
  tension_divisor = (tension_divisor / 5);
  tension_hall    = (tension_hall / 5);
  
  /*---CALCULO DE TENSION, CORRIENTE, POTENCIA Y ENERGIA---*/
  
  tension = map(tension_divisor, 744.2748,930.3428,48,60);  		   //Mapea los niveles de tension recibidos de 0 a 1023, para 0 a 60v
  
  if(tension_hall >= 511.5){
    
    corriente = map(tension_hall, 511.5 ,1023, 0, 50);				   //Si la circulacion de corriente es positiva, la mapea asi
    
  }else{if(tension_hall < 2.5){
    
      corriente = -(map(tension_hall, 511.5, 1023, 0, 50));			   //Si la circulacion de corriente es contraria (cargando bateria) se expresa negativa
      
	}
   }

  sum_corriente +=  corriente;
  
  potencia       = (tension * corriente);								//Calcula la potencia en funcion de la corriente, y la tension medida en las baterias
  sum_potencia  +=  potencia;											//Calcula la energia consumida hasta ese punto en funcion de la potencia y tiempo (watt - hora)
  energia        = (sum_potencia * tiempo_energia);
  
  frecuencia     = (revoluciones/(tiempo_programa/1000));				//Calcula la frecuencia (c/s)
  revoluciones   =  0;
  velocidad      = (((PI*diametro*frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)
  
}

void calcular_capacidad(bool tabla){	//Calculo de capacidad restante de  la bateria y calculo por tabla
	
	if(tabla){
		SOC_inicial = map(tension,46.04,50.92,0.1,1);		
	}
	
	SOC_calculado = (-1/capacidad_baterias) * (muestreo * contador_SOC * sum_corriente);
	
	SOC = SOC_inicial - SOC_calculado;
	
	contador_SOC = 0;
	sum_corriente = 0;
	SOC_inicial = SOC;
	
	SOC = (SOC * capacidad_baterias);
	
	corriente_objetivo = (SOC / (tiempo_objetivo/60));			//Divide la capacidad restante de la bateria (Ah) por el tiempo restante pasado a horas, para determinar la corriente para desarrollar
}

void acelerador(){
	
	entrada_acelerador = analogRead(A0);
	entrada_acelerador = map(entrada_acelerador,179,880,70,190);
	
	switch(estado_acelerador){
		
		case 'm':
			salida_acelerador = entrada_acelerador;
			break;
			
		case 'a':
			calculo_pid();
			salida_acelerador = ((limite_pid * 120)/100);
			salida_acelerador += 70;
			if( entrada_acelerador < salida_acelerador){
				salida_acelerador = entrada_acelerador;
			}
		break;
		
		case 'c':
			salida_acelerador = ((nivel_acelerador * 120)/100);
			salida_acelerador += 70;
			if( entrada_acelerador < salida_acelerador){
			salida_acelerador = entrada_acelerador;
			}
		break;
	}
	
	analogWrite(pwm_acelerador, salida_acelerador);
	
}

void calculo_pid(){
	
	float tiempo = millis();
	
	tiempo_transcurrido = (tiempo - tiempo_previo_pid);
	
	error = corriente_objetivo - corriente;
	
	sum_error += (error * tiempo_transcurrido);
	
	freq_error = ((error - error_previo)/tiempo_transcurrido);
	
	salida_pid = (kp * error) + (ki * sum_error) + (kd * freq_error);
	
	error_previo = error;
	tiempo_previo_pid = tiempo;
	
	if(salida_pid < 0){
		limite_pid = 0;
	}else{
		limite_pid = map(salida_pid,0,100,0,100);
	}
	
}

void leds(){							//Control de tira de LEDS y RGB 

  int cantidad_leds = map(SOC,0.1,1,0,9);
  
  int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};
  
  for(int i = 0; i<=9; i++){
    digitalWrite(leds[i],LOW);
  }
  for(int i = 0; i <= cantidad_leds; i++){
    digitalWrite(leds[i],HIGH);
  }
  
  float diferencia = (corriente_objetivo - corriente);
  
  if(diferencia >= 0){
	verde = map(diferencia,0,5,255,0);     	//Mapea la diferencia de potencia positiva (por debajo del objetivo)
	azul = 0;								  	//Deja el color restante en 0 para que no se muestre
	rojo = (255 - verde);                     	//Hace la diferencia entre el nivel de tension que deberia tener el led verde, y el restante se va al rojo
  }else{
	azul = map(diferencia, -5, 0, 255, 0);  	//Mapea la diferencia de potencia negativa (por arriba del objetivo)
	verde = 0;									//Deja el color restante en 0 para que no se muestre
	rojo = (255 - azul);                     	//Hace la diferencia entre el nivel de tension que deberia tener el led verde, y el restante se va al rojo
  }
  
  analogWrite(RGB_R , rojo);                         //AnalogWrite correspondiente a los niveles de tension
  analogWrite(RGB_V , verde);
  analogWrite(RGB_A , azul);
}

void prueba (){							//Configuracion de tiempo de la prueba, y cambio de archivos
	if(doble){
	doble = false;
	posicion_secundario++;
    datos_secundario = "DSEC_";
    datos_secundario.concat(posicion_secundario);
    datos_secundario.concat(extension);	
	}
	if(!doble){
		doble = true;
		while(true){
			est_encoder = encoder.read();
			LCD_display("Tiempo (min): " + est_encoder);
			delay(150);
			if( digitalRead(sw) == 0 ){
				tiempo_objetivo = est_encoder;
        tiempo_inicio = millis();
				break;
			}
		}
		
	}
}

void blink(){							//Contador de revoluciones y boton del encoder

  if(digitalRead(sw) == 0){				//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	prueba();
	
  }else{								//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
		revoluciones++;		
		}
	}	
