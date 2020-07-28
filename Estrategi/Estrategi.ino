int poten1 = A1;
int poten2 = A2;
int poten3 = A3;
int poten4 = A4;
int poten5 = A5;

int rele = 2;

int valor1 = 0;
int valor2 = 0;
int valor3 = 0;
int valor4 = 0;
int valor5 = 0;

int victoria = valor1 + valor2 + valor3 + valor4 + valor5;


void inipoten(){
  int poten1 = analogRead(A1);
  int poten2 = analogRead(A2);
  int poten3 = analogRead(A3);
  int poten4 = analogRead(A4);
  int poten5 = analogRead(A5);
}


void setup()
{
  Serial.begin(9600);
  Serial.println("Iniciando Quiditch Estrategia ...");
  pinMode (rele, OUTPUT);
  pinMode (13, OUTPUT);
 
  inipoten();
  
  }


void loop(){
  
  //int victoria = valor1 + valor2 + valor3 + valor4 + valor5;
  
  int poten1 = analogRead(A1);
  int poten2 = analogRead(A2);
  int poten3 = analogRead(A3);
  int poten4 = analogRead(A4);
  int poten5 = analogRead(A5);
  
  Serial.print("{origen: ");
  Serial.print(poten1);
    Serial.print(" - ");
  Serial.print(poten2);
    Serial.print(" - ");
  Serial.print(poten3);
    Serial.print(" - ");
  Serial.print(poten4);
    Serial.print(" - ");
  Serial.print(poten5);
    Serial.println(" }");
  
  int potenled1 = map(poten1, 0, 1023, 1, 4);
  int potenled2 = map(poten2, 0, 1023, 1, 4);
  int potenled3 = map(poten3, 0, 1023, 1, 4);
  int potenled4 = map(poten4, 0, 1023, 1, 4);
  int potenled5 = map(poten5, 0, 1023, 1, 4);
  
  int victoria = valor1 + valor2 + valor3 + valor4 + valor5;
    
  Serial.print("{poten:   ");
  Serial.print(potenled1);
    Serial.print(" - ");
  Serial.print(potenled2);
    Serial.print(" - ");
  Serial.print(potenled3);
    Serial.print(" - ");
  Serial.print(potenled4);
    Serial.print(" - ");
  Serial.print(potenled5);
    Serial.println(" }");
  
  int solucion[6] = { 1, 1, 2, 3, 3};
  
  Serial.print("Solucion: ");
  Serial.print(solucion[0]);
    Serial.print(" - ");
  Serial.print(solucion[1]);
    Serial.print(" - ");
  Serial.print(solucion[2]);
    Serial.print(" - ");
  Serial.print(solucion[3]);
    Serial.print(" - ");
  Serial.println(solucion[4]);
  
  Serial.print("Coincidencias: ");
  
    if (potenled1 == solucion[0]){valor1 = 1;}
        else {valor1 = 0;}
        Serial.print(valor1);
    if (potenled2 == solucion[1]){valor2 = 1;}
        else {valor2 = 0;}
     Serial.print(valor2);
    if (potenled3 == solucion[2]){valor3 = 1;}
        else {valor3 = 0;}
      Serial.print(valor3); 
    if (potenled4 == solucion[3]){valor4 = 1;}
        else {valor4 = 0;}
      Serial.print(valor4);
    if (potenled5 <= solucion[4]){valor5 = 1;}
        else {valor4 = 0;}
      Serial.print(valor5);
        Serial.print("  // Puntuacion de victoria: ");
        Serial.println(victoria);
  
  Serial.print("Situacion del rele ");
  
  if (victoria == 5){
    digitalWrite(rele, HIGH);
    digitalWrite(13, HIGH);
    Serial.println(" /OPEN");
  }
  else {Serial.println(" /CLOSE");
  digitalWrite(rele, LOW);
  digitalWrite(13, LOW);
       }
  Serial.println();
      delay(2000);
}
