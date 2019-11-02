class LCD_Leds{
	
	private:
		
		#include <LiquidCrystal_I2C.h>
		#include <Wire.h>
		#include <Encoder.h>

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
		const int dt = 19;
		const int clk = 18;
		const int enable = 8;

		float verde = 0;
		float rojo = 0;
		float azul = 0;
		
		int est_encoder = 0;
		int ultimo_est_encoder = 0;
		
	public:
		
		float tiempo_objetivo = 0;
		bool cargar_datos = false;

		void setup(){
			
			/*---INICIO DE PROGRAMA---*/
			Serial.begin(9600);          		//Inicia una comunicacion serial para edicion de codigo

			/*---CONFIGURACION LCD---*/  
			LiquidCrystal_I2C lcd(0x27,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
			lcd.init();							//Inicio de la funcion lcd
			lcd.clear();							//Pone en blanco el lcd
			lcd.backlight();						//Enciende el backlight del display
			lcd.print("Cargando programa...");
			  
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

		}
		
		void datos (float tension, float corriente, float potencia, float energia, float soc, float corriente_objetivo, float velocidad){							//Layout principal para el display LCD
			LiquidCrystal_I2C lcd(0x27,20,4);   	//Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
			lcd.clear();							//Pone en blanco el lcd
			lcd.print("Cargando programa...");
		}

		void display (String palabra){			//Errores y otras cosas para mostrar en el display
			LiquidCrystal_I2C lcd(0x27,20,4);   	//Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
			lcd.clear();                        	//Limpia el LCD de todo lo que tenga escrito
			lcd.setCursor(0,0);                 	//Pone el cursor en el primer casillero de la primer fila
			lcd.print(palabra);                   	//Escribe el error donde debería estar el cursor   
			return;
		}

		bool cargar(){
			LiquidCrystal_I2C lcd(0x27,20,4);   	//Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
			lcd.clear();          
			lcd.setCursor(0,0);             
			lcd.print("¿Desea cargar?");                  
			lcd.setCursor(0,1);
			lcd.print("   NO          SI   ");

			Encoder encoder (dt,clk);

			while(true){
			
				est_encoder = encoder.read();

				if(est_encoder > ultimo_est_encoder){
					
					lcd.clear();          
					lcd.setCursor(0,0);             
					lcd.print("¿Desea cargar?");                  
					lcd.setCursor(0,1);
					lcd.print("   NO         >SI   ");

					cargar_datos = true;
				}

				if(est_encoder < ultimo_est_encoder){

					lcd.clear();          
					lcd.setCursor(0,0);             
					lcd.print("¿Desea cargar?");                  
					lcd.setCursor(0,1);
					lcd.print("  >NO          SI   ");
					
					cargar_datos = false;

				}
				
				ultimo_est_encoder = est_encoder;
				
				if(digitalRead(boton_encoder) == 0){
					if(cargar_datos){
						display("Cargando datos...");
						delay(500);
						}

					if(!cargar_datos){
						display("Comenzando de cero.");
						delay(500);
					}

					return cargar_datos;
				}

			}

		}
		
		float comenzar_prueba(){

			Encoder encoder (dt,clk);

			while(true){
				
				est_encoder = encoder.read();

				if(est_encoder != ultimo_est_encoder){
					display("Tiempo (min): " + est_encoder);
				}
			
				ultimo_est_encoder = est_encoder;

				if( digitalRead(boton_encoder) == 0 ){
					tiempo_objetivo = est_encoder;
				break;
			}

			}

			return tiempo_objetivo;

		}

		void leds(float soc, float diferencia){							//Control de tira de LEDS y RGB 

			int cantidad_leds = map(soc,0.1,1,0,9);
			  
			int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};
			  
			for(int i = 0; i<=9; i++){
				digitalWrite(leds[i],LOW);
			}
			
			for(int i = 0; i <= cantidad_leds; i++){
				digitalWrite(leds[i],HIGH);
			}

			if(diferencia >= 0){
				verde = map(diferencia,0,5,255,0);     	//Mapea la diferencia de potencia positiva (por debajo del objetivo)
				azul = 0;								  	//Deja el color restante en 0 para que no se muestre
				rojo = (255 - verde);                     	//Hace la diferencia entre el nivel de tension que deberia tener el led verde, y el restante se va al rojo
			}else{
				azul = map(diferencia, -5, 0, 255, 0);  	//Mapea la diferencia de potencia negativa (por arriba del objetivo)
				verde = 0;									//Deja el color restante en 0 para que no se muestre
				rojo = (255 - azul);                     	//Hace la diferencia entre el nivel de tension que deberia tener el led verde, y el restante se va al rojo
			}
			  
			analogWrite(RGB_R , rojo);                         //AnalogWrite correspondiente a los niveles de tension
			analogWrite(RGB_V , verde);
			analogWrite(RGB_A , azul);
		}
		
};
