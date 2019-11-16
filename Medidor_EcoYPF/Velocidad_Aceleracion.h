#include <Arduino.h>

class Velocidad_Aceleracion{
	private:
		
		/*--VARAIBLES ACELERADOR---*/
		float entrada_acelerador = 0;			//Valor analogico de entrada del acelerador de puño
		float salida_acelerador = 0;			//Valor analogico de salida hacia el controlador
		const int pwm_acelerador = 0;			//Pin de salida PWM para el acelerador del auto
		
		/*--VARIABLES VELOCIDAD---*/
		float frecuencia = 0;					//Frecuencia calculada
		float diametro = 500;             	    //Diametro del eje (mm)

		/*---VARIABLES PID---*/
		float kp = 0;							//Constante proporcional del PID
		float ki = 0;							//Constante integral del PID
		float kd = 0;							//Constante derivativa del PID
		float corriente_objetivo = 0;			//Objetivo de corriente a mantener calculado por el SOC
		float corriente = 0;					//Nivel actual de corriente para hacer la correccion
		float salida_pid = 0;					//Salida analogica del PID que limitara el acelerador
		float error = 0;						//Error calculado por el sistema
		float error_previo = 0;					//Error previo calculado
		float tiempo_previo_pid = 0;			//Tiempo anterior en que fue llamada la funcion
		float tiempo_transcurrido_pid = 0;			//Tiempo transcurrido en la funcion
		float sum_error = 0;					//Sumatoria de los errores para integrar
		float freq_error = 0;					//Frequencia del error para derivar
		
	public:
	
		float velocidad = 0;

		void setup(){
			pinMode(pwm_acelerador, OUTPUT);
		}
	
		float calcular_velocidad(float revoluciones, float tiempo_transcurrido){

			revoluciones = (revoluciones / 2);
			frecuencia += (revoluciones/(tiempo_transcurrido/1000));

			velocidad = (((PI * diametro * frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)

			revoluciones = 0;
			frecuencia = 0;
			
			return velocidad;
		}

		float calculo_pid(){
			
			float tiempo = millis();
			
			tiempo_transcurrido_pid = (tiempo - tiempo_previo_pid);
			
			error = corriente_objetivo - corriente;
			
			sum_error += (error * tiempo_transcurrido_pid);
			
			freq_error = ((error - error_previo)/tiempo_transcurrido_pid);
			
			salida_pid = (kp * error) + (ki * sum_error) + (kd * freq_error);
			
			error_previo = error;
			tiempo_previo_pid = tiempo;
			
			if(salida_pid < 0){
				salida_pid = 0;
			}else{
				salida_pid = map(salida_pid,0,100,0,100);
			}
			
			return salida_pid;

		}
		
		void acelerador( char estado_acelerador, float nivel_acelerador ){
	
			entrada_acelerador = analogRead(A0);
			entrada_acelerador = map(entrada_acelerador,179,880,70,190);
			
			switch(estado_acelerador){
				
				case 'M':

					salida_acelerador = entrada_acelerador;

					break;
					
				case 'A':

					salida_pid = calculo_pid();
					salida_acelerador = ((salida_pid * 120)/100);
					salida_acelerador += 70;
					if( entrada_acelerador < salida_acelerador){
						salida_acelerador = entrada_acelerador;
					}

				break;
				
				case 'C':

					salida_acelerador = ((nivel_acelerador * 120)/100);
					salida_acelerador += 70;
					if( entrada_acelerador < salida_acelerador){
					salida_acelerador = entrada_acelerador;
					}

				break;

			}
			
			analogWrite(pwm_acelerador, salida_acelerador);
			
		}
	
};
