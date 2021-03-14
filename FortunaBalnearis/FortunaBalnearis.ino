//Basicos
int poten1 = A0;
int poten2 = A1;
int rele = 11;

//int roscaIz = 0;
//int roscaDe = 0;

void blink() {
  digitalWrite(rele, HIGH);
  Serial.println("open");
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(rele, LOW);
  Serial.println("close");
  delay(1000); // Wait for 1000 millisecond(s)
}

void posiciones() {
  
int poten1 = analogRead(A0);
int poten2 = analogRead(A1);
  
int roscaIz = map(poten1, 0, 1023, 1, 6);
int roscaDe = map(poten2, 0, 1023, 1, 6);
  
Serial.print("{posicion:  Izq: ");
 Serial.print(roscaIz);
 Serial.print(" - Der: ");
 Serial.print(roscaDe);
 Serial.println(" }");
}

void victoriaP(){

int poten1 = analogRead(A0);
int poten2 = analogRead(A1);
  
int roscaIz = map(poten1, 0, 1023, 1, 6);
int roscaDe = map(poten2, 0, 1023, 1, 6);
  
Serial.print("{posicion:  Izq: ");
 Serial.print(roscaIz);
 Serial.print(" - Der: ");
 Serial.print(roscaDe);
 Serial.println(" }");


  Serial.print("Vitoria  :");
  int solucion[2] = {2,4}; //soluciones {izquierda , derecha}
  int valorIz = 0;
  int valorDe = 0;
    
  if (roscaIz == solucion[0]) {
   valorIz = 5;
   Serial.print(roscaIz);
   Serial.print(" / bien    ");
  }
  else {valorIz = 0;
    Serial.print(roscaIz);
    Serial.print(" mal    ");
  }
  
  if (roscaDe == solucion[1]) {
   valorDe = 5;
   Serial.print(roscaDe);
   Serial.println(" / bien");
  }
  else {valorDe = 0;
    Serial.print(roscaDe);
    Serial.println(" mal");
  }
int completo = valorIz+valorDe;
  
  if (completo == 10) {
   blink();
  }
  else {}
  
}



void setup(){
  Serial.begin(9600);
  Serial.println("Fortuna Balnearis SPQR...");
  pinMode (rele, OUTPUT);
}

void loop(){
victoriaP();
//blink();
  
}
