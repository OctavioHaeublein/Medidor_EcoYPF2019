#include "Modulo_SD.h"
#include "LCD_Leds.h"
#include "Calculos_Variables.h"
#include "Velocidad_Aceleracion.h"

LCD_Leds LCD_Leds;
Modulo_SD Modulo_SD;
Calculos_Variables Calculos_Variables;
Velocidad_Aceleracion Velocidad_Aceleracion;

const int sensor_inductivo = 2;			//Pin de entrada del sensor inductivo

String datos_principal;					//Variable para almacenar el nombre del archivo principal
String datos_secundario;				//Variable para el nombre del archivo secundario
bool guardar_secundario = false;		//Booleano que indica si se deberan guardar en archivo a parte

const int boton_encoder = 3;			//Pin de entrada del boton del encoder

float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto

float tension = 0;              	    //Nivel de tension de las baterias
float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             	    //Potencia del motor
float energia = 0;              	    //Energia consumida por el motor
float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)

float corriente_objetivo = 0;           //Corriente estimada que debera mantener para alcanzar el timepo objetivo
float soc = 0;							//Estado de carga actual de las baterias (soc_calculado - soc_inicial)

float tiempo_objetivo = 0;
float tiempo_inicio_prueba = 0;
float tiempo_transcurrido = 0;
float tiempo_programa = 0;

char estado_acelerador = 'm';
float nivel_acelerador = 0;

int muestreo = 1000;

void setup() {

  pinMode(sensor_inductivo  , INPUT);	//Señal cuadrada para velocidad de la rueda
  
  LCD_Leds.setup();

  datos_principal, datos_secundario = Modulo_SD.setup();
  
  soc, corriente_objetivo = Calculos_Variables. calcular_capacidad(true);

  attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
  
}

void loop() {														//Loop principal del programa
 /*
	if(true){
		if(truecorriente == 0){Calculos_Variables.calcular_capacidad(true);};				//Puede que nunca lea 0 por temas de ruido
		if(truecorriente != 0){Calculos_Variables.calcular_capacidad(false);}
	}
	
	if(true){
		SD_guardar(datos_principal);                              	//Para guardar los datos en el txt
		if(estado_prueba){										
			SD_guardar(datos_secundario);
		}					
		n_guardado++;
	}
  */

	Velocidad_Aceleracion.acelerador(estado_acelerador, nivel_acelerador);

	if( (millis() - tiempo_programa) >= muestreo ){

		tension, corriente, potencia, energia, soc, corriente_objetivo = Calculos_Variables.calcular();

		if(guardar_secundario){
			Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
			Modulo_SD.SD_guardar(datos_secundario, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
		}else{
			Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
		}
		
		tiempo_programa = millis();

	}

}

void blink(){											//Contador de revoluciones y boton del encoder

  if(digitalRead(boton_encoder) == 0){					//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	guardar_secundario = Modulo_SD.estado_prueba();
  }else{												//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
		revoluciones++;		
		}

}	
