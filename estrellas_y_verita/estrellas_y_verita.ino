#include <Adafruit_NeoPixel.h>

int ojos = 9;
int ledCielo = 10;
int rele = 11;
int ojosCarona = 12;
int estrellas = 24;

int pulsaTroll = A0;
int pulsaVaca = A1;



const int CONSTELACIONES_NUM = 5;

const int POTS_PINS[CONSTELACIONES_NUM] = {
    A0, A1, A2, A3, A4};

const int LEDS_CONSTELACIONES[CONSTELACIONES_NUM][2] = {
    {0, 4}, 
    {6, 10}, //troll
    {11, 15},
    {16, 20},
    {20, 24}};


Adafruit_NeoPixel stripOjos = Adafruit_NeoPixel(ojosCarona, ojos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_GRB + NEO_KHZ800);


int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

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
  Serial.print(" - troll ON - ");
  }
    else {
     trollOff();
     Serial.print(" - troll OFF - "); 
    }
    Serial.println(" - troll over - ");
}

void signoVaca() {
  Serial.print("++Nivel de Vaca :");
      
    int pulsaVaca = analogRead(A1);
    int vacaLevel = map(pulsaVaca, 0, 1024, 0, 10);
        Serial.print("/ -  vaca: ");
        Serial.print(pulsaVaca);
        Serial.print("/ medida: ");
        Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    vacaOn();
    Serial.print(" - vaca ON - ");
    }
    else {
    vacaOff();
    Serial.print(" - vaca OFF - ");
    }
  
  Serial.println(" - vaca over - ");
}

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
  if (openAll == 3) {
    blink();
  }
}


void trollOn(){
  for (int i = 0; i <= 25 ; i++) {
      stripCielo.setPixelColor(1, 10+i*3, 250-i*3, 10);
      stripCielo.setPixelColor(3, 10+i*2, 250-i*4, 10);
      stripCielo.setPixelColor(8, 10, 250, 10);
      stripCielo.setPixelColor(13, 10, 250, 10);
      stripCielo.show();
            }
  Serial.print("*Troll* up:");
  }

void trollOff(){
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(1, 100, 100, 100);
      stripCielo.setPixelColor(3, 100, 100, 100);
      stripCielo.setPixelColor(8, 100, 100, 100);
      stripCielo.setPixelColor(13, 100, 100, 100);
      stripCielo.show();
      }
  Serial.print("*Troll* down:");
  }

void vacaOn(){
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(2, 250, 10, 10);
      stripCielo.setPixelColor(5, 250, 10, 10);
      stripCielo.setPixelColor(9, 250, 10, 10);
      stripCielo.setPixelColor(0, 250, 10, 10);
      stripCielo.show();
      }
    }
 void vacaOff(){
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(2, 100, 100, 100);
      stripCielo.setPixelColor(5, 100, 100, 100);
      stripCielo.setPixelColor(9, 100, 100, 100);
      stripCielo.setPixelColor(0, 100, 100, 100);
      stripCielo.show();
      }
    }


void noche() {
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 100);
      stripCielo.show();
     }
  Serial.println("+cielo+ OK:");
 }

void constelaciones() {
  signoTroll();
  signoVaca();
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
