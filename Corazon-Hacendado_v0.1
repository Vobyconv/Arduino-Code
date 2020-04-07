#include <Adafruit_NeoPixel.h>

int poten1 = A1;
int poten2 = A2;
int poten3 = A2;
int poten4 = A3;

int tira1 = 8;
int tira2 = 9;
int tira5 = 11;
int puntosled = 11;
int puntosexito = 23;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(puntosled, tira1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(puntosled, tira2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip5 = Adafruit_NeoPixel(puntosexito, tira5, NEO_GRB + NEO_KHZ800);

int rele = 2;

int valor1 = 0;
int valor2 = 0;
int valor3 = 0;
int valor4 = 0;
int victoria = valor1 + valor2 + valor3 + valor4;


void setup()
{
  Serial.begin(9600);
  pinMode (rele, OUTPUT);
  
  int poten1 = analogRead(A1);
  int poten2 = analogRead(A2);
  int poten3 = analogRead(A3);
  int poten4 = analogRead(A4);
  
  strip1.begin();
  strip1.setBrightness(200);
  strip1.show();
  
  strip2.begin();
  strip2.setBrightness(200);
  strip2.show();
  
  strip5.begin();
  strip5.setBrightness(200);
  strip5.show();
    
  }

void loop(){
  
  int victoria = valor1 + valor2 + valor3 + valor4;
  
  int poten1 = analogRead(A1);
  int poten2 = analogRead(A2);
  //int poten3 = analogRead(A3);
  //int poten4 = analogRead(A4);
  
  int potenled1 = map(poten1, 0, 1023, 0, 11);
  int potenled2 = map(poten2, 0, 1023, 0, 11);
  //int potenled3 = map(poten3, 0, 1023, 24, 35);
  //int potenled4 = map(poten4, 0, 1023, 36, 47);
  
  for (int i = 0; i <= potenled1; i++) {
    strip1.setPixelColor(i, 255, 0, 10);
    strip1.show();
    delay(5);
  }
  
  for (int i = 0; i <= potenled2; i++) {
    strip2.setPixelColor(i, 10, 0, 250);
    strip2.show();
    delay(5);
  }
  
    
  //strip.setPixelColor(potenled1, 255, 0, 10);
  //strip.show();
  //strip.setPixelColor(potenled2, 10,250, 10);
  //strip.show();
  //strip.setPixelColor(potenled3, 10, 0, 250);
  //strip.show();
  //strip.setPixelColor(potenled4, 200, 200, 10);
  //strip.show();
  strip1.clear();
  strip2.clear();
    
  Serial.print("{poten: ");
  Serial.print(potenled1);
    Serial.print(" - ");
  Serial.print(potenled2);
    Serial.print(" - ");
 // Serial.print(potenled3);
   // Serial.print(" - ");
 // Serial.print(potenled4);
   // Serial.println("}");
  
  int solucion[3] = { 2, 9 };
  Serial.print(" solucion: ");
  Serial.print(solucion[0]);
  Serial.print(" - ");
  Serial.print(solucion[1]);
  
   Serial.print("/ coincidencias: ");
  
    if (potenled1 == solucion[0]){valor1 = 1;}
        else {valor1 = 0;}
        Serial.print(valor1);
    if (potenled2 == solucion[1]){valor2 = 1;}
        else {valor2 = 0;}
 		Serial.print(valor2);
   // if (potenled3 == 29){valor3 = 1;}
       // else {valor3 = 0;}
	   // Serial.print(valor3);	
   // if (potenled4 == 44){valor4 = 1;}
       // else {valor4 = 0;}
  		//Serial.println(valor4);
        
        // Serial.println(victoria);
  
  if (victoria == 2){
  digitalWrite(rele, HIGH);
    Serial.println(" /OPEN");
    for (int i = 0; i <= puntosexito ; i++) {
    strip5.setPixelColor(i, 10, 255, 10);
    strip5.show();
    delay(1);
  }
  }
  else {Serial.println(" /CLOSE");}
      
      delay(50);

}
