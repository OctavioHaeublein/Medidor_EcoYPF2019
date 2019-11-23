#include "Modulo_SD.h"
#include "LCD_Leds.h"
#include "Calculos_Variables.h"
#include "Velocidad_Aceleracion.h"

LCD_Leds LCD_Leds;
Modulo_SD Modulo_SD;
Calculos_Variables Calculos_Variables;
Velocidad_Aceleracion Velocidad_Aceleracion;

const int sensor_inductivo = 2;			//Pin de entrada del sensor inductivo
const int boton_encoder = 3;			//Pin de entrada del boton del encoder

float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto

float tension = 0;              	    //Nivel de tension de las baterias
float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
float potencia = 0;             	    //Potencia del motor
float energia = 0;              	    //Energia consumida por el motor
float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)

float corriente_objetivo = 0;           //Corriente estimada que debera mantener para alcanzar el timepo objetivo
float soc = 0;							//Estado de carga actual de las baterias (soc_calculado - soc_inicial)
float diferencia = 0;					//Diferencia entre la corriente actual y la objetivo

float tiempo_objetivo = 0;				//Tiempo establecido por el piloto para la competencia
float tiempo_transcurrido = 0;			//Tiempo transcurrido entre mediciones para el calculo de energia y sum_corriente
float tiempo_programa = 0;				//Tiempo transcurrido en minutos del funcionamiento del programa
float tiempo_previo = 0;				//Tiempo para seguir la ultima vez que se llamo al metodo calcular	

int muestreo = 1000;					//Tiempo de intervalo entre medidas

String error = "";						//Variable para disponer de un manejo de errores que se muestre en el LCD

float minimo = 200;						//Intervalo minimo de tiempo entre interrupciones (Evitar ruido)
float tiempo_interrupt = 0;				//Tiempo transcurrido entre interrupciones
bool boton_apretado = false;			//Si el boton del encoder fue apretado

void setup() {

	pinMode(sensor_inductivo  , INPUT);											//Señal cuadrada para velocidad de la rueda
	pinMode(boton_encoder, INPUT_PULLUP);										//Señal de entrada para el boton del encoder
	Serial.begin(9600);															//Inicio de comunicacion serial para identificacion de errores

	LCD_Leds.setup();															//Declaracion inicial de salidas y entradas para el header

	Calculos_Variables.setup();													//Declaracion inicial de salidas y entradas para el header

	Modulo_SD.setup(LCD_Leds.cargar());															//Declaracion inicial de salidas y entradas para el header

	attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
	//attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);	//Establecimiento de la interrupcion del sensor inductivo para la velocidad

}

void loop() {

	tiempo_transcurrido = (millis() - tiempo_programa);

	if( tiempo_transcurrido >= muestreo ){

		detachInterrupt(digitalPinToInterrupt(boton_encoder));
		//detachInterrupt(digitalPinToInterrupt(sensor_inductivo));

		tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_programa = Calculos_Variables. calcular(tiempo_transcurrido, tiempo_objetivo, revoluciones);

		LCD_Leds.datos(tension, corriente, potencia, energia, soc, corriente_objetivo, velocidad, error, tiempo_objetivo);
		LCD_Leds.control_leds(soc, diferencia);

		revoluciones = 0;

		error = Modulo_SD.SD_guardar("1", tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
		
		if(tiempo_objetivo > 0){
			tiempo_objetivo = (tiempo_objetivo - (tiempo_transcurrido/60000));
				
				if( tiempo_objetivo < 0){
					tiempo_objetivo = 0;
					Modulo_SD.estado_prueba();
				}

			error = Modulo_SD.SD_guardar("2", tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
		}

		if( ((corriente != 0) || (velocidad != 0)) && ((error != "ERROR 1") || (error != "ERROR 1") || (error != "ERROR 1")) ){
			error = Modulo_SD.SD_guardar("1", tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
			if(tiempo_objetivo > 0){
				
				error = Modulo_SD.SD_guardar("2", tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);

				tiempo_objetivo = (tiempo_objetivo - tiempo_transcurrido/60000);
				
				if( tiempo_objetivo < 0){
					tiempo_objetivo = 0;
					Modulo_SD.estado_prueba();
				}
			}
		}
		
		tiempo_programa = millis();

		attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
  		//attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);		//Establecimiento de la interrupcion del sensor inductivo para la velocidad

	}

	if(boton_apretado){
		detachInterrupt(digitalPinToInterrupt(boton_encoder));
		delay(250);
		boton_apretado = !boton_apretado;
	    Modulo_SD.estado_prueba();
	    tiempo_objetivo = LCD_Leds.comenzar_prueba();
	    delay(500);
	    tiempo_programa = millis();
	    attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
	}

}

void blink(){

	if(millis() - tiempo_interrupt > minimo){
		if(digitalRead(boton_encoder) == 0){
			boton_apretado = !boton_apretado;
		}else{
			revoluciones++;
		}
	}	
	tiempo_interrupt = millis();
}