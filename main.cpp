#include <Arduino.h>
#include <driver/adc.h>
#include "driver/ledc.h"
#include "config.h"

#define canalServoPWM 4
#define freqServoPWM 50
#define resServoPWM 10


#define canalLed1PWM 0
#define canalLed2PWM 1
#define canalLed3PWM 2
#define freqLedPWM 5000
#define resLedPWM 8


#define seg0 33
#define seg1 25
#define seg2 15
#define seg3 4
#define seg4 16
#define seg5 32
#define seg6 27
#define seg7 2
#define b1 39
#define sensor 34
#define ledG 14
#define ledY 12
#define ledR 13
#define PinServo 23
#define multiplex1 22
#define multiplex2 26
#define multiplex3 21


float temp = 0.0;
int indicador = 0;


volatile bool leerTemperaturaFlag = false;


int ledON = 255;
int ledOFF = 0;
int posServo1 = 51;
int posServo2 = 72;
int posServo3 = 102;


int displayCentenas[] = {seg0, seg1, seg2, seg3, seg4, seg5, seg6, seg7};
int displayDecenas[] = {seg0, seg1, seg2, seg3, seg4, seg5, seg6, seg7};
int displayUnidades[] = {seg0, seg1, seg2, seg3, seg4, seg5, seg6, seg7};




const int digitos[10][7] = {
  {1, 1, 1, 1, 1, 1, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1},  // 2
  {1, 1, 1, 1, 0, 0, 1},  // 3
  {0, 1, 1, 0, 0, 1, 1},  // 4
  {1, 0, 1, 1, 0, 1, 1},  // 5
  {1, 0, 1, 1, 1, 1, 1},  // 6
  {1, 1, 1, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1},  // 8
  {1, 1, 1, 1, 0, 1, 1}   // 9
};


void initPWM();
void IRAM_ATTR B1ISR();
float leerTemperatura();
void ledturnON(float temperatura);
void MovimientoServo(float temperatura);
void mostrarDigito(int digito, int display[], bool activarPuntoDecimal = false);
void display(float temperatura);


AdafruitIO_Feed *valor = io.feed("ProyectoED2");

void setup(){
  initPWM();
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);
  pinMode(sensor, INPUT);
  pinMode(seg0,OUTPUT);
  pinMode(seg1, OUTPUT);
  pinMode(seg2, OUTPUT);
  pinMode(seg3, OUTPUT);
  pinMode(seg4, OUTPUT);
  pinMode(seg5, OUTPUT);
  pinMode(seg6, OUTPUT);
  pinMode(seg7, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledY, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(PinServo, OUTPUT);
  pinMode(multiplex1, OUTPUT);
  pinMode(multiplex2, OUTPUT);
  pinMode(multiplex3, OUTPUT);


 attachInterrupt(b1, B1ISR, FALLING);


while(!Serial);

  Serial.print("Connecting to Adafruit IO");

  io.connect();

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());


}


void loop(){
    leerTemperaturaFlag = digitalRead(b1);

    if (leerTemperaturaFlag) {
    leerTemperaturaFlag = false;
    temp = leerTemperatura();
    
    }
    ledturnON(temp);
    MovimientoServo(temp);
    display(temp);

    io.run();
     Serial.print("Sending -> ");
    Serial.println(temp);
    valor->save(temp);*

     Serial.print("Temperatura en Celsius: ");
    Serial.println(temp);
}


void IRAM_ATTR B1ISR() {
   leerTemperaturaFlag = true;
   
}


float leerTemperatura(){
  int valorADC = analogRead(sensor);
  float voltaje = valorADC * (3.3 / 4095.0);
  float temperatura = voltaje * 100;
  return temperatura;
}


void ledturnON(float temperatura){
  if (temperatura < 37){
  ledcWrite(canalLed1PWM, ledON);
  ledcWrite(canalLed2PWM, ledOFF);
  ledcWrite(canalLed3PWM, ledOFF);
  }
  else if (temperatura > 37 && temperatura < 37.5){
  ledcWrite(canalLed1PWM, ledOFF);
  ledcWrite(canalLed2PWM, ledON);
  ledcWrite(canalLed3PWM, ledOFF);
  }
  else{
  ledcWrite(canalLed1PWM, ledOFF);
  ledcWrite(canalLed2PWM, ledOFF);
  ledcWrite(canalLed3PWM, ledON);
  }
}


void MovimientoServo(float temperatura){
  if (temperatura < 37){
  ledcWrite(canalServoPWM, posServo1);
  }
  else if (temperatura > 37 && temperatura < 37.5){
  ledcWrite(canalServoPWM, posServo2);
  }
  else{
  ledcWrite(canalServoPWM, posServo3);
  }


}




void mostrarDigito(int digito, int display[], bool activarPuntoDecimal){
  for(int i = 0; i < 7; i++){
    digitalWrite(display[i], digitos[digito][i]);
  }
  digitalWrite(display[7], activarPuntoDecimal ? HIGH : LOW);
}


void display(float temperatura){
  int newtemp = temperatura * 10;
  int centenas = newtemp / 100;
  int decenas = (newtemp / 10) % 10;
  int unidades = newtemp % 10;


  digitalWrite(multiplex1, HIGH);
  mostrarDigito(centenas, displayCentenas,false);
  delay(5);
  digitalWrite(multiplex1, LOW);


  digitalWrite(multiplex2, HIGH);
  mostrarDigito(decenas, displayDecenas, true);
  delay(5);
  digitalWrite(multiplex2, LOW);


  digitalWrite(multiplex3, HIGH);
  mostrarDigito(unidades, displayUnidades);
  delay(5);
  digitalWrite(multiplex3, LOW);
 
}


void initPWM() {
  // Configurar el canal PWM para el servo
  ledcSetup(canalServoPWM, freqServoPWM, resServoPWM);
  // Asignar el pin del servo al canal PWM
  ledcAttachPin(PinServo, canalServoPWM);
  // Inicializar la posición del servo
  ledcWrite(canalServoPWM, posServo1);


  // Configurar los canales PWM para los LEDs
  ledcSetup(canalLed1PWM, freqLedPWM, resLedPWM);
  ledcSetup(canalLed2PWM, freqLedPWM, resLedPWM);
  ledcSetup(canalLed3PWM, freqLedPWM, resLedPWM);


  // Asignar los pines de los LEDs a sus canales PWM correspondientes
  ledcAttachPin(ledG, canalLed1PWM);
  ledcAttachPin(ledY, canalLed2PWM);
  ledcAttachPin(ledR, canalLed3PWM);


  // Inicializar las intensidades de los LEDs a cero
  ledcWrite(canalLed1PWM, ledOFF);
  ledcWrite(canalLed2PWM, ledOFF);
  ledcWrite(canalLed3PWM, ledOFF);
}