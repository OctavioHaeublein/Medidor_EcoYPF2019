#include <LiquidCrystal_I2C.h>
#define PI 3.1415926535897932384626433832795
#define pwm_acelerador 9
#define sensor_inductivo 3

/*--VARAIBLES ACELERADOR---*/
float entrada_acelerador = 0;
float salida_acelerador = 0;
float nivel_acelerador = 20;
char estado_acelerador = 'm';

/*--VARIABLES VELOCIDAD---*/
float tiempo = 0;
int contador = 0;
float tiempo_previo_velocidad = 0;
float revoluciones = 0;
float frecuencia = 0;

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

long tension_hall = 0;
long tension_divisor = 0;



LiquidCrystal_I2C lcd(0x27,20,4);   //Crea el obtejo "lcd" y define la dirección de comunicación, y tamaño (20x4)

void setup() {
  Serial.begin(9600);
  pinMode(pwm_acelerador,OUTPUT);
  pinMode(sensor_inductivo, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode (A1, INPUT);
  
  /*---CONFIGURACION LCD---*/  
  lcd.init();						//Inicio de la funcion lcd
  lcd.clear();						//Pone en blanco el lcd
  lcd.backlight();					//Enciende el backlight del display
  lcd.setCursor(0,0);
  lcd.print("Hola");
  attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
}

void loop() {
	
	if(Serial.available() > 0){
		estado_acelerador = Serial.read();
	}
	   
	//medidor();
	//acelerador();
	calcular_velocidad();
	
}

void medidor(){
	
  /*---LECTURA DE SENSORES---*/
  
  for(int i = 0; i < 5; i++){													//Se realizan 5 lecturas seguidas y se promedia
  tension_hall    += analogRead(A1);
  tension_divisor += analogRead(A0);
  }
  
  /*---CALCULO DE TENSION, CORRIENTE, POTENCIA Y ENERGIA---*/
  //tension_divisor =  map(tension_divisor, 7611.45,9207.79,38.4,51.4);  		//Mapea los niveles de tension recibidos de 0 a 1023, para 0 a 60v
  
  tension_divisor = (tension_divisor/5);
  
  tension_divisor = ( (tension_divisor * (r1 + r2)) / r2 );
  
  tension_divisor = interpolar (tension_divisor, 7611.45, 9207.79, 38.4, 51.4);  
  
  tension_hall    = (tension_hall / 10); 										//Mapea los niveles de tension medidos por el sensor hall
  
  if(tension_hall >= 525){
    
    //corriente = map(tension_hall, 525,1023, 0, 70);				//Si la circulacion de corriente es positiva, la mapea asi
    tension_hall 	  = interpolar (tension_hall, 525, 1023, 0, 70);
	
  }else{if(tension_hall < 525){
    
      //corriente = -(map(tension_hall, 525, 1023, 0, 70));			//Si la circulacion de corriente es contraria (cargando bateria) se expresa negativa
	  tension_hall 	  = interpolar (tension_hall, 0, 525, 0, -70);
	  
	}
   }
   
    Serial.print("Corriente: ");
	Serial.println(tension_hall);
	Serial.println(analogRead(A1));
	Serial.print("Tension: ");
	Serial.println(tension_divisor);
	Serial.println(analogRead(A0));
	Serial.println("");
	tension_divisor = 0;
	delay(1000);

}

long interpolar (long x, long in_min, long in_max, long out_min,long out_max){
	return ( (x-in_min)*(out_max - out_min) + (out_min * (in_max - in_min)) ) / (in_max - in_min);
}

void acelerador(){
	
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

void calculo_pid(){
	
	tiempo = millis();
	
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

void calcular_velocidad(){
	
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
		
		float velocidad = (((PI * 500 * frecuencia)/1000)*3.6);				//Calcula la velocidad de la rueda (Km/h)
		frecuencia = 0;
		contador = 0;
		
		Serial.println("Revoluciones");
		Serial.println(revoluciones);
		Serial.println("");
		Serial.println("Velocidad: ");
		Serial.println(velocidad);
		Serial.println("");
		
		attachInterrupt(digitalPinToInterrupt(sensor_inductivo), blink, FALLING);
	}
}

void blink(){
	revoluciones++;
	Serial.println(revoluciones);
}
