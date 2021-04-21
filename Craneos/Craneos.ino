// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>


//Ordenes basicas ---------------------
int tiraCraneos = 11;
int tiraCraneos2 = 10;
int rele = 12;
int puntosCraneos = 44;
int puntosCraneos2 = 44;
int sensibilidad = 6;

//pines de pulsadores
int sensorC0 = A0;
int sensorC1 = A1;
int sensorC2 = A2;
int sensorC3 = A3;
int sensorC4 = A4;
int sensorC5 = A5;
int sensorC4 = A6;
int sensorC5 = A7;

Adafruit_NeoPixel stripCraneos = Adafruit_NeoPixel(puntosCraneos, tiraCraneos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCraneos2 = Adafruit_NeoPixel(puntosCraneos2, tiraCraneos2, NEO_GRB + NEO_KHZ800);


//puntitos de Calaveras
const int numLeds = 7;
const int craneoLed0[numLeds] = { 5, 6, 7, 8, 9, 10, 11};
const int craneoLed1[numLeds] = { 16, 17, 18, 19, 20, 21, 22};
const int craneoLed2[numLeds] = { 27, 28, 29, 30, 31, 32, 33};
const int craneoLed3[numLeds] = { 38, 39, 40, 41, 42, 43, 44};
const int craneoLed4[numLeds] = { 5, 6, 7, 8, 9, 10, 11};
const int craneoLed5[numLeds] = { 16, 17, 18, 19, 20, 21, 22};
const int craneoLed6[numLeds] = { 27, 28, 29, 30, 31, 32, 33};
const int craneoLed7[numLeds] = { 38, 39, 40, 41, 42, 43, 44};

// Funciones de constelaciones

//-- craneo 0
void Constel_0_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed0[i], 10, 250, 10);
      stripCraneos.show();
  }
      Serial.print(" / * TrollLED - ON * / ");
}

void Constel_0_off(){
  int orden = 0;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed0[i], 200, 200, 200);
      stripCraneos.show();
  }
      Serial.print(" / * TrollLED - OFF * / ");
}

//--
void Constel_1_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed1[i], 10, 250, 10);
      stripCraneos.show();
  }
      Serial.print(" / * VacaLED - ON * / ");
}

void Constel_1_off(){
  int orden = 1;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed1[i], 200, 200, 200);
      stripCraneos.show();
  }
      Serial.print(" / * VacaLED - OFF * / ");
}
//--
void Constel_2_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed2[i], 10, 250, 10);
      stripCraneos.show();
  }
      Serial.print(" / * OsoLED - ON * / ");
}

void Constel_2_off(){
  int orden = 2;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed2[i], 200, 200, 200);
      stripCraneos.show();
  }
      Serial.print(" / * OsoLED - OFF * / ");
}
//--
void Constel_3_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed3[i], 10, 250, 10);
      stripCraneos.show();
  }
      Serial.print(" / * MurteLed - ON * / ");
}

void Constel_3_off(){
  int orden = 3;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos.setPixelColor(craneoLed3[i], 200, 200, 200);
      stripCraneos.show();
  }
      Serial.print(" / * MuerteLED - OFF * / ");
}
//--
void Constel_4_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed4[i], 10, 250, 10);
      stripCraneos2.show();
  }
      Serial.print(" / * FocaLed - ON * / ");
}

void Constel_4_off(){
  int orden = 4;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed4[i], 200, 200, 200);
      stripCraneos2.show();
  }
      Serial.print(" / * FocaLED - OFF * / ");
}

//--
void Constel_5_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed5[i], 10, 250, 10);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLed - ON * / ");
}

void Constel_5_off(){
  int orden = 5;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed5[i], 200, 200, 200);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLED - OFF * / ");
}

//--
void Constel_6_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed6[i], 10, 250, 10);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLed - ON * / ");
}


void Constel_6_off(){
  int orden = 6;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed6[i], 200, 200, 200);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLED - OFF * / ");
}

//--
void Constel_7_on(){
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed7[i], 10, 250, 10);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLed - ON * / ");
}


void Constel_7_off(){
  int orden = 7;
  for (int i = 0; i <= numLeds-1 ; i++) {
      stripCraneos2.setPixelColor(craneoLed7[i], 200, 200, 200);
      stripCraneos2.show();
  }
      Serial.print(" / * BallenaLED - OFF * / ");
}





//Contadores de victoria
int solucion = 2;
int solucionNum[] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[] = { true, true, false, false, false, false, false, false};
int openAll = 0;

//inicio de tioras led
void iniled() {

  stripCraneos.begin();
  stripCraneos.setBrightness(10);
  stripCraneos.show();
  stripCraneos.clear();
  
  stripCraneos2.begin();
  stripCraneos2.setBrightness(10);
  stripCraneos2.show();
  stripCraneos2.clear();
}


// Craneo 0  --------------------------------------------
void craneo0() {
  int orden = 0;
  Serial.print("++Nivel Cr0 : ");
    int sensorC0 = analogRead(A0);
    int c0Level = map(sensorC0, 0, 900, 10, 0);
       
        Serial.print(sensorC0);
        Serial.print("/ medida: ");
        Serial.print(c0Level);
        
    if (c0Level >= sensibilidad) {
    Serial.print(" / * C0 - UP * /");
    Constel_0_on();
    solucionNum[0] = true;
    }
    else {
     Serial.print(" / * C0 - DOWN * /");
     Constel_0_off();
     solucionNum[0] = false;   
    }
Serial.println(" - C0 OVER - //");
}

// Craneo 1  --------------------------------------------
void craneo1() {
  int orden = 1;
  Serial.print("++Nivel Cr1 : ");
    int sensorC1 = analogRead(A1);
    int c1Level = map(sensorC1, 0, 900, 10, 0);
       
        Serial.print(sensorC1);
        Serial.print("/ medida: ");
        Serial.print(c1Level);
        
    if (c1Level >= sensibilidad) {
    Serial.print(" / * C1 - UP * /");
    Constel_1_on();
    solucionNum[1] = true;
    }
    else {
     Serial.print(" / * C1 - DOWN * /");
     Constel_1_off();
     solucionNum[1] = false;   
    }
Serial.println(" - C1 OVER - //");
}

// Craneo 2  --------------------------------------------
void craneo2() {
  int orden = 2;
  Serial.print("++Nivel Cr2 : ");
    int sensorC2 = analogRead(A2);
    int c2Level = map(sensorC2,  0, 900, 10, 0);
       
        Serial.print(sensorC2);
        Serial.print("/ medida: ");
        Serial.print(c2Level);
        
    if (c2Level >= sensibilidad) {
    Serial.print(" / * C2 - UP * /");
    Constel_2_on();
    solucionNum[2] = true;
    }
    else {
     Serial.print(" / * C2 - DOWN * /");
     Constel_2_off();
     solucionNum[2] = false;   
    }
Serial.println(" - C2 OVER - //");
}

// Craneo 3  --------------------------------------------
void craneo3() {
  int orden = 3;
  Serial.print("++Nivel Cr3 : ");
    int sensorC3 = analogRead(A3);
    int c3Level = map(sensorC3,  0, 900, 10, 0);
       
        Serial.print(sensorC3);
        Serial.print("/ medida: ");
        Serial.print(c3Level);
        
    if (c3Level >= sensibilidad) {
    Serial.print(" / * C3 - UP * /");
    Constel_3_on();
    solucionNum[3] = true;
    }
    else {
     Serial.print(" / * C3 - DOWN * /");
     Constel_3_off();
     solucionNum[3] = false;   
    }
Serial.println(" - C3 OVER - //");
}

// Craneo 4  --------------------------------------------
void craneo4() {
  int orden = 4;
  Serial.print("++Nivel Cr4 : ");
    int sensorC4 = analogRead(A4);
    int c4Level = map(sensorC4,  0, 900, 10, 0);
       
        Serial.print(sensorC4);
        Serial.print("/ medida: ");
        Serial.print(c4Level);
        
    if (c4Level >= sensibilidad) {
    Serial.print(" / * C4 - UP * /");
    Constel_4_on();
    solucionNum[4] = true;
    }
    else {
     Serial.print(" / * C4 - DOWN * /");
     Constel_4_off();
     solucionNum[4] = false;   
    }
Serial.println(" - C4 OVER - //");
}

// Craneo 5  --------------------------------------------
void craneo5() {
  int orden = 5;
  Serial.print("++Nivel Cr5 : ");
    int sensorC5 = analogRead(A5);
    int c5Level = map(sensorC5,  0, 900, 10, 0);
      
        Serial.print(sensorC5);
        Serial.print("/ medida: ");
        Serial.print(c5Level);
        
    if (c5Level >= sensibilidad) {
    Serial.print(" / * C5 - UP * /");
    Constel_5_on();
    solucionNum[5] = true;
    }
    else {
     Serial.print(" / * C5 - DOWN * /");
     Constel_5_off();
     solucionNum[5] = false;   
    }
Serial.println(" - C5 OVER - //");
}

// Craneo 6  --------------------------------------------
void craneo6() {
  int orden = 6;
  Serial.print("++Nivel Cr6 : ");
    int sensorC6 = analogRead(A6);
    int c6Level = map(sensorC6,  0, 900, 10, 0);
       
        Serial.print(sensorC6);
        Serial.print("/ medida: ");
        Serial.print(c6Level);
        
    if (c6Level >= sensibilidad) {
    Serial.print(" / * C6 - UP * /");
    Constel_6_on();
    solucionNum[6] = true;
    }
    else {
     Serial.print(" / * C6 - DOWN * /");
     Constel_6_off();
     solucionNum[6] = false;   
    }
Serial.println(" - C6 OVER - //");
}

// Craneo 7  --------------------------------------------
void craneo7() {
  int orden = 7;
  Serial.print("++Nivel Cr7 : ");
    int sensorC7 = analogRead(A7);
    int c7Level = map(sensorC7,  0, 900, 10, 0);
       
        Serial.print(sensorC7);
        Serial.print("/ medida: ");
        Serial.print(c7Level);
        
    if (c7Level >= sensibilidad) {
    Serial.print(" / * C7 - UP * /");
    Constel_7_on();
    solucionNum[7] = true;
    }
    else {
     Serial.print(" / * C7 - DOWN * /");
     Constel_7_off();
     solucionNum[7] = false;   
    }
Serial.println(" - C7 OVER - //");
}




void pinM(){
  pinMode(rele, OUTPUT);
}

void blink() {
  digitalWrite(rele, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(rele, LOW);
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
    blink();
    Serial.println("Rele abierto");
  }
  else {
  Serial.println("Rele cerrado");
  }
}

void noche() {
  for (int i = 0; i <= puntosCraneos ; i++) {
      stripCraneos.setPixelColor(i, 120, 100, 120);
      stripCraneos.show();
     }
  for (int i = 0; i <= puntosCraneos ; i++) {
      stripCraneos.setPixelColor(i, 120, 100, 120);
      stripCraneos.show();
     }
  
  for (int i = 0; i <= puntosCraneos ; i++) {
      stripCraneos2.setPixelColor(i, 120, 100, 120);
      stripCraneos2.show();
     }
  for (int i = 0; i <= puntosCraneos ; i++) {
      stripCraneos2.setPixelColor(i, 120, 100, 120);
      stripCraneos2.show();
     }
  
  
  Serial.println("+tiras Led+ OK:");
 }

void constelaciones() {
  craneo0();
  craneo1();
  craneo2();
  craneo3();
  craneo4();
  craneo5();
  craneo6();
  craneo7();
  }

void setup()
{
  Serial.begin(9600);
  Serial.println("++Starting Cielo++");
  iniled();
  noche();
 
}

void loop()
{
// Miradas
  constelaciones();
  releOpen();
  delay(100);
}
