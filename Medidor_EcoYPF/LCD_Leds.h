Class LCD_Leds{
	Private:
		#include <LiquidCrystal_I2C.h>

		#define LA1 34
		#define LA2 36
		#define LA3 38

		#define LR1 40
		#define LR2 42

		#define LV1 43
		#define LV2 41
		#define LV3 39
		#define LV4 37
		#define LV5 35
		  
		#define RGB_R 4
		#define RGB_V 5
		#define RGB_A 6
		
		float verde = 0;
		float rojo = 0;
		float azul = 0;
		
		LiquidCrystal_I2C lcd(0x27,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)
		
	Public:
		
		void setup(){
			
			/*---INICIO DE PROGRAMA---*/
			Serial.begin(9600);          		//Inicia una comunicacion serial para edicion de codigo
			  
			/*---CONFIGURACION LCD---*/  
			lcd.init();						//Inicio de la funcion lcd
			lcd.clear();						//Pone en blanco el lcd
			lcd.backlight();					//Enciende el backlight del display
			  
			/*---DIGITAL OUTPUTS---*/
			pinMode(LR1 , OUTPUT);   			//LED Rojo1
			pinMode(LR2 , OUTPUT);   			//LED Rojo
			pinMode(LA1 , OUTPUT);   			//LED Amarillo
			pinMode(LA2 , OUTPUT);   			//LED Amarillo1
			pinMode(LA3 , OUTPUT);   			//LED Amarillo2
			pinMode(LV1 , OUTPUT);   			//LED Verde
			pinMode(LV2 , OUTPUT);   			//LED Verde1
			pinMode(LV3 , OUTPUT);   			//LED Verde2
			pinMode(LV4 , OUTPUT);   			//LED Verde3
			pinMode(LV5 , OUTPUT);   			//LED Verde4
			  
			/*---DIGITAL PWM OUTPUTS---*/
			pinMode(RGB_R , OUTPUT);    		//RGB Rojo
			pinMode(RGB_V , OUTPUT);    		//RGB Verde
			pinMode(RGB_A , OUTPUT);    		//RGB Azul
			
		}
		
		void LCD_datos (){							//Layout principal para el display LCD
			lcd.setCursor(0,0);                 	//Pone el cursor en el primer casillero de la segunda fila
			lcd.print("Hola Mundo");            	//Escribe eso donde debería estar el cursor      
		}

		void LCD_display (String palabra){			//Errores y otras cosas para mostrar en el display
			lcd.clear();                        	//Limpia el LCD de todo lo que tenga escrito
			lcd.setCursor(0,0);                 	//Pone el cursor en el primer casillero de la primer fila
			lcd.print(palabra);                   	//Escribe el error donde debería estar el cursor   
			return;
		}
	
		void leds(){							//Control de tira de LEDS y RGB 

			int cantidad_leds = map(SOC,0.1,1,0,9);
			  
			int leds[] = {LR1,LR2,LA1,LA2,LA3,LV1,LV2,LV3,LV4,LV5};
			  
			for(int i = 0; i<=9; i++){
				digitalWrite(leds[i],LOW);
			}
			
			for(int i = 0; i <= cantidad_leds; i++){
				digitalWrite(leds[i],HIGH);
			}
			  
			float diferencia = (corriente_objetivo - corriente);
			  
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
		
}