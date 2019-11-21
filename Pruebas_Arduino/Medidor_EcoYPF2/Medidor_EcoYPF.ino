#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

/*---VARIBLES LCD Y LEDS---*/
LiquidCrystal_I2C lcd(0x27,20,4);   	//Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
const int LA1 = 34;
const int LA2 = 36;
const int LA3 = 38;

const int LR1 = 40;
const int LR2 = 42;

const int LV1 = 43;
const int LV2 = 41;
const int LV3 = 39;
const int LV4 = 37;
const int LV5 = 35;
  
const int RGB_R = 4;
const int RGB_V = 5;
const int RGB_A = 6;

const int dt = .0;
const int clk = 1;

float verde = 0;
float rojo = 0;
float azul = 0;

/*---INTERRUPTS---*/
const int sensor_inductivo = 2;			//Pin de entrada del sensor inductivo
const int boton_encoder = 3;			//Pin de entrada del boton del encoder

/*---CALCULOS, VELOCIDAD, Y TIEMPOS---*/
float revoluciones = 0;		    		//Contador de revoluciones dadas por la rueda del auto
float frecuencia = 0;					//Frecuencia calculada
float diametro = 500;             	    //Diametro del eje (mm)	

float r1 = 1010000;						//Valor medido de la primer resistencia del divisor resistivo
float r2 = 82100;						//Valor medido de la segunda resistencia del divisor resisistivo

float tension_divisor = 0;  			//Valor de tension de entrada del divisor resistivo 
float tension_hall = 0; 				//Valor de tension de entrada del sensor hall
float sum_potencia = 0;					//Sumatoria de las potencias a ser multiplicado por el tiempo_transcurrido

float tension_minima = 48;		    	//Tension minima a la que puede funcionar el auto
float capacidad_baterias = 10;          //Capacidad de las baterias (Ampere - hora)

float sum_corriente = 0;				//Variable para la sumatoria de corriente en el tiempo
float soc_inicial = 1;					//Estado de carga inicial de las baterias 	(Medido con tension en vacio)
float soc_calculado = 0;				//Estado de bateria calculado por suma de corrientes

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

/*---MODULO SD---*/
int posicion_principal = 0;			    	//Numero de archivo principal
int posicion_secundario = 0;				//Numero de archivo secundario
String datos_principal = "DPRIM_";	    	//Nombre del archivo principal de adquisición de datos
String datos_secundario = "DSEC_";	    	//Nombre del archivo secundario
String extension = ".txt";			    	//Extensión del archivo para concatenarlos
bool guardar_secundario = false;
const int cs = 8;
File datos;

void setup() {

	/*---INICIO DE PROGRAMA---*/
	Serial.begin(9600);          			//Inicia una comunicacion serial para edicion de codigo

	/*---CONFIGURACION LCD---*/  
	lcd.init();								//Inicio de la funcion lcd
	lcd.clear();							//Pone en blanco el lcd
	lcd.backlight();						//Enciende el backlight del display
	lcd.setCursor(0,0);
	lcd.print("CARGANDO PROGRAMA...");
	lcd.setCursor(0,2);
	lcd.print("NO ACELERAR");

	/*---ENCENDIDO DE LEDS---*/
	int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};
	for(int i = 0; i<=9; i++){
		digitalWrite(leds[i],HIGH);
	}
	  
	/*---DIGITAL OUTPUTS---*/
	pinMode(LR1 , OUTPUT);   				//LED Rojo1
	pinMode(LR2 , OUTPUT);   				//LED Rojo
	pinMode(LA1 , OUTPUT);   				//LED Amarillo
	pinMode(LA2 , OUTPUT);   				//LED Amarillo1
	pinMode(LA3 , OUTPUT);   				//LED Amarillo2
	pinMode(LV1 , OUTPUT);   				//LED Verde
	pinMode(LV2 , OUTPUT);   				//LED Verde1
	pinMode(LV3 , OUTPUT);   				//LED Verde2
	pinMode(LV4 , OUTPUT);   				//LED Verde3
	pinMode(LV5 , OUTPUT);   				//LED Verde4
	  
	/*---DIGITAL PWM OUTPUTS---*/
	pinMode(RGB_R , OUTPUT);    			//RGB Rojo
	pinMode(RGB_V , OUTPUT);    			//RGB Verde
	pinMode(RGB_A , OUTPUT);    			//RGB Azul

	/*---CONFIGURACION ENCODER---*/
	pinMode(boton_encoder  , INPUT_PULLUP);	//Señal de entrada del boton
	pinMode(dt  , INPUT);					//Señal de entrada 'A' del encoder
	pinMode(clk , INPUT);					//Señal de entrada 'B' del encoder

	/*---CONFIGURACION INTERRUPTS---*/
	pinMode(sensor_inductivo  , INPUT);											//Señal cuadrada para velocidad de la rueda
	pinMode(boton_encoder, INPUT_PULLUP);										//Señal de entrada para el boton del encoder

	/*---CONFIGURACION ENTRADAS CORRIENTE Y TENSION---*/
	pinMode(A0,INPUT);    				//Nivel de tension de las baterias
	pinMode(A1, INPUT);   				//Nivel de tension del sensor Hall
	
	setup_sd(cargar_datos_lcd());															//Declaracion inicial de salidas y entradas para el header

	attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
	//attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);	//Establecimiento de la interrupcion del sensor inductivo para la velocidad

}

void loop() {

	tiempo_transcurrido = (millis() - tiempo_programa);

	if( tiempo_transcurrido >= muestreo ){

		detachInterrupt(digitalPinToInterrupt(boton_encoder));
		//detachInterrupt(digitalPinToInterrupt(sensor_inductivo));

		tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa = calcular_variables(tiempo_transcurrido, tiempo_objetivo, revoluciones);
		Serial.print("tiempo_objetivo: ");
		Serial.println(tiempo_objetivo);
		LCD_datos(tension, corriente, potencia, energia, soc, corriente_objetivo, velocidad, error, tiempo_objetivo);
		control_leds(soc, diferencia);

		revoluciones = 0;

		error = SD_guardar(datos_principal, tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
		
		if(tiempo_objetivo > 0){
			Serial.print("Secundario: ");
			Serial.println(tiempo_objetivo);
			error = SD_guardar(datos_secundario, tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
		}


		if( ((corriente != 0) || (velocidad != 0)) && ((error != "ERROR 1") || (error != "ERROR 1") || (error != "ERROR 1")) ){
			error = SD_guardar(datos_principal, tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
			if(tiempo_objetivo > 0){
				error = SD_guardar(datos_secundario, tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa);
			}else{
				estado_prueba();
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
	    estado_prueba();
	    tiempo_objetivo = comenzar_prueba();
	    delay(250);
	    attachInterrupt(digitalPinToInterrupt(boton_encoder), blink, FALLING);		//Establecimiento de la interrupcion del boton del encoder para el programa
	}

}

/*---MANEJO DEL MODULO SD---*/

String setup_sd ( bool cargar){								//Al inicio del programa busca datos anteriores para cargar
	
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
		
		if(!SD.exists(datos_secundario) ){
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

/*---METODO PARA ESCRIBIR LOS ARCHIVOS EN LA TARJETA---*/
String SD_guardar (String archivo, float tension, float corriente, float potencia, float energia, float soc, float velocidad, float corriente_objetivo, float tiempo_objetivo, float tiempo_programa){		//Guardar datos en archivo txt

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

/*---MANEJO DEL BOOLEANO 'guardar_secundario' PARA ARCHIVO SD---*/
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

/*---MANEJO DEL DISPLAY LCD---*/

void LCD_datos (float tension, float corriente, float potencia, float energia, float soc, float corriente_objetivo, float velocidad, String error, float tiempo_objetivo){							//Layout principal para el display LCD
	//Serial.println(tiempo_objetivo);
	lcd.clear();							//Pone en blanco el lcd
	lcd.setCursor(0,0);
	lcd.print("V:");
	lcd.print(velocidad);
	lcd.setCursor(7,0);
	lcd.print("Km/h");
	lcd.setCursor(13,0);
	lcd.print(error);
	lcd.setCursor(0,1);
	lcd.print("I:");
	lcd.print(corriente);
	lcd.setCursor(7,1);
	lcd.print("A");
	lcd.setCursor(11,1);
	lcd.print(tiempo_objetivo);
	lcd.setCursor(17,1);
	lcd.print("MIN");
	lcd.setCursor(0,2);
	lcd.print("T:");
	lcd.print(tension);
	lcd.setCursor(7,2);
	lcd.print("V");
	lcd.setCursor(11,2);
	lcd.print("C:");
	lcd.print(soc);
	lcd.setCursor(19,2);
	lcd.print("%");
	lcd.setCursor(0,3);
	lcd.print("P:");
	lcd.print(potencia);
	lcd.setCursor(7,3);
	lcd.print("W");
	lcd.setCursor(11,3);
	lcd.print("E:");
	lcd.print(energia);
	lcd.setCursor(18,3);
	lcd.print("Wh");
}

/*---INICIO DE PROGRAMA PARA DETERMIAR ARCHIVOS---*/
bool cargar_datos_lcd(){

	lcd.clear();  
	lcd.setCursor(0,0);             
	lcd.print("Datos encontrados.");          
	lcd.setCursor(0,2);             
	lcd.print("Desea cargar?");                  
	lcd.setCursor(0,3);
	lcd.print("   NO          SI   ");

	int aState = 0;
	int aLastState = digitalRead(dt);

	while(true){
	
		aState = digitalRead(dt);

		if(aState != aLastState){
			if(digitalRead(clk) != aState){
				lcd.clear();  
				lcd.setCursor(0,0);             
				lcd.print("Datos encontrados.");          
				lcd.setCursor(0,2);             
				lcd.print("Desea cargar?");                  
				lcd.setCursor(0,3);
				lcd.print("   NO         >SI   ");
				cargar_datos = true;
			}else{
				lcd.clear(); 
				lcd.setCursor(0,0);             
				lcd.print("Datos encontrados.");          
				lcd.setCursor(0,2);             
				lcd.print("Desea cargar?");                  
				lcd.setCursor(0,3);
				lcd.print("   >NO         SI   ");
				cargar_datos = false;
			}
		}

		aLastState = aState;
		
		if(digitalRead(boton_encoder) == 0){
			
			lcd.clear();          
			lcd.setCursor(0,0);

			if(cargar_datos){
				lcd.print("Cargando datos...");
			}
			if(!cargar_datos){
				lcd.print("Comenzando de cero.");
			}

			delay(500);
			return cargar_datos;
		}

	}

}

/*---CONFIGURACION DEL TIEMPO DE LA PRUEBA VIA LCD---*/
float comenzar_prueba(){

	int aState = 0;
	int aLastState = digitalRead(dt);
	float tiempo_objetivo = 0;

	lcd.clear();          
	lcd.setCursor(0,0);
	lcd.print("Tiempo (min): ");
	lcd.setCursor(14,0);
	lcd.print(tiempo_objetivo);

	while(true){

		aState = digitalRead(dt);

		if(aState != aLastState){
			if(digitalRead(clk) != aState){
				tiempo_objetivo++;
			}else{
				tiempo_objetivo--;
				if(tiempo_objetivo < 0){
					tiempo_objetivo = 0;
				}
			}

			lcd.clear();          
			lcd.setCursor(0,0);
			lcd.print("Tiempo (min): ");
			lcd.setCursor(14,0);
			lcd.print(tiempo_objetivo);

		}

		aLastState = aState;
	
		if( digitalRead(boton_encoder) == 0 ){
			delay(500);
			break;
		}
	}

	return tiempo_objetivo;

}

/*---MANEJO DE LEDS---*/
void control_leds(float soc, float diferencia){							//Control de tira de LEDS y RGB 

	int led = map(soc,0.1,1,0,9);
	int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};

	for(int i = 0; i<=9; i++){
		digitalWrite(leds[i],LOW);
	}

	digitalWrite(led, HIGH);
	
}

/*---MANEJO DE CALCULOS DE BATERIAS, VELOCIDAD, Y TIEMPOS---*/

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

float calcular_variables(float tiempo_transcurrido, float tiempo_objetivo, float revoluciones){
	
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

	Serial.print("tiempo_objetivo: ");
	Serial.println(tiempo_objetivo);

	return tension, corriente, potencia, energia, soc, velocidad, corriente_objetivo, tiempo_objetivo, tiempo_programa;
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



/*


		if( (tension > 65) || (tension < 40) ){
				error = "ERROR 4";
		}

		if( (corriente != 0) && (velocidad == 0)){
				error = "ERROR 5";
		}

*/