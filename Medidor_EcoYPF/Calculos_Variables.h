class Calculos{
	
	private:
		
		float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
		float tension_hall = 0; 				//Valor de tension de entrada del sensor hall
		float sum_potencia = 0;					//Sumatoria de las potencias a ser multiplicado por el tiempo_transcurrido
		
		float tension_minima = 48;		    	//Tension minima a la que puede funcionar el auto
		float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)
		
		float sum_corriente = 0;				//Variable para la sumatoria de corriente en el tiempo
		float soc_inicial = 1;					//Estado de carga inicial de las baterias 	(Medido con tension en vacio)
		float soc_calculado = 0;				//Estado de bateria calculado por suma de corrientes
		float contador_soc = 0;					//Contador para el calculo del soc 

		float r1 = 1010000;
		float r2 = 82100;

	public:
		//FALTA SOLUCIONAR EL CALCULO DE LA ENERGIA DE ALGUNA FORMA
		
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

		void setup(){
						
			/*---ANALOG INPUTS---*/
			pinMode(A0,INPUT);    			//Nivel de tension de las baterias
			pinMode(A1, INPUT);   			//Nivel de tension del sensor Hall
			
		}

		long interpolar (long x, long in_min, long in_max, long out_min,long out_max){
			return ( (x-in_min)*(out_max - out_min) + (out_min * (in_max - in_min)) ) / (in_max - in_min);
		}
		
		float calcular_capacidad (bool tabla){	//Calculo de capacidad restante de  la bateria y calculo por tabla
			
			if(tabla){soc_inicial = map(tension,46.04,50.92,0.1,1);}
			
			soc_calculado = (-1/capacidad_baterias) * (tiempo_transcurrido * sum_corriente);
			
			soc = (soc_inicial - soc_calculado);
			
			contador_soc = 0;
			sum_corriente = 0;
			soc_inicial = soc;
			
			soc = (soc * capacidad_baterias);
			corriente_objetivo = (soc / (tiempo_objetivo/60));			//Divide la capacidad restante de la bateria (Ah) por el tiempo restante pasado a horas, para determinar la corriente para desarrollar
			
			return soc, corriente_objetivo;
		}
		
		float calcular(){
			
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
			  
			if(tension_hall >= 525){     corriente 	      = interpolar (tension_hall, 525, 1023, 0, 70);}
			else{if(tension_hall < 525){ corriente 	      = interpolar (tension_hall, 0, 525, 0, -70);}}
			
			sum_corriente  +=  corriente;
			potencia        = (tension * corriente);								//Calcula la potencia en funcion de la corriente, y la tension medida en las baterias
			sum_potencia   +=  potencia;											//Calcula la energia consumida hasta ese punto en funcion de la potencia y tiempo (watt - hora)
			energia         = (sum_potencia * tiempo_transcurrido);
			
			soc, corriente_objetivo = calcular_capacidad(false);
			
			return tension, corriente, potencia, energia, soc, corriente_objetivo;
		}

};
