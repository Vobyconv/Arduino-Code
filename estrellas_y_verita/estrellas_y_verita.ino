#include <Adafruit_NeoPixel.h>

int ojos = 9;
int ledCielo = 10;
int rele = 11;
int ojosCarona = 12;
int estrellas = 23;

int pulsaTroll = A0;
int pulsaVaca = A1;
int pulsaOso = A2;
int pulsaHachu = A3;



const int BotonesNum = 8;

//const int ConstPulsar[BotonesNum] = { A0, A1, A2, A3, A4, A5, A6, A7};

//puntitos de las constelaciones
const int numPtsConst = 4;
const int trollLeds[numPtsConst] = { 1, 7, 13, 19};
const int vacaLeds[numPtsConst] = { 2, 5, 11, 17,};
const int osoLeds[numPtsConst] = { 6, 9, 15, 21};

Adafruit_NeoPixel stripOjos = Adafruit_NeoPixel(ojosCarona, ojos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_GRB + NEO_KHZ800);


int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

//Contadores de victoria

int open1 = 0;
int open2 = 0;
int open3 = 0;
int openAll = 0;

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
  Serial.print("*Troll* up:");
  }

void trollOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(trollLeds[i], 200, 200, 200);
     
      stripCielo.show();
      }
  Serial.print("*Troll* down:");
  }

void signoTroll() {
  Serial.print("++Nivel Troll :");
    int pulsaTroll = analogRead(A0);
    int trollLevel = map(pulsaTroll, 0, 1024, 0, 10);
        Serial.print(" -- troll: ");
        Serial.print(pulsaTroll);
        Serial.print("/ medida: ");
        Serial.print(trollLevel);
        
    if (trollLevel >= 6) {
    trollOn();
    open1 = 2;
  Serial.print(" - troll ON - ");
  }
    else {
     trollOff();
     open1 = 0;
     Serial.print(" - troll OFF - "); 
    }
     Serial.println(" - troll over - ");
}

// La Vaca
void vacaOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(vacaLeds[i], 250, 10, 10);
      stripCielo.show();
      }
    }
 void vacaOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(vacaLeds[i], 100, 100, 100);
      stripCielo.show();
      }
    }

void signoVaca() {
  Serial.print("++Nivel Vaca :");
      
    int pulsaVaca = analogRead(A1);
    int vacaLevel = map(pulsaVaca, 0, 1024, 0, 10);
       Serial.print("/ -  vaca: ");
       Serial.print(pulsaVaca);
       Serial.print("/ medida: ");
       Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    vacaOn();
    open2 = 2;
    Serial.print(" - vaca ON - ");
    }
    else {
    vacaOff();
    open2 = 0;
    Serial.print(" - vaca OFF - ");
    }
  
  Serial.println(" - vaca over - ");
}

// El Oso
void osoOn(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(osoLeds[i], 250, 200, 10);
      stripCielo.show();
            }
  Serial.print("*Oso* up:");
  }

void osoOff(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(osoLeds[i], 200, 200, 200);
     
      stripCielo.show();
      }
  Serial.print("*Oso* down:");
  }

void signoOso() {
  Serial.print("++Nivel Oso :");
    int pulsaOso = analogRead(A2);
    int osoLevel = map(pulsaOso, 0, 1024, 0, 10);
        Serial.print(" -- oso: ");
        Serial.print(pulsaOso);
        Serial.print("/ medida: ");
        Serial.print(osoLevel);
        
    if (osoLevel >= 6) {
    osoOn();
    open3 = 2;
  Serial.print(" - oso ON - ");
  }
    else {
     osoOff();
     open3 = 0;
     Serial.print(" - oso OFF - "); 
    }
     Serial.println(" - oso over - ");
}


//Condición de Victoria
void blink() {
  digitalWrite(rele, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(rele, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
}

void releOpen() {
  int openAll = open1+open2+open3;
  Serial.print("openAll: ");
  Serial.println(openAll);
  if (openAll == 6) {
    blink();
    Serial.println("Rele abierto");
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
  constelaciones();
  releOpen();
  //delay(500);
}
