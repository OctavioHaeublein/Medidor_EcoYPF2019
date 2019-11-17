#include <Arduino.h>

class Calculos_Variables{
	
	private:
		
		float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
		float tension_hall = 0; 				//Valor de tension de entrada del sensor hall
		float sum_potencia = 0;					//Sumatoria de las potencias a ser multiplicado por el tiempo_transcurrido
		
		float tension_minima = 48;		    	//Tension minima a la que puede funcionar el auto
		float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)
		
		float sum_corriente = 0;				//Variable para la sumatoria de corriente en el tiempo
		float soc_inicial = 1;					//Estado de carga inicial de las baterias 	(Medido con tension en vacio)
		float soc_calculado = 0;				//Estado de bateria calculado por suma de corrientes

		float r1 = 1010000;						//Valor medido de la primer resistencia del divisor resistivo
		float r2 = 82100;						//Valor medido de la segunda resistencia del divisor resisistivo

		float frecuencia = 0;					//Frecuencia calculada
		float diametro = 500;             	    //Diametro del eje (mm)

	public:

		float tension = 0;              	    //Nivel de tension de las baterias
		float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
		float potencia = 0;             	    //Potencia del motor
		float energia = 0;              	    //Energia consumida por el motor
		float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)
		float tiempo_programa = 0;
		float corriente_objetivo = 0;           //Corriente estimada que debera mantener para alcanzar el timepo objetivo
		float soc = 0;							//Estado de carga actual de las baterias (soc_calculado - soc_inicial)

		void setup(){
						
			pinMode(A0,INPUT);    				//Nivel de tension de las baterias
			pinMode(A1, INPUT);   				//Nivel de tension del sensor Hall
			
		}

		long interpolar (long x, long in_min, long in_max, long out_min,long out_max){
			return ( (x-in_min)*(out_max - out_min) + (out_min * (in_max - in_min)) ) / (in_max - in_min);
		}
		
		float calcular_capacidad (bool tabla, float sum_corriente, float tiempo_objetivo){	//Calculo de capacidad restante de  la bateria y calculo por tabla
			
			if(tabla){soc_inicial = map(tension,46.04,50.92,0.1,1);}
			
			soc_calculado = ((-1/capacidad_baterias) * sum_corriente) ;
			
			soc = (soc_inicial - soc_calculado);

			sum_corriente = 0;
			soc_inicial = soc;
			
			soc = (soc * capacidad_baterias);

			if(tiempo_objetivo != 0){
				corriente_objetivo = (soc / (tiempo_objetivo/60));			//Divide la capacidad restante de la bateria (Ah) por el tiempo restante pasado a horas, para determinar la corriente para desarrollar
			}else{
				corriente_objetivo = 0;
			}
						
			return soc, corriente_objetivo;
		}
		
		float calcular_velocidad(float revoluciones, float tiempo_transcurrido){

			revoluciones = (revoluciones / 2);
			frecuencia += (revoluciones/(tiempo_transcurrido/1000));

			velocidad = (((PI * diametro * frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)

			frecuencia = 0;
			
			return velocidad;
		}

		float calcular(float tiempo_transcurrido, float tiempo_objetivo, float revoluciones){
			
			/*---LECTURA DE SENSORES---*/
			  
			for(int i = 0; i < 5; i++){													//Se realizan 5 lecturas seguidas y se promedia
			tension_hall    += analogRead(A1);
			tension_divisor += analogRead(A0);
			}
			  
			/*---CALCULO DE TENSION, CORRIENTE, POTENCIA Y ENERGIA---*/
			  
			tension_divisor = (tension_divisor/5);
			  
			tension_divisor = ( (tension_divisor * (r1 + r2)) / r2 );
			  
			tension         = interpolar (tension_divisor, 7611.45, 9207.79, 38.4, 51.4);  
			  
			tension_hall    = (tension_hall / 10); 										//Mapea los niveles de tension medidos por el sensor hall
			  
			if(tension_hall >= 525){     
				corriente = interpolar (tension_hall, 525, 1023, 0, 70);
			}else{
					corriente = interpolar (tension_hall, 0, 525, 0, -70);
				}

			sum_corriente  +=  (corriente * tiempo_transcurrido);				//Calcula la corriente desarrollada en el tiempo para el calculo del SOC (Ampere - hora)
			potencia        = (tension * (corriente/ 3600000));							//Calcula la potencia en funcion de la corriente, y la tension medida en las baterias
			energia        += (potencia * (tiempo_transcurrido/ 3600000)); 				//Calcula la energia consumida hasta ese punto en funcion de la potencia y tiempo (watt - hora)

			tiempo_objetivo = (tiempo_objetivo - tiempo_transcurrido/60000);
			tiempo_programa += (tiempo_transcurrido/60000);

			if( tiempo_objetivo < 0){
				tiempo_objetivo = 0;
			}
			if( corriente == 0 ){
				soc, corriente_objetivo = calcular_capacidad(true, sum_corriente, tiempo_objetivo);
			}else{
					soc, corriente_objetivo = calcular_capacidad(false, sum_corriente, tiempo_objetivo);
			}
			
			calcular_velocidad(revoluciones, tiempo_transcurrido);

			Serial.println(tiempo_objetivo);

			return tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa;
		}

};
