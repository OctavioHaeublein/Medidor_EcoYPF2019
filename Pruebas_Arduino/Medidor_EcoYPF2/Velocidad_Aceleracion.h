#include <Arduino.h>

class Velocidad_Aceleracion{

	private:

		float frecuencia = 0;					//Frecuencia calculada
		float diametro = 500;             	    //Diametro del eje (mm)

	public:
	
		float velocidad = 0;
	
		float calcular_velocidad(float revoluciones, float tiempo_transcurrido){

			revoluciones = (revoluciones / 2);
			frecuencia += (revoluciones/(tiempo_transcurrido/1000));

			velocidad = (((PI * diametro * frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)

			frecuencia = 0;
			
			return velocidad;
		}
	
};
