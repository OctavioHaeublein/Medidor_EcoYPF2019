#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,20,4);   	//Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

class LCD_Leds{

	private:

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
		
		const int boton_encoder = 3;
		const int dt = .0;
		const int clk = 1;

		float verde = 0;
		float rojo = 0;
		float azul = 0;
		
	public:

		bool cargar_datos = false;

		void setup(){
			
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
			lcd.setCursor(0,3);
			lcd.print("HAEUBLEIN, HECHEME");

			/*---ENCENDIDO DE LEDS---*/
			int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};
			for(int i = 0; i<=9; i++){
				digitalWrite(leds[i],HIGH);
				delay(100);
			}
			delay(250);
			for(int i = 9; i==0; i--){
				digitalWrite(leds[i],LOW);
				delay(100);
			} 
			delay(250);

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

			/*---CONFIGURACION ENCODER---*/
			pinMode(boton_encoder  , INPUT_PULLUP);	//Señal de entrada del boton
	 		pinMode(dt  , INPUT);					//Señal de entrada 'A' del encoder
	  		pinMode(clk , INPUT);					//Señal de entrada 'B' del encoder

		}
		
		void datos (float tension, float corriente, float potencia, float energia, float soc, float corriente_objetivo, float velocidad, String error, float tiempo_objetivo){							//Layout principal para el display LCD
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

		bool cargar(){

			lcd.clear();  
			lcd.setCursor(0,0);             
			lcd.print("Datos encontrados.");          
			lcd.setCursor(0,2);             
			lcd.print("Desea cargar?");                  
			lcd.setCursor(0,3);
			lcd.print("  >NO          SI   ");

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

		void control_leds(float soc, float diferencia){							//Control de tira de LEDS y RGB 

			int led = map(soc,0.1,1,0,9);
			int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};

			for(int i = 0; i<=9; i++){
				digitalWrite(leds[i],LOW);
			}

			digitalWrite(led, HIGH);
			
		}
		
};
