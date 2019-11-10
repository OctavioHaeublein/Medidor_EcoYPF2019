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

float tiempo_objetivo = 0;				//Tiempo establecido por el piloto para la competencia
float tiempo_inicio_prueba = 0;			//Tiempo en el que se inicia el contador para el tiempo objetivo
float tiempo_transcurrido = 0;			//Tiempo transcurrido entre mediciones para el calculo de energia y sum_corriente
float tiempo_programa = 0;				//Tiempo para seguir la ultima vez que se llamo al metodo calcular	

char estado_acelerador = 'M';			//Estado del acelerador ('M'anual, 'A'utomatico, 'C'rucero)
float nivel_acelerador = 0;				//Nivel limite del acelerador (Entre 0 y 1)

int muestreo = 1000;					//Tiempo de intervalo entre medidas

void setup() {

  pinMode(sensor_inductivo  , INPUT);	//Señal cuadrada para velocidad de la rueda
  
  LCD_Leds.setup();

  datos_principal, datos_secundario = Modulo_SD.setup();
  
  soc, corriente_objetivo = Calculos_Variables. calcular_capacidad(true);

  attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
  
}

void loop() {														//Loop principal del programa

	Velocidad_Aceleracion.acelerador(estado_acelerador, nivel_acelerador);

	tiempo_transcurrido = (millis() - tiempo_programa);

	if( tiempo_transcurrido >= muestreo ){

		tension, corriente, potencia, energia, soc, corriente_objetivo = Calculos_Variables. calcular(tiempo_transcurrido, tiempo_objetivo);

		if( (corriente != 0) || (velocidad != 0) ){
			if(guardar_secundario){
				Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
				Modulo_SD.SD_guardar(datos_secundario, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
			}else{
				Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
			}
		}
		
		tiempo_programa = millis();

	}

}

void blink(){											//Contador de revoluciones y boton del encoder

  if(digitalRead(boton_encoder) == 0){					//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	guardar_secundario, tiempo_objetivo, tiempo_inicio_prueba = Modulo_SD.estado_prueba();
  }else{												//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
		revoluciones++;		
		}

}	
