// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

//Ordenes basicas ---------------------
int ledCielo = 11;
int rele = 12;
int estrellas = 8;

//pines de pulsadores
int pulsa0 = A0; //Troll
int pulsa1 = A1; //Vaca
int pulsa2 = A2; //Oso
int pulsa3 = A3; //Muerte
int pulsa4 = A4; //Foca
int pulsa5 = A5; //Ballena
int pulsa6 = A6; //Otro
int pulsa7 = A7; //Pato

Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_RGB + NEO_KHZ800);

//Contadores de victoria
int solucion = 2;
int solucionNum[] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[] = { true, true, false, false, false, false,};// false, false}
int openAll = 0;

//puntitos de las constelaciones
const int numPtsConst = 4;
const int constelLeds0[numPtsConst] = { 1, 9, 17, 19};
const int constelLeds1[numPtsConst] = { 2, 10, 18, 17};
const int constelLeds2[numPtsConst] = { 3, 11, 19, 21};
const int constelLeds3[numPtsConst] = { 4, 12, 20, 20};
const int constelLeds4[numPtsConst] = { 5, 13, 21, 19};
const int constelLeds5[numPtsConst] = { 6, 14, 22, 17};
const int constelLeds6[numPtsConst] = { 7, 15, 23, 21};
const int constelLeds7[numPtsConst] = { 8, 16, 24, 20};

// Funciones de constelaciones
void Constel_0_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], 10, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * TrollLED - ON * / ");
}

void Constel_0_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * TrollLED - OFF * / ");
}
//--
void Constel_1_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 250, 10, 10);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - ON * / ");
}

void Constel_1_off(){
  int orden = 1;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - OFF * / ");
}
//--
void Constel_2_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 250, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - ON * / ");
}

void Constel_2_off(){
  int orden = 2;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - OFF * / ");
}
//--
void Constel_3_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 10, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * MurteLed - ON * / ");
}

void Constel_3_off(){
  int orden = 3;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * MuerteLED - OFF * / ");
}
//--
void Constel_4_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds4[i], 10, 250, 250);
      stripCielo.show();
  }
      Serial.print(" / * FocaLed - ON * / ");
}

void Constel_4_off(){
  int orden = 4;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds4[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * FocaLED - OFF * / ");
}

//--
void Constel_5_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds5[i], 10, 250, 250);
      stripCielo.show();
  }
      Serial.print(" / * BallenaLed - ON * / ");
}

void Constel_5_off(){
  int orden = 5;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds5[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * BallenaLED - OFF * / ");
}

void Constel_6_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds6[i], 10, 250, 250);
      stripCielo.show();
  }
      Serial.print(" / * OtroLed - ON * / ");
}

void Constel_6_off(){
  int orden = 6;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds6[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * OtroLED - OFF * / ");
}

void Constel_7_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds7[i], 10, 250, 250);
      stripCielo.show();
  }
      Serial.print(" / * PatoLed - ON * / ");
}

void Constel_7_off(){
  int orden = 7;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds7[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * PatoLED - OFF * / ");
}

//inicio de tioras led
void iniled() {
  stripCielo.begin();
  stripCielo.setBrightness(10);
  stripCielo.show();
  stripCielo.clear();
}

// El Troll  -------------------------------------
void signoTroll() {
  Serial.print("++Nivel Troll : ");
    int pulsa0 = analogRead(A0);
    int trollLevel = map(pulsa0, 0, 1024, 0, 10);
       
        Serial.print(pulsa0);
        Serial.print("/ medida: ");
        Serial.print(trollLevel);
        
    if (trollLevel >= 6) {
    Serial.print(" / * Trollpush - UP * /");
    Constel_0_on();
    solucionNum[0] = true;
    }
    else {
     Serial.print(" / * Trollpush - DOWN * /");
     Constel_0_off();
     solucionNum[0] = false;
    }
Serial.println(" - Troll OVER - //");
}

// La Vaca  --------------------------------------------
void signoVaca() {
  Serial.print("++Nivel Vaca  : ");
      
    int pulsa1 = analogRead(A1);
    int vacaLevel = map(pulsa1, 0, 1024, 0, 10);
       Serial.print(pulsa1);
       Serial.print("/ medida: ");
       Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    Serial.print(" / * vacaPush - UP  * /");
    Constel_1_on();
    solucionNum[1] = true;
    }
    else {
    Serial.print(" / * VacaPush - DOWN * /");
    Constel_1_off();
    solucionNum[1] = false;
    }
  Serial.println(" - Vaca OVER - //");
}

// El Oso  ------------------------------------------------
void signoOso() {
  Serial.print("++Nivel  Oso  : ");
    int pulsa2 = analogRead(A2);
    int osoLevel = map(pulsa2, 0, 1024, 0, 10);
    
        Serial.print(pulsa2);
        Serial.print("/ medida: ");
        Serial.print(osoLevel);
        
    if (osoLevel >= 6) {
    Serial.print(" / * OsoPush - UP * /");
    Constel_2_on();
    solucionNum[2] = true;
    }
    else {
     Serial.print(" / * OsoPush - DOWN * /");
     Constel_2_off(); 
     solucionNum[2] = false;
    }
Serial.println(" - Oso over - //");
}

// La Muerte  --------------------------------------------
void signoMuerte() {
  Serial.print("++Nivel Muerte : ");
    int pulsa3 = analogRead(A3);
    int muerteLevel = map(pulsa3, 0, 1024, 0, 10);
       
        Serial.print(pulsa3);
        Serial.print("/ medida: ");
        Serial.print(muerteLevel);
        
    if (muerteLevel >= 6) {
    Serial.print(" / * Muertepush - UP * /");
    Constel_3_on();
    solucionNum[3] = true;
    }
    else {
     Serial.print(" / * Muertepush - DOWN * /");
     Constel_3_off();
     solucionNum[3] = false;
      
    }
Serial.println(" - Muerte OVER - //");
}

// La Foca  --------------------------------------------
void signoFoca() {
  int orden = 4;
  Serial.print("++Nivel Foca : ");
    int pulsa4 = analogRead(A4);
    int focaLevel = map(pulsa4, 0, 1024, 0, 10);
       
        Serial.print(pulsa4);
        Serial.print("/ medida: ");
        Serial.print(focaLevel);
        
    if (focaLevel >= 6) {
    Serial.print(" / * Focapush - UP * /");
    Constel_4_on();
    solucionNum[4] = true;
    }
    else {
     Serial.print(" / * Focapush - DOWN * /");
     Constel_4_off();
     solucionNum[4] = false;   
    }
Serial.println(" - Foca OVER - //");
}

// La Ballena  --------------------------------------------
void signoBallena() {
  int orden = 5;
  Serial.print("++Nivel Ballena : ");
    int pulsa5 = analogRead(A5);
    int ballenaLevel = map(pulsa5, 0, 1024, 0, 10);
       
        Serial.print(pulsa5);
        Serial.print("/ medida: ");
        Serial.print(ballenaLevel);
        
    if (ballenaLevel >= 6) {
    Serial.print(" / * Ballenapush - UP * /");
    Constel_5_on();
    solucionNum[5] = true;
    }
    else {
     Serial.print(" / * Ballenapush - DOWN * /");
     Constel_5_off();
     solucionNum[5] = false;   
    }
Serial.println(" - Ballena OVER - //");
}

// La Otra  --------------------------------------------
void signoOtro() {
  int orden = 6;
  Serial.print("++Nivel Otro : ");
    int pulsa6 = analogRead(A6);
    int ballenaLevel = map(pulsa6, 0, 1024, 0, 10);
       
        Serial.print(pulsa6);
        Serial.print("/ medida: ");
        Serial.print(ballenaLevel);
        
    if (ballenaLevel >= 6) {
    Serial.print(" / * Otropush - UP * /");
    Constel_6_on();
    solucionNum[6] = true;
    }
    else {
     Serial.print(" / * Otropush - DOWN * /");
     Constel_6_off();
     solucionNum[6] = false;   
    }
Serial.println(" - Otro OVER - //");
}

//Condición de Victoria  --------------------------------
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
  int total = 4;
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
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  Serial.println("+cielo+ OK:");
 }

void constelaciones() {
  signoTroll();
  signoVaca();
  signoOso();
  signoMuerte();
  signoFoca();
  signoBallena();
  }

void setup()
{
  Serial.begin(9600);
  sSerial.begin(9600);
  Serial.println("++Starting Cielo++");
  initAudioPins();
  resetAudio();
  iniled();
  pinM();
  noche();
  playLedStartupPattern();
  sSerial.listen();
}

void loop()
{
// Boca
  readTagAndPlayAudio();
  showStaticLed();
// Cielo 
  constelaciones();
  releOpen();
  delay(100);
}
