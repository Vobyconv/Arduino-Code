// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

//Ordenes basicas ---------------------
int ledCielo = 9;
int rele = 12;
int estrellas = 135;
int wait = 200;

//pines de pulsadores
int pulsa0 = A0; //Troll
int pulsa1 = A1; //Vaca
int pulsa2 = A2; //Oso
int pulsa3 = A3; //Muerte
int pulsa4 = A4; //Foca
int pulsa5 = A5; //Ballena
int pulsa6 = A6; //Otro
int pulsa7 = A7; //Pato

//Tira led y colores
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_RGB + NEO_KHZ800);

const uint32_t Azul = Adafruit_NeoPixel::Color(5, 5, 250);
const uint32_t Blanco = Adafruit_NeoPixel::Color(200, 200, 200);

//Contadores de victoria
int solucion = 2;
int solucionNum[] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[] = { false, true, false, true, true, false, false, false};
int openAll = 0;

//puntitos de las constelaciones
const int numPtsConst = 9;
const int constelLeds0[numPtsConst] = { 0, 1, 2, 3, 4, 5, 6, 7};
const int constelLeds1[numPtsConst] = { 19, 20, 21, 22, 23, 24, 25, 26, 27};
const int constelLeds2[numPtsConst] = { 38, 39, 40, 41, 42, 43, 44, 45};
const int constelLeds3[numPtsConst] = { 52, 53, 54, 55, 56, 57, 58, 59, 60};
const int constelLeds4[numPtsConst] = { 67, 68, 69, 70, 71, 72, 73, 74, 75};
const int constelLeds5[numPtsConst] = { 81, 82, 83, 84, 85, 86, 87, 88};
const int constelLeds6[numPtsConst] = { 103, 104, 105, 106, 107, 108, 109, 110};
const int constelLeds7[numPtsConst] = { 120, 121, 122, 123, 124, 125, 126, 127, 128};

// Funciones de constelaciones
void Constel_0_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * TrollLED - ON * / "));
}

void Constel_0_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * TrollLED - OFF * / "));
}
//--
void Constel_1_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * VacaLED - ON * / "));
}

void Constel_1_off(){
  int orden = 1;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * VacaLED - OFF * / "));
}
//--
void Constel_2_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * OsoLED - ON * / "));
}

void Constel_2_off(){
  int orden = 2;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * OsoLED - OFF * / "));
}
//--
void Constel_3_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * MurteLed - ON * / "));
}

void Constel_3_off(){
  int orden = 3;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * MuerteLED - OFF * / "));
}
//--
void Constel_4_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds4[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * FocaLed - ON * / "));
}

void Constel_4_off(){
  int orden = 4;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds4[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * FocaLED - OFF * / "));
}

//--
void Constel_5_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds5[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * BallenaLed - ON * / "));
}

void Constel_5_off(){
  int orden = 5;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds5[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * BallenaLED - OFF * / "));
}

void Constel_6_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds6[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * CocoLed - ON * / "));
}

void Constel_6_off(){
  int orden = 6;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds6[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * CocoLED - OFF * / "));
}

void Constel_7_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds7[i], Azul);
      stripCielo.show();
  }
      Serial.print(F(" / * PatoLed - ON * / "));
}

void Constel_7_off(){
  int orden = 7;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds7[i], Blanco);
      stripCielo.show();
  }
      Serial.print(F(" / * PatoLED - OFF * / "));
}

//inicio de tiras led
void iniled() {
  stripCielo.begin();
  stripCielo.setBrightness(15);
  stripCielo.show();
  delay(wait);
  stripCielo.clear();
}

// El Troll  -------------------------------------
void signoTroll() {
  Serial.print(F("++Nivel Troll : "));
    int pulsa0 = analogRead(A0);
    int trollLevel = map(pulsa0, 0, 1024, 0, 10);
       
        Serial.print(pulsa0);
        Serial.print(F("/ medida: "));
        Serial.print(trollLevel);
        
    if (trollLevel >= 6) {
    Serial.print(F(" / * Trollpush - UP * /"));
    Constel_0_on();
    solucionNum[0] = true;
    }
    else {
     Serial.print(F(" / * Trollpush - DOWN * /"));
     Constel_0_off();
     solucionNum[0] = false;
    }
Serial.println(F(" - Troll OVER - //"));
}

// La Vaca  --------------------------------------------
void signoVaca() {
  Serial.print(F("++Nivel Vaca  : "));
      
    int pulsa1 = analogRead(A1);
    int vacaLevel = map(pulsa1, 0, 1024, 0, 10);
       Serial.print(pulsa1);
       Serial.print(F("/ medida: "));
       Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    Serial.print(F(" / * vacaPush - UP  * /"));
    Constel_1_on();
    solucionNum[1] = true;
    }
    else {
    Serial.print(F(" / * VacaPush - DOWN * /"));
    Constel_1_off();
    solucionNum[1] = false;
    }
  Serial.println(F(" - Vaca OVER - //"));
}

// El Oso  ------------------------------------------------
void signoOso() {
  Serial.print(F("++Nivel  Oso  : "));
    int pulsa2 = analogRead(A2);
    int osoLevel = map(pulsa2, 0, 1024, 0, 10);
    
        Serial.print(pulsa2);
        Serial.print(F("/ medida: "));
        Serial.print(osoLevel);
        
    if (osoLevel >= 6) {
    Serial.print(F(" / * OsoPush - UP * /"));
    Constel_2_on();
    solucionNum[2] = true;
    }
    else {
     Serial.print(F(" / * OsoPush - DOWN * /"));
     Constel_2_off(); 
     solucionNum[2] = false;
    }
Serial.println(F(" - Oso over - //"));
}

// La Muerte  --------------------------------------------
void signoMuerte() {
  Serial.print(F("++Nivel Muerte : "));
    int pulsa3 = analogRead(A3);
    int muerteLevel = map(pulsa3, 0, 1024, 0, 10);
       
        Serial.print(pulsa3);
        Serial.print(F("/ medida: "));
        Serial.print(muerteLevel);
        
    if (muerteLevel >= 6) {
    Serial.print(F(" / * Muertepush - UP * /"));
    Constel_3_on();
    solucionNum[3] = true;
    }
    else {
     Serial.print(" / * Muertepush - DOWN * /");
     Constel_3_off();
     solucionNum[3] = false;
      
    }
Serial.println(F(" - Muerte OVER - //"));
}

// La Foca  --------------------------------------------
void signoFoca() {
  int orden = 4;
  Serial.print(F("++Nivel Foca : "));
    int pulsa4 = analogRead(A4);
    int focaLevel = map(pulsa4, 0, 1024, 0, 10);
       
        Serial.print(pulsa4);
        Serial.print(F("/ medida: "));
        Serial.print(focaLevel);
        
    if (focaLevel >= 6) {
    Serial.print(F(" / * Focapush - UP * /"));
    Constel_4_on();
    solucionNum[4] = true;
    }
    else {
     Serial.print(F(" / * Focapush - DOWN * /"));
     Constel_4_off();
     solucionNum[4] = false;   
    }
Serial.println(F(" - Foca OVER - //"));
}

// La Ballena  --------------------------------------------
void signoBallena() {
  int orden = 5;
  Serial.print(F("++Nivel Ballena : "));
    int pulsa5 = analogRead(A5);
    int ballenaLevel = map(pulsa5, 0, 1024, 0, 10);
       
        Serial.print(pulsa5);
        Serial.print(F("/ medida: "));
        Serial.print(ballenaLevel);
        
    if (ballenaLevel >= 6) {
    Serial.print(F(" / * Ballenapush - UP * /"));
    Constel_5_on();
    solucionNum[5] = true;
    }
    else {
     Serial.print(F(" / * Ballenapush - DOWN * /"));
     Constel_5_off();
     solucionNum[5] = false;   
    }
Serial.println(F(" - Ballena OVER - //"));
}

// La Otra  --------------------------------------------
void signoOtro() {
  int orden = 6;
  Serial.print(F("++Nivel Coco : "));
    int pulsa6 = analogRead(A6);
    int otroLevel = map(pulsa6, 0, 1024, 0, 10);
       
        Serial.print(pulsa6);
        Serial.print(F("/ medida: "));
        Serial.print(otroLevel);
        
    if (otroLevel >= 6) {
    Serial.print(F(" / * Cocopush - UP * /"));
    Constel_6_on();
    solucionNum[6] = true;
    }
    else {
     Serial.print(F(" / * Otropush - DOWN * /"));
     Constel_6_off();
     solucionNum[6] = false;   
    }
Serial.println(F(" - Coco OVER - //"));
}

// El Pato  --------------------------------------------
void signoPato() {
  int orden = 7;
  Serial.print(F("++Nivel Pato : "));
    int pulsa7 = analogRead(A7);
    int patoLevel = map(pulsa7, 0, 1024, 0, 10);
       
        Serial.print(pulsa7);
        Serial.print(F("/ medida: "));
        Serial.print(patoLevel);
        
    if (patoLevel >= 6) {
    Serial.print(F(" / * Patopush - UP * /"));
    Constel_7_on();
    solucionNum[7] = true;
    }
    else {
     Serial.print(F(" / * Otropush - DOWN * /"));
     Constel_7_off();
     solucionNum[7] = false;   
    }
Serial.println(F(" - Otro OVER - //"));
}

//Condición de Victoria  --------------------------------
void pinM(){
  pinMode(rele, OUTPUT);
}

void blink() {
  digitalWrite(rele, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  //digitalWrite(rele, LOW);
  //delay(1000); // Wait for 1000 millisecond(s)
}

void releOpen() {
  //int openAll = solucionNum[0]+solucionNum[1]+solucionNum[2]+solucionNum[3]+solucionNum[4]+solucionNum[5]+solucionNum[6]+solucionNum[7];
  Serial.print(F("openAll: "));
  Serial.println(openAll);
  int total = 8;
  int exito = 0;
  for (int i = 0; i <= total-1 ; i++) {
    if (solucionNum[i] == openRele[i]) {
    exito++;
    Serial.print(i);
    Serial.println(F(" // Coincidencia //"));
  }
  else {
  Serial.print(i);
  Serial.println(F(" // Falla //"));
  }
  
  }
  if (exito == total) {
    blink();
    Serial.println("Rele abierto");
  }
  else {
  digitalWrite(rele, LOW);
  Serial.println(F("Rele cerrado"));
  }
}

void noche() {
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  Serial.println(F("+cielo+ OK:"));
 }

void constelaciones() {
  signoTroll();
  signoVaca();
  signoOso();
  signoMuerte();
  signoFoca();
  signoBallena();
  signoOtro();
  signoPato();
  }

void setup()
{
  Serial.begin(9600);
  Serial.println(F("++Starting Cielo++"));

  iniled();
  pinM();
  noche();

}

void loop()
{

// Cielo 
  constelaciones();
  releOpen();
  delay(500);
}
