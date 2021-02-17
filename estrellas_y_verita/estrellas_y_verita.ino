#include <Adafruit_NeoPixel.h>

int ojos = 10;
int ledCielo = 11;
int rele = 12;
int ojosCarona = 12;
int estrellas = 8;

//pines de pulsadores
int pulsaTroll = A0;
int pulsaVaca = A1;
int pulsaOso = A2;
int pulsaMuerte = A3;

Adafruit_NeoPixel stripOjos = Adafruit_NeoPixel(ojosCarona, ojos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_GRB + NEO_KHZ800);

//tarjeta de audio
const byte PIN_AUDIO_RST = 6;
const byte PIN_AUDIO_ACT = 7;

//rfid
const byte PIN_RFID_RX = 2;
const byte PIN_RFID_TX = 3;


//puntitos de las constelaciones
const int numPtsConst = 4;
const int constelLeds0[numPtsConst] = { 1, 9, 17, 25};
const int constelLeds1[numPtsConst] = { 2, 10, 18, 17};
const int constelLeds2[numPtsConst] = { 3, 11, 19, 21};
const int constelLeds3[numPtsConst] = { 4, 12, 20, 20};
const int constelLeds4[numPtsConst] = { 5, 13, 21, 19};
const int constelLeds5[numPtsConst] = { 6, 14, 22, 17};
const int constelLeds6[numPtsConst] = { 7, 15, 23, 21};
const int constelLeds7[numPtsConst] = { 8, 16, 24, 20};

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

void Constel_1_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 250, 10, 10);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - ON * / ");
}

void Constel_1_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - OFF * / ");
}

void Constel_2_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 250, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - ON * / ");
}

void Constel_2_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - OFF * / ");
}

void Constel_3_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 10, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * MurteLed - ON * / ");
}

void Constel_3_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * MuerteLED - OFF * / ");
}

//Contadores de victoria
int solucion = 2;
int solucionNum[] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[] = { true, true, false, false,};// false, false, false, false}
int openAll = 0;

//inicio de tioras led
void iniled() {
  stripOjos.begin();
  stripOjos.setBrightness(30);
  stripOjos.show();
  stripOjos.clear();
  
  stripCielo.begin();
  stripCielo.setBrightness(10);
  stripCielo.show();
  stripCielo.clear();
}

// El Troll  -------------------------------------
void signoTroll() {
  Serial.print("++Nivel Troll : ");
    int pulsaTroll = analogRead(A0);
    int trollLevel = map(pulsaTroll, 0, 1024, 0, 10);
       
        Serial.print(pulsaTroll);
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
      
    int pulsaVaca = analogRead(A1);
    int vacaLevel = map(pulsaVaca, 0, 1024, 0, 10);
       Serial.print(pulsaVaca);
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
    int pulsaOso = analogRead(A2);
    int osoLevel = map(pulsaOso, 0, 1024, 0, 10);
        Serial.print(pulsaOso);
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
    int pulsaMuerte = analogRead(A3);
    int muerteLevel = map(pulsaMuerte, 0, 1024, 0, 10);
       
        Serial.print(pulsaMuerte);
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
  }

void setup()
{
  Serial.begin(9600);
  Serial.println("++Starting Cielo++");
  iniled();
  pinM();
  noche();
}

void loop()
{
  constelaciones();
  releOpen();
  delay(500);
}
