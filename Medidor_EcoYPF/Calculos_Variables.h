Class Calculos{
	Private:
		
		float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
		float tension_hall = 0; 				//Valor de tension de entrada del sensor hall
		float sum_potencia = 0;					//Sumatoria de las potencias a ser multiplicado por el tiempo_transcurrido
		
		float tension_minima = 48;		    	//Tension minima a la que puede funcionar el auto
		float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)
		
		float sum_corriente = 0;				//Variable para la sumatoria de corriente en el tiempo
		float SOC_inicial = 1;					//Estado de carga inicial de las baterias 	(Medido con tension en vacio)
		float SOC_calculado = 0;				//Estado de bateria calculado por suma de corrientes
		float contador_SOC = 0;					//Contador para el calculo del SOC 

	Public:
		//FALTA SOLUCIONAR EL CALCULO DE LA ENERGIA DE ALGUNA FORMA
		
		float tension = 0;              	    //Nivel de tension de las baterias
		float corriente = 0;            	    //Corriente calculada del motor (Depende si carga, o no lo hace)
		float potencia = 0;             	    //Potencia del motor
		float energia = 0;              	    //Energia consumida por el motor
		float velocidad = 0;            	    //Velocidad tangencial de la rueda (Km/h)

		float corriente_objetivo = 0;           //Corriente estimada que debera mantener para alcanzar el timepo objetivo
		float SOC = 0;							//Estado de carga actual de las baterias (SOC_calculado - SOC_inicial)
		
		void setup(){
						
			/*---ANALOG INPUTS---*/
			pinMode(A0,INPUT);    			//Nivel de tension de las baterias
			pinMode(A1, INPUT);   			//Nivel de tension del sensor Hall
			
		}
		
		float calcular_capacidad (bool tabla){	//Calculo de capacidad restante de  la bateria y calculo por tabla
			
			if(tabla){SOC_inicial = map(tension,46.04,50.92,0.1,1);}
			
			SOC_calculado = (-1/capacidad_baterias) * (muestreo * contador_SOC * sum_corriente);
			
			SOC = (SOC_inicial - SOC_calculado);
			
			contador_SOC = 0;
			sum_corriente = 0;
			SOC_inicial = SOC;
			
			SOC = (SOC * capacidad_baterias);
			corriente_objetivo = (SOC / (tiempo_objetivo/60));			//Divide la capacidad restante de la bateria (Ah) por el tiempo restante pasado a horas, para determinar la corriente para desarrollar
			
			return SOC, corriente_objetivo;
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
			  
			if(tension_hall >= 525){     corriente 	      = interpolar (tension_hall, 525, 1023, 0, 70);}}
			else{if(tension_hall < 525){ corriente 	      = interpolar (tension_hall, 0, 525, 0, -70);}}
			
			sum_corriente  +=  corriente;
			potencia        = (tension * corriente);								//Calcula la potencia en funcion de la corriente, y la tension medida en las baterias
			sum_potencia   +=  potencia;											//Calcula la energia consumida hasta ese punto en funcion de la potencia y tiempo (watt - hora)
			energia         = (sum_potencia * tiempo_energia);
			
			SOC, corriente_objetivo = calcular_capacidad();
			
			return tension, corriente, potencia, energia, SOC, corriente_objetivo;
		}

}