// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

//Ordenes basicas ---------------------
int tiraEyes = 10;
int rele = 12;
int puntosEyes = 12;
int sensibilidad = 4;
int tope = 20;
int limiteChillido = 48;
int agudo =1100;
int tiempoChillido = 4;

Adafruit_NeoPixel stripBat = Adafruit_NeoPixel(puntosEyes, tiraEyes, NEO_GRB + NEO_KHZ800);

//pines de murcielagos
int batSensor0 = A0;
int batSensor1 = A1;
int batSensor2 = A2;
int batSensor3 = A3;
int batSensor4 = A4;
int batSensor5 = A5;

//chillidos
int batSound0 = 2;

//puntitos de Ojos murcielago
const int numLeds = 2;
const int batLed0[numLeds] = {0, 1};
const int batLed1[numLeds] = {2, 3};
const int batLed2[numLeds] = {4, 5};
const int batLed3[numLeds] = {6, 7};
const int batLed4[numLeds] = {8, 9};
const int batLed5[numLeds] = {10, 11};

//Contadores de luz
int batLight0 = 0;
int batLight1 = 0;
int batLight2 = 0;
int batLight3 = 0;
int batLight4 = 0;
int batLight5 = 0;

//Contadores de victoria

int solucionNum[] = { 1, 2, 3, 4, 5, 6};
int openRele[] = { true, true, true, true, true, true,};


//inicio de tioras led
void iniled() {

  stripBat.begin();
  stripBat.setBrightness(10);
  stripBat.show();
  stripBat.clear();
}

// Funciones de murcielagos
//-- Murcielago 0

void bat_0_dark(){
  int chillido0 = random(0, 50);
  Serial.print(F(" / Chillido: "));Serial.print(chillido0);
  if (chillido0 >= 40){
   for (int i = 0; i <= numLeds ; i++) {
      stripBat.setPixelColor(batLed0[i], random(100), 0, 0);
      stripBat.show();
   }
  }
  else{
  for (int i = 0; i <= numLeds ; i++) {
      stripBat.setPixelColor(batLed0[i], 0, 0, 0);
      stripBat.show();
   }
  }
}

void bat_0_light(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripBat.setPixelColor(batLed0[i], 200, 200, 0);
      stripBat.show();
  }
}

void bat_0_bright(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripBat.setPixelColor((batLed0[i]), 0, 0, 200);
      stripBat.show();
  }
}


// Bat 0  --------------------------------------------
void bat0() {
    int batSensor0 = analogRead(A0);
    int batLevel = map(batSensor0, 0, 700, 0, 10);
       
      Serial.print(F("/Murci 0: ")); Serial.print(batSensor0);
        Serial.print(F(" / medida: ")); Serial.print(batLevel);
        Serial.print(F(" / Estado: "));Serial.print(batLight0);
   
   int chillido0 = random(0, 50);
   Serial.print(F(" / Chillido: "));Serial.print(chillido0);
   if (chillido0 >= limiteChillido){
   tone(batSound0, agudo, tiempoChillido);
   }
  
   if (batLevel >= sensibilidad) {
    Serial.print(F(" / UP / medidor luz:  ")); Serial.print(batLight0);
    bat_0_light(); 
    batLight0 = batLight0+1;
    tone(batSound0, agudo+batLight0*50, tiempoChillido*3);
    }
    else {
    Serial.print(F(" /  DOWN / medidor luz:  "));  
    }
  
  if (batLight0 >= tope) {
    Serial.println(" /  bat Bright /"); Serial.print(batLight0);
    bat_0_bright();
    solucionNum[0] = true;
    }
  
    else {
    bat_0_dark();
    Serial.println(F(" /  bat Evil /"));
    solucionNum[0] = false;   
    }
  
    Serial.println(F("/"));
   
}


void pinM(){
  pinMode(rele, OUTPUT);
}

void win() {
  digitalWrite(rele, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
}

void releOpen() {
  int openAll = solucionNum[0]+solucionNum[1]+solucionNum[2]+solucionNum[3];
  Serial.print("openAll: ");
  Serial.println(openAll);
  int total = 5;
  int exito = 0;
  for (int i = 0; i <= total-1 ; i++) {
    if (solucionNum[i] == openRele[i]) {
    exito++;
    Serial.print(i);
    Serial.println(" // Coincidencia //");
  }
  else {
  Serial.print(i);
  Serial.println(" // Falla //");
  }
  
  }
  if (exito == total) {
    win();
    Serial.println("Rele abierto");
  }
  else {
  Serial.println("Rele cerrado");
  }
}

void noche() {
  for (int i = 0; i <= puntosEyes ; i++) {
      stripBat.setPixelColor(i, 1, 200,10);
      stripBat.show();
     }
  for (int i = 0; i <= puntosEyes ; i++) {
      stripBat.setPixelColor(i, 1, 200, 10);
      stripBat.show();
     }
  Serial.println("+Eyes Led+ OK:");
 }

void murcielagos() {
  bat0();

  }

void setup()
{
  Serial.begin(9600);
  Serial.println("++Starting Murcielagos++");
  iniled();
  pinMode(batSound0,OUTPUT);
  //noche();
 
}

void loop()
{
// Miradas
  murcielagos();
  //releOpen();
  delay(50);
}
