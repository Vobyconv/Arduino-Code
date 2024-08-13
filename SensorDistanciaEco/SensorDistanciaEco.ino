#define trigPin 2
#define ecoPin 4
int rele = 12;

void setup() {
  Serial.begin(9600);
  pinMode(rele, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(ecoPin, INPUT);

}
void medir() {
  
long tiempo, distancia;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  tiempo = pulseIn(ecoPin, HIGH);
  distancia = (tiempo/2) / 29.1;
  Serial.print(tiempo);
  Serial.print(F(" - - "));
  Serial.println(distancia);
  
  int lejos = map(distancia, 2, 300, 0, 10);
  Serial.println(lejos);
  
  if (lejos < 5) {
  digitalWrite(rele, HIGH);
  Serial.println("Rele abierto");
  }
  else {
    delay(50);
  digitalWrite(rele, LOW);
  Serial.println(F("Rele cerrado"));
  }
}

void loop() {
  
  medir();
  delay(10);
}
