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

String error = "";

float minimo = 200;
float tiempo_interrupt = 0;

void setup() {

  pinMode(sensor_inductivo  , INPUT);											//Señal cuadrada para velocidad de la rueda
  pinMode(boton_encoder, INPUT_PULLUP);
  Serial.begin(9600);

  LCD_Leds.setup();																//Declaracion inicial de salidas y entradas para el header

  Calculos_Variables.setup();													//Declaracion inicial de salidas y entradas para el header

  //Velocidad_Aceleracion.setup();												//Declaracion inicial de salidas y entradas para el header

  datos_principal, datos_secundario, error = Modulo_SD.setup();						//Declaracion inicial de salidas y entradas para el header

  attachInterrupt(digitalPinToInterrupt(boton_encoder), encoder, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
  //attachInterrupt(digitalPinToInterrupt(sensor_inductivo), revolucion, FALLING);		//Establecimiento de la interrupcion del sensor inductivo para la velocidad
  
}

void loop() {

	//Velocidad_Aceleracion.acelerador(estado_acelerador, nivel_acelerador);

	tiempo_transcurrido = (millis() - tiempo_programa);

	if( tiempo_transcurrido >= muestreo ){

		detachInterrupt(digitalPinToInterrupt(boton_encoder));
		//detachInterrupt(digitalPinToInterrupt(sensor_inductivo));

		tension, corriente, potencia, energia, soc, corriente_objetivo, tiempo_objetivo = Calculos_Variables. calcular(tiempo_transcurrido, tiempo_objetivo);
		velocidad = Velocidad_Aceleracion.calcular_velocidad(revoluciones, tiempo_transcurrido);
		LCD_Leds.datos(tension, corriente, potencia, energia, soc, corriente_objetivo, velocidad, error, tiempo_objetivo);

		revoluciones = 0;

		if( (corriente != 0) || (velocidad != 0) ){
			if(guardar_secundario){
				error = Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
				error = Modulo_SD.SD_guardar(datos_secundario, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
			}else{
				error = Modulo_SD.SD_guardar(datos_principal, tiempo_transcurrido, tension, corriente, potencia, energia, velocidad, corriente_objetivo);
			}
		}
		
		tiempo_programa = millis();

		attachInterrupt(digitalPinToInterrupt(boton_encoder), encoder, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
  		//attachInterrupt(digitalPinToInterrupt(sensor_inductivo), revolucion, FALLING);		//Establecimiento de la interrupcion del sensor inductivo para la velocidad

	}

}

void encoder(){

	if(millis() - tiempo_interrupt > minimo){
		guardar_secundario = Modulo_SD.estado_prueba();
		Serial.println(guardar_secundario);
		if(guardar_secundario){
			tiempo_inicio_prueba, tiempo_objetivo = LCD_Leds.comenzar_prueba();
		}
	}	

	tiempo_interrupt = millis();
}

void revolucion(){

	if(millis() - tiempo_interrupt > minimo){
		revoluciones++;
	}
}