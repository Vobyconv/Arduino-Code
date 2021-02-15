#include <Adafruit_NeoPixel.h>

int ojos = 10;
int ledCielo = 11;
int rele = 12;
int ojosCarona = 12;
int estrellas = 23;

//pines de pulsadores
int pulsaTroll = A0;
int pulsaVaca = A1;
int pulsaOso = A2;
int pulsaMuerte = A3;

//tarjeta de audio
const byte PIN_AUDIO_RST = 6;
const byte PIN_AUDIO_ACT = 7;

//rfid
const byte PIN_RFID_RX = 2;
const byte PIN_RFID_TX = 3;


//puntitos de las constelaciones
const int numPtsConst = 4;
const int trollLeds[numPtsConst] = { 1, 7, 13, 19};
const int vacaLeds[numPtsConst] = { 2, 5, 11, 17,};
const int osoLeds[numPtsConst] = { 6, 9, 15, 21};
const int muerteLeds[numPtsConst] = { 3, 8, 14, 20};

Adafruit_NeoPixel stripOjos = Adafruit_NeoPixel(ojosCarona, ojos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_GRB + NEO_KHZ800);

//Contadores de victoria
int solucion = 2;
int solucionNum[8] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[8] = { 1, 1, 5, 5, 5, 5, 5, 5};
int openAll = 0;

//inicio de tioras led
void iniled() {
  stripOjos.begin();
  stripOjos.setBrightness(200);
  stripOjos.show();
  stripOjos.clear();
  
  stripCielo.begin();
  stripCielo.setBrightness(200);
  stripCielo.show();
  stripCielo.clear();
}

// El Troll
void trollOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(trollLeds[i], 10, 250, 10);
      stripCielo.show();
   }
   Serial.print(" / * TrollLED - ON * / ");
}

void trollOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(trollLeds[i], 200, 200, 200);
      stripCielo.show();
      }
  Serial.print(" / * TrollLed - OFF * / ");
}

void signoTroll() {
  Serial.print("++Nivel Troll : ");
    int pulsaTroll = analogRead(A0);
    int trollLevel = map(pulsaTroll, 0, 1024, 0, 10);
       
        Serial.print(pulsaTroll);
        Serial.print("/ medida: ");
        Serial.print(trollLevel);
        
    if (trollLevel >= 6) {
    Serial.print(" / * Trollpush - UP * /");
    trollOn();
    solucionNum[0] = openRele[0];
    }
    else {
     Serial.print(" / * Trollpush - DOWN * /");
     trollOff();
     solucionNum[0] = 0;
      
    }
Serial.println(" - Troll OVER - //");
}

// La Vaca
void vacaOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(vacaLeds[i], 250, 10, 10);
      stripCielo.show();
      }
  Serial.print(" / * VacaLed - ON * / ");
    }
 void vacaOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(vacaLeds[i], 100, 100, 100);
      stripCielo.show();
      }
   Serial.print(" / * VacaLed - OFF / ");
    }

void signoVaca() {
  Serial.print("++Nivel Vaca  : ");
      
    int pulsaVaca = analogRead(A1);
    int vacaLevel = map(pulsaVaca, 0, 1024, 0, 10);
       Serial.print(pulsaVaca);
       Serial.print("/ medida: ");
       Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    Serial.print(" / * vacaPush - UP  * /");
    vacaOn();
    solucionNum[1] = openRele[1];
    }
    else {
    Serial.print(" / * VacaPush - DOWN * /");
    vacaOff();
    solucionNum[1] = 0;
    }
  
  Serial.println(" - Vaca OVER - //");
}

// El Oso
void osoOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(osoLeds[i], 250, 200, 10);
      stripCielo.show();
            }
  Serial.print(" / * OsoLed - ON * / ");
  }

void osoOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(osoLeds[i], 200, 200, 200);
     
      stripCielo.show();
      }
  Serial.print(" / * OsoLed - OFF * / ");
  }

void signoOso() {
  Serial.print("++Nivel  Oso  : ");
    int pulsaOso = analogRead(A2);
    int osoLevel = map(pulsaOso, 0, 1024, 0, 10);
        Serial.print(pulsaOso);
        Serial.print("/ medida: ");
        Serial.print(osoLevel);
        
    if (osoLevel >= 6) {
    Serial.print(" / * OsoPush - UP * /");
    osoOn();
    solucionNum[2] = openRele[2];
    }
    else {
     Serial.print(" / * OsoPush - DOWN * /");
     osoOff(); 
     solucionNum[2] = 0;
    }
Serial.println(" - Oso over - //");
}


// La Muerte
void muerteOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(muerteLeds[i], 10, 10, 250);
      stripCielo.show();
   }
   Serial.print(" / * MuerteLED - ON * / ");
}

void muerteOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(muerteLeds[i], 200, 200, 200);
      stripCielo.show();
      }
  Serial.print(" / * MuerteLed - OFF * / ");
}

void signoMuerte() {
  Serial.print("++Nivel Muerte : ");
    int pulsaMuerte = analogRead(A3);
    int muerteLevel = map(pulsaMuerte, 0, 1024, 0, 10);
       
        Serial.print(pulsaMuerte);
        Serial.print("/ medida: ");
        Serial.print(muerteLevel);
        
    if (muerteLevel >= 6) {
    Serial.print(" / * Muertepush - UP * /");
    muerteOn();
    solucionNum[3] = openRele[3];
    }
    else {
     Serial.print(" / * Muertepush - DOWN * /");
     muerteOff();
     solucionNum[3] = 0;
      
    }
Serial.println(" - Muerte OVER - //");
}


//Condición de Victoria
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
  if (openAll == solucion) {
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
  //delay(500);
}
