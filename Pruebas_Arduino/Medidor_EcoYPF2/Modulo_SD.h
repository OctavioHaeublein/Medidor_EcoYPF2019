#include <SD.h>
#include <SPI.h>
#include <Arduino.h>

class Modulo_SD{

	private:

		int posicion_principal = 0;			    	//Numero de archivo principal
		int posicion_secundario = 0;				//Numero de archivo secundario
		String datos_principal = "DPRIM_";	    	//Nombre del archivo principal de adquisición de datos
		String datos_secundario = "DSEC_";	    	//Nombre del archivo secundario
		String extension = ".txt";			    	//Extensión del archivo para concatenarlos
		bool guardar_secundario = false;
		const int cs = 8;
		File datos;

	public:
		
		float tension = 0;
		float corriente = 0;
		float potencia = 0;
		float energia = 0;
		float soc = 0;
		float velocidad = 0;
		float corriente_objetivo = 0;
		float tiempo_objetivo = 0;
		float tiempo_programa = 0;

		

		String setup( bool cargar){								//Al inicio del programa busca datos anteriores para cargar
			
			if(!SD.begin(cs)){
				datos_principal = "DPRIM_0.txt";
				datos_secundario = "DSEC_0.txt";
				Serial.println("ERROR 1");
				return datos_principal, datos_secundario;
			}
			
			  
			for (int i = 1; i<= 50; i++){			//Comienza a buscar el ultimo archivo principal de datos
				datos_principal.concat(i);
				datos_principal.concat(extension);
					
			if(!SD.exists(datos_principal)){
				datos_principal = "DPRIM_";
				datos_principal.concat(i-1);
				datos_principal.concat(extension);
				
				if( SD.exists(datos_principal) ){
					posicion_principal = (i-1);
					break;					
					}else{datos_principal = "DPRIM_";}
				}else{datos_principal = "DPRIM_";}
			  
			}
			  
			for (int i = 1; i<= 50; i++){		//Comienza a buscar el ultimo archivo secundario de datos
				datos_secundario.concat(i);
				datos_secundario.concat(extension);
				
				if(!SD.exists(datos_secundario){
					datos_secundario = "DSEC_";
					datos_secundario.concat(i-1);
					datos_secundario.concat(extension);
					
					if( SD.exists(datos_secundario) ){
						posicion_secundario = i;
					    datos_secundario = "DSEC_";
					    datos_secundario.concat(posicion_secundario);
					    datos_secundario.concat(extension);	
						break;
						}else{datos_secundario = "DSEC_";}
					}else{datos_secundario = "DSEC_";}
			
			}

			if(datos_secundario == "DSEC_"){
				datos_secundario = "DSEC_0.txt";	//Si no encuentra un archivo secundario, lo nombra como el primero
			}
			
			if(datos_principal == "DPRIM_"){		//Si no encuentra un archivo principal de datos, se nombra como el primero
				datos_principal = "DPRIM_0.txt";
			}else{
				if(!cargar){
					posicion_principal++;
				    datos_principal = "DPRIM_";
				    datos_principal.concat(posicion_principal);
				    datos_principal.concat(extension);
				}
			}
			Serial.print("Setup");
			Serial.println(datos_principal);
			Serial.println(datos_secundario);
			Serial.println(cargar);

			return datos_principal, datos_secundario;
		}
		
		String SD_guardar (String archivo, float tension, float corriente, float potencia, float energia, float soc, float velocidad, float corriente_objetivo, float tiempo_objetivo, float tiempo_programa){		//Guardar datos en archivo txt

			if(archivo == "1"){
				archivo = datos_principal;
			}else{
				archivo = datos_secundario;
			}

			Serial.println(archivo);

			if(! SD.begin(cs) ){
				Serial.println("ERROR 1");
				return "ERROR 1";
			}
			  
			datos = SD.open(archivo,FILE_WRITE);    	//La variable "Archivo" Puede variar entre copias y el principal
			  
			if(!datos){                             	//Comprueba si puede escribir a la SD, envía error si no fuese así
				Serial.println("ERROR 2");
				return "ERROR 2";
			}
													 	 //Almacena las distintas variables separadas por una coma
			datos.print(tiempo_programa);
			datos.print(",");
			datos.print(tension);
			datos.print(",");
			datos.print(corriente);
			datos.print(",");
			datos.print(potencia);
			datos.print(",");
			datos.print(energia);
			datos.print(",");
			datos.print(soc);
			datos.print(",");
			datos.print(velocidad);
			datos.print(",");
			datos.print(corriente_objetivo);
			datos.print(",");
			datos.println(tiempo_objetivo);
			  
			datos.close();                          //Cierra el archivo y vuelve al programa
			
			return "";
		}

		void estado_prueba (){							//Configuracion de tiempo de la prueba, y cambio de archivos

			if(guardar_secundario){
				guardar_secundario = false;
				posicion_secundario++;
			    datos_secundario = "DSEC_";
			    datos_secundario.concat(posicion_secundario);
			    datos_secundario.concat(extension);
			    Serial.println(datos_secundario);	
			}else{
				guardar_secundario = true;
			}

			return;
		}

};
