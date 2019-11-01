class Velocidad_Aceleracion{
	Private:
		
		/*--VARAIBLES ACELERADOR---*/
		float entrada_acelerador = 0;
		float salida_acelerador = 0;
		
		/*--VARIABLES VELOCIDAD---*/
		float tiempo = 0;
		int contador = 0;
		float tiempo_previo_velocidad = 0;
		float revoluciones = 0;
		float frecuencia = 0;
		float diametro = 0;             	    //Diametro del eje (mm)

		/*---VARIABLES PID---*/
		float kp = 0;
		float ki = 0;
		float kd = 0;
		float corriente_objetivo = 0;
		float corriente = 0;
		float salida_pid = 0;
		float error = 0;
		float error_previo = 0;
		float tiempo_previo_pid = 0;
		float tiempo_transcurrido = 0;
		float sum_error = 0;
		float freq_error = 0;
		float limite_pid = 0;
		
	Public:
	
		float velocidad = 0;
	
		float calcular_velocidad(){
	
			tiempo = millis();
			
			
			if( (tiempo - tiempo_previo_velocidad) > 1000){
				
				detachInterrupt(digitalPinToInterrupt(sensor_inductivo));
				
				revoluciones    = (revoluciones / 2);
				frecuencia     += (revoluciones/((tiempo-tiempo_previo_velocidad)/1000));
				contador ++;
				revoluciones = 0;
				tiempo_previo_velocidad = tiempo;
				attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
			}
			
			if(contador >= 5){
				detachInterrupt(digitalPinToInterrupt(sensor_inductivo));
				
				frecuencia = (frecuencia / contador);
				
				velocidad = (((PI * 500 * frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)
				frecuencia = 0;
				contador = 0;
				
				attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
			}
			
			return velocidad;
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
		
		void acelerador( char estado_acelerador, float nivel_acelerador ){
	
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
	
}