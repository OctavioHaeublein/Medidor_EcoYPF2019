#include "Calculos_Variables.h"
#include "Modulo_SD.h"
#include "LCD_Leds.h"
#include "Velocidad_Aceleracion.h"



#define sensor_inductivo    2			//Pin de entrada del sensor inductivo
float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto

LCD_Leds LCD_Leds;

void setup() {

  pinMode(sensor_inductivo  , INPUT);	//Señal cuadrada para velocidad de la rueda
  
  LCD_Leds.setup();

  datos_principal, datos_secundario = Modulo_SD.setup();
  
  attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);
  attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
  
}

void loop() {														//Loop principal del programa
 
	if(){
		if(corriente == 0){calcular_capacidad(true);};				//Puede que nunca lea 0 por temas de ruido
		if(corriente != 0){calcular_capacidad(false);}
	}
	
	if( ( (velocidad != 0) || (corriente != 0) ) && () ){
		SD_guardar(datos_principal);                              	//Para guardar los datos en el txt
		if(estado_prueba){										
			SD_guardar(datos_secundario);
		}					
		n_guardado++;
	}
  
}

void blink(){							//Contador de revoluciones y boton del encoder

  if(digitalRead(boton_encoder) == 0){				//Si se pone a 0 la entrada del boton, entonces alterna el valor del booleano
	estado_prueba = Modulo_SD.estado_prueba();
	
  }else{								//Si se pone en 0 la entrada del sensor optico, agrega una revolucion al contador
		revoluciones++;		
		}
	}	
