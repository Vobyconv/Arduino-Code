#include <Servo.h>

int poten = A0;
int mediluz = A1;
int temp = A2;
Servo motor1, motor2, motor3, motor4;
#define trigPin 2
#define ecoPin 4

void setup() {
  Serial.begin(9600);
  motor1.attach(11);
  motor2.attach(10);
  motor3.attach(6);
  motor4.attach(5);
  
  pinMode(trigPin, OUTPUT);
  pinMode(ecoPin, INPUT);
  
  int poten = analogRead(A0);
  int mediluz = analogRead(A1);
  int temp = analogRead(A2);
}

void loop() {
  int poten = analogRead(A0);
  int mediluz = analogRead(A1);
  int temp = analogRead(A2);
  
  long tiempo, distancia;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  tiempo = pulseIn(ecoPin, HIGH);
  distancia = (tiempo/2) / 29.1;
      
  int potenpos = map(poten, 0, 1023, 0, 180);
  int medipos = map(mediluz, 6, 679, 180, 0);
  int temppos = map(temp, 20, 358, 180, 0);
  int distpos = map(distancia, 17, 300, 180, 0);
  
  Serial.print("{poten: ");
    Serial.print(poten);
    Serial.print(" - ");
    Serial.print(potenpos); 
  Serial.print("}  {luz: ");
    Serial.print(mediluz);
    Serial.print(" - ");
    Serial.print(medipos);
  Serial.print("}  {temp: ");
    Serial.print(temp);
    Serial.print(" - ");
    Serial.print(temppos); 
  Serial.print("}  {tiempo/dist: ");
    Serial.print(tiempo);
    Serial.print(" / ");
    Serial.print(distancia);
    Serial.print(" - ");
    Serial.print(distpos);
      Serial.println("}");
    
   motor1.write(potenpos);
   motor2.write(medipos);
   motor3.write(temppos);
   motor4.write(distpos);
      
   delay(50);
}
