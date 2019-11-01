#include "Calculos_Variables.h"
#include "Modulo_SD.h"
#include "LCD_Leds.h"
#include "Velocidad_Aceleracion.h"

#include <Wire.h>
#include <SPI.h>
#include <Encoder.h>

#define si    2

#define sw 3
#define dt 19
#define clk 18

#define enable 8

/*---VARIABLES ENCODER---*/
int est_encoder = 0;
int ultimo_est_encoder = 0;
Encoder encoder (dt,clk);

/*---VARIABLES SENSORES---*/
float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto

/*---VARIABLES PROGRAMA---*/
float tiempo_transcurrido = 0;          //Tiempo transcurrido del programa en horas
float tiempo_previo = 0;        	    //Variable auxiliar para el calculo del tiempo
float tiempo_programa = 0;              //Tiempo desde el ultimo calculo de variables
float tiempo_objetivo = 0;			    //Tiempo de funcionamiento necesario
float tiempo_energia = 0;
float tiempo_inicio = 0;

float muestreo = 1000;            		//Frecuencia con la que se almacenan los datos en la tarjeta SD (ms)

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
 * Agregar variables al display para que se vean
 * Calcular distanica recorrida de la rueda en funcion de la velocidad y tiempo en python
*/



void setup() {

  /*---DIGITAL INPUTS---*/
  pinMode(si  , INPUT);    			//Señal cuadrada para velocidad de la rueda
  pinMode(sw  , INPUT_PULLUP);		//Señal de entrada del boton
  pinMode(dt  , INPUT);				//Señal de entrada 'A' del encoder
  pinMode(clk , INPUT);				//Señal de entrada 'B' del encoder
      
  if(!constantes){
    SD_constantes();
  }
  
  //attachInterrupt(digitalPinToInterrupt(sw), blink, FALLING);
  //attachInterrupt(digitalPinToInterrupt(si), blink, FALLING);
  
}

void loop() {														//Loop principal del programa
  
  if( (tiempo_programa >= muestreo)){
	  
    //detachInterrupt(digitalPinToInterrupt(sw));         	      	//Elimina temporalmente las interrupciones para poder grabar los datos
    //detachInterrupt(digitalPinToInterrupt(si));
  
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
