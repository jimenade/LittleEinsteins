#include <Arduino_FreeRTOS.h>
//#include <PIDController.hpp>

#include <Arduino.h>
#include "FastLED.h"
#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

#define LEFT   A2
#define MIDDLE A1
#define RIGHT  A0
#define TRIG_PIN 13  
#define ECHO_PIN 12  
// Enable/Disable motor control.
//  HIGH: motor control enabled
//  LOW: motor control disabled
#define PIN_Motor_STBY 3

// Group A Motors (Right Side)
// PIN_Motor_AIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_AIN_1 7
// PIN_Motor_PWMA: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMA 5

// Group B Motors (Left Side)
// PIN_Motor_BIN_1: Digital output. HIGH: Forward, LOW: Backward
#define PIN_Motor_BIN_1 8
// PIN_Motor_PWMB: Analog output [0-255]. It provides speed.
#define PIN_Motor_PWMB 6

#define LEFT_OK 800
#define RIGHT_OK 800
#define MIDDLE_OK 915
#define baseSpeed 120

volatile int distance; 
volatile long output; 
volatile int state; 
volatile long left; 
volatile long middle; 
volatile long right; 
volatile double error_total = 0; 
// Constantes del controlador
double Kp=0.05722;
double Kd=0.011;
double Ki= 0.0004;
String sendBuff = "";
// Crear el controlador PID
//PIDController pid(Kp, Ki, Kd);

double error = 0;
double lastError = 0;
double integral = 0;


void setup() {
  Serial.begin(9600);
  pinMode(PIN_Motor_STBY,OUTPUT);
  pinMode(PIN_Motor_AIN_1,OUTPUT);
  pinMode(PIN_Motor_PWMA,OUTPUT);
  pinMode(PIN_Motor_BIN_1,OUTPUT);
  pinMode(PIN_Motor_PWMB,OUTPUT);

  pinMode(LEFT,INPUT) ;
  pinMode(MIDDLE,INPUT) ;
  pinMode(RIGHT,INPUT) ;

  pinMode(TRIG_PIN, OUTPUT); //pin como salida
  pinMode(ECHO_PIN, INPUT);  //pin como entrada
  digitalWrite(TRIG_PIN, LOW);//Inicializamos el pin con 0
  
  // Inicializar el controlador PID
  //pid.begin();
  
  // Crear las tareas
  //xTaskCreate(TaskReadSensors, "ReadSensors", 100, NULL, 0, NULL);
  //xTaskCreate(TaskComputePID, "ComputePID", 100, NULL, 2, NULL);
  //xTaskCreate(TaskPing, "SendPing", 100, NULL, 1, NULL);
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);

  // Set Red Color to LED
  FastLED.showColor(Color(255, 0, 0));
  
  
  while(1) {

    if (Serial.available()) {

      char c = Serial.read();
      sendBuff += c;
      
      if (c == '}')  {            
        Serial.print("Received data in serial port from ESP32: ");
        Serial.println(sendBuff);

        // Set Red Green to LED
        FastLED.showColor(Color(0, 255, 0));
        sendBuff = "";
        break;
      } 

    }
  }



  
}

void loop() {
  calcularDistancia();
  if(distance < 8){
    digitalWrite(PIN_Motor_STBY, LOW);
    if (Serial.available()) {
      calcularDistancia();
      Serial.println(1);
      

      Serial.println(distance);
      delay(800);
      
      

    }
    exit(0);
  }
  else{

    // // Leer los valores de los sensores
    int left = analogRead(LEFT);
    int middle = analogRead(MIDDLE);
    int right = analogRead(RIGHT);
    // if (middle < 60){
    //   output = (-1* output) + 10;
    // }
    // else{
      // Calcular la posición de la línea
    double posicion = -1 * left + 1 * right;
    double error_central = 800 - middle;
    if (error_central < 0){
      error_central = 0; 
    }
    // Calcular el error
    double error_lateral = 0 - posicion;
    if (error_lateral < 0){
      error_total = error_lateral - error_central;
    }
    if (error_lateral >= 0){
      error_total = error_lateral + error_central;
    }
    
    integral += error_total;
    double derivative = error_total - lastError;

    // if (-20<= error <= 20 ){
      
    //   output = 0; 
    // }
    // if (error > 20){
      
    //   // Calcular la salida del controlador PID
    //   //double output = Kp * error + Ki * integral + Kd * derivative;
    //   output = Kp * error + + Kd * derivative;
    //   // Guardar el error actual para la próxima iteración
    //   lastError = error;
    // }
    // if (error < -20){
      
    //   // Calcular la salida del controlador PID
    //   //double output = Kp * error + Ki * integral + Kd * derivative;
    //   output = Kp * error + + Kd * derivative;
    //   // Guardar el error actual para la próxima iteración
    //   lastError = error;
    // }
    output = Kp * error_total + + Ki * integral + Kd * derivative;
    //   // Guardar el error actual para la próxima iteración
    lastError = error_total;
  
    int speedRight = baseSpeed + output;
    int speedLeft = baseSpeed - output;
    

    speedRight = max(speedRight, 0);
    speedLeft = max(speedLeft, 0);
    
    digitalWrite(PIN_Motor_STBY, HIGH);
    digitalWrite(PIN_Motor_AIN_1, HIGH);
    digitalWrite(PIN_Motor_BIN_1, HIGH);
    analogWrite(PIN_Motor_PWMA, speedRight);
    analogWrite(PIN_Motor_PWMB, speedLeft);
  }

}

void TaskPing(void *pvParameters) {
  TickType_t xLastWakeTime, aux;
  while(1){
    left = analogRead(LEFT);
    middle = analogRead(MIDDLE);
    right = analogRead(RIGHT);
  }
  xTaskDelayUntil( &xLastWakeTime, 100 );
}
void calcularDistancia(){
  long t; //timepo que demora en llegar el eco
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(TRIG_PIN, LOW);
  
  t = pulseIn(ECHO_PIN, HIGH); //obtenemos el ancho del pulso
  distance = t/59;             //es
}

// void TaskControlMotors(void *pvParameters) {

//   int speedRight = baseSpeed + output;
//   int speedLeft = baseSpeed - output;
//   Serial.println(output);
//   Serial.println(speedRight);
//   Serial.println(speedLeft);
//   speedRight = max(speedRight, 0);
//   speedLeft = max(speedLeft, 0);
//   //digitalWrite(PIN_Motor_STBY, HIGH);
//   //digitalWrite(PIN_Motor_AIN_1, HIGH);
//   //digitalWrite(PIN_Motor_BIN_1, HIGH);
  
//   // Envía las velocidades a los motores
//   //analogWrite(PIN_Motor_PWMA, speedRight);
//   //analogWrite(PIN_Motor_PWMB, speedLeft);
// }



void TaskComputePID(void *pvParameters) {
  TickType_t xLastWakeTime, aux;
  while(1){
    calcularDistancia();
    if(distance < 8){
      digitalWrite(PIN_Motor_STBY, LOW);
      if (Serial.available()) {

        
        Serial.println(1);
        delay(1000);
        
        

      }
      exit(0);
    }
    else{

      // // Leer los valores de los sensores
      int left = analogRead(LEFT);
      int middle = analogRead(MIDDLE);
      int right = analogRead(RIGHT);
      // if (middle < 60){
      //   output = (-1* output) + 10;
      // }
      // else{
        // Calcular la posición de la línea
      double posicion = -1 * left + 1 * right;
      double error_central = 800 - middle;
      if (error_central < 0){
        error_central = 0; 
      }
      // Calcular el error
      double error_lateral = 0 - posicion;
      if (error_lateral < 0){
        error_total = error_lateral - error_central;
      }
      if (error_lateral >= 0){
        error_total = error_lateral + error_central;
      }
      
      integral += error_total;
      double derivative = error_total - lastError;

      // if (-20<= error <= 20 ){
        
      //   output = 0; 
      // }
      // if (error > 20){
       
      //   // Calcular la salida del controlador PID
      //   //double output = Kp * error + Ki * integral + Kd * derivative;
      //   output = Kp * error + + Kd * derivative;
      //   // Guardar el error actual para la próxima iteración
      //   lastError = error;
      // }
      // if (error < -20){
       
      //   // Calcular la salida del controlador PID
      //   //double output = Kp * error + Ki * integral + Kd * derivative;
      //   output = Kp * error + + Kd * derivative;
      //   // Guardar el error actual para la próxima iteración
      //   lastError = error;
      // }
      output = Kp * error_total + + Ki * integral + Kd * derivative;
      //   // Guardar el error actual para la próxima iteración
      lastError = error_total;
   
      int speedRight = baseSpeed + output;
      int speedLeft = baseSpeed - output;
      

      speedRight = max(speedRight, 0);
      speedLeft = max(speedLeft, 0);
      
      digitalWrite(PIN_Motor_STBY, HIGH);
      digitalWrite(PIN_Motor_AIN_1, HIGH);
      digitalWrite(PIN_Motor_BIN_1, HIGH);
      analogWrite(PIN_Motor_PWMA, speedRight);
      analogWrite(PIN_Motor_PWMB, speedLeft);
    }
    xTaskDelayUntil( &xLastWakeTime, 50 );
  }
  
  
  
}