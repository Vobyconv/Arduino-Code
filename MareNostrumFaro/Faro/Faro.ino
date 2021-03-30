#include <Adafruit_NeoPixel.h>

int faroLedPin = 10;
int LedsFaro = 4;
int corto = 400;
int largo = 700;
int mucho = 1100;

Adafruit_NeoPixel Faro = Adafruit_NeoPixel(LedsFaro, faroLedPin, NEO_GRB + NEO_KHZ800);


void iniled() {
  Faro.begin();
  Faro.setBrightness(200);
  Faro.show();
  Faro.clear();
}

void Apagado(){
delay(largo);
Faro.clear();
Faro.show();
delay(largo);
}

void Purpura() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 255, 0, 255);
    Faro.show();
}
    Apagado();
 
    Serial.println(" / * Rojo * / ");
}

void Rojo() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 255, 0, 0);
    Faro.show();
}
    Apagado();
 
    Serial.println(" / * Rojo * / ");
}

void Verde() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 0, 255, 0);
    Faro.show();
}
    Apagado();
 
    Serial.println(" / * Verde * / ");
}

void Azul() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 0, 0, 255);
    Faro.show();
}
    Apagado();
 
    Serial.println(" / * Azul * / ");
}

void Amarillo() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 255, 255, 0);
    Faro.show();
}
    Apagado();
 
    Serial.println(" / * Amar * / ");
}

void LuzFuerte() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 255, 255, 255);
    Faro.show();
}
    Serial.println(" / * Flash * / ");
}
void LuzFlojo() {
for (int i = 0; i <= LedsFaro ; i++) {
    Faro.setPixelColor(i, 50, 50, 50);
    Faro.show();
}
    Serial.println(" / * Flash * / ");
}

void Fogonazo() {
LuzFlojo(); delay(corto);
LuzFuerte(); delay(corto);
LuzFlojo(); delay(corto);
LuzFuerte(); delay(corto);
LuzFlojo(); delay(corto);
LuzFuerte(); delay(corto);
delay(mucho);
}

void Llamada() {
 Fogonazo();
     Rojo();
    Azul();
     Rojo();
   Verde();
     Rojo();
  Amarillo();
    Azul();
    Azul();
   Verde();
     Rojo();
  
}

void Victoria() {
  
  LuzFlojo();
  Purpura();
  LuzFuerte();
  
}

void setup()
{
  Serial.begin(9600);
  Serial.println(" SPQR Faro ");
  iniled();
}

void loop()
{
  Llamada();
    
}
