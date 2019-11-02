class Modulo_SD{
	
	#include <SD.h>
	#include <SPI.h>
	#include "LCD_Leds.h"

	private:

		int posicion_principal = 0;			    	//Numero de archivo principal
		int posicion_secundario = 0;				//Numero de archivo secundario
		const int cs = 9;
		LCD_Leds LCD_Leds;

	public:
	
		String datos_principal = "DPRIM_";	    	//Nombre del archivo principal de adquisición de datos
		String datos_secundario = "DSEC_";	    	//Nombre del archivo secundario
		String extension = ".txt";			    	//Extensión del archivo para concatenarlos
		bool guardar_secundario = false;			//Indica si deberan guardarse en archivo a parte los valores obtenidos

		String setup(){								//Al inicio del programa busca datos anteriores para cargar

			LCD_Leds.display("Cargando...");		//Intenta comunicarse con el modulo SD
			
			if(!SD.begin(cs)){
				return;
			}
			
			  
			for (int i = 0; i<= 20; i++){			//Comienza a buscar el ultimo archivo principal de datos
				datos_principal.concat(i);
				datos_principal.concat(extension);
					
			if( (!SD.exists(datos_principal)) && (i > 0) ){
				datos_principal = "DPRIM_";
				datos_principal.concat(i-1);
				datos_principal.concat(extension);
				
				if( SD.exists(datos_principal) ){
					posicion_principal = i;
					datos_principal.concat(i);
					datos_principal.concat(extension);
					break;					
					}else{datos_principal = "DPRIM_";}
				}else{datos_principal = "DPRIM_";}
			  
			}
			  
			for (int i = 0; i<= 20; i++){			//Comienza a buscar el ultimo archivo secundario de datos
			datos_secundario.concat(i);
			datos_secundario.concat(extension);
				
			if( (!SD.exists(datos_principal)) && (i > 0) ){
				datos_secundario = "DSEC_";
				datos_secundario.concat(i-1);
				datos_secundario.concat(extension);
				
				if( SD.exists(datos_secundario) ){
					posicion_secundario = i;
					datos_secundario.concat(i);
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
				if( LCD_Leds.cargar() ){
					cargar_datos();
				}else{
					datos_principal = "DPRIM_0.txt";
					datos_secundario = "DSEC_0.txt";
				}
			}

			return datos_principal, datos_secundario

		}

		void cargar_datos(String datos_principal){

			File datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
			  
			if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
				LCD_Leds.display("E3, E2");
				return;
			}
			  
			int posicion = 0;
			  
			for(int i = 1; i <= datos.size() ; i++){       //Busca por la ultima linea para poder leer exclusivamente esa
				datos.seek(datos.size()-i);
				if(datos.read() == 10){                     //PUEDE SER QUE HAYA QUE CAMBIAR A i = 1 POR EL TEMA DE NUEVA LINEA
					posicion = (datos.position()+1);          //Devuelve la posicion del primer caracter de la ultima linea
					break;
				}
			}
			  
			datos.close();
			  
			File datos = SD.open(datos_principal,FILE_READ);    //La variable "Archivo" Puede variar entre copias y el principal
			  
			if(!datos){                                    //Comprueba si puede escribir a la SD, envía error si no fuese así
				return;
			}
			  
			datos.seek(posicion);                          //Se desplaza hasta la posicion obtenida anteriormente
			  
			if(datos.available()){                         //Comienza a leer la ultima fila para obtener los datos guardados
				tiempo_transcurrido = datos.read();
				tension = datos.read();
				corriente = datos.read();
				potencia = datos.read();
				energia = datos.read();
				velocidad = datos.read();
				corriente_objetivo = datos.read();

			}
			  
			datos.close();

			return;
			
			}
		
		void SD_guardar (String archivo){		//Guardar datos en archivo txt

			if(! SD.begin(cs) ){
				LCD_Leds.display("E1");
				return;
			}
			  
			File datos = SD.open(archivo,FILE_WRITE);    //La variable "Archivo" Puede variar entre copias y el principal
			  
			if(!datos){                             //Comprueba si puede escribir a la SD, envía error si no fuese así
				LCD_Leds.display("E2");
				return;
			}
													  //Almacena las distintas variables separadas por una coma
			datos.print(tiempo_transcurrido);
			datos.print(",");
			datos.print(tension);
			datos.print(",");
			datos.print(corriente);
			datos.print(",");
			datos.print(potencia);
			datos.print(",");
			datos.print(energia);
			datos.print(",");
			datos.print(velocidad);
			datos.print(",");
			datos.println(corriente_objetivo);
			  
			datos.close();                          //Cierra el archivo y vuelve al programa
			return;
		}

		bool estado_prueba (){							//Configuracion de tiempo de la prueba, y cambio de archivos

			if(guardar_secundario){
				guardar_secundario = false;
				posicion_secundario++;
			    datos_secundario = "DSEC_";
			    datos_secundario.concat(posicion_secundario);
			    datos_secundario.concat(extension);	
			}
			if(!guardar_secundario){
				guardar_secundario = true;
				tiempo_objetivo = LCD_Leds.comenzar_prueba();
				tiempo_inicio_prueba = millis();
			}

			return guardar_secundario;
		}

};
