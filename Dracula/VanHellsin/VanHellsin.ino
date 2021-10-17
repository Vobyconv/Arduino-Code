// C++ code
// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>

//Ordenes basicas ---------------------
int pinBestias = 12;
int puntosLed = 20;
int numBestias = 4;
int poten = A0;
int bestia = 0;
const int ledPorBestia = (puntosLed/numBestias);


//tiras -------------------
Adafruit_NeoPixel tiraBestias = Adafruit_NeoPixel(puntosLed, pinBestias, NEO_GRB + NEO_KHZ800);

void iniled() {
  tiraBestias.begin();
  tiraBestias.setBrightness(200);
  tiraBestias.show();
  tiraBestias.clear();
}

const uint32_t verde = Adafruit_NeoPixel::Color(5, 250, 5);
const uint32_t azul = Adafruit_NeoPixel::Color(5, 5, 250);
const uint32_t rojo = Adafruit_NeoPixel::Color(250, 5, 5);
const uint32_t amarillo = Adafruit_NeoPixel::Color(250, 250, 5);
const uint32_t blanco = Adafruit_NeoPixel::Color(200, 200, 200);
  
void hombreLobo(){
  int posicion = 1;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, azul);
      tiraBestias.show();
  }
}

void aracne(){
  int posicion = 2;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, verde);
      tiraBestias.show();
  }
}

void chupacabras(){
  int posicion = 3;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, rojo);
      tiraBestias.show();
  }
}

void serpiente(){
  int posicion = 4;
  int factor = posicion-1;
  for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, amarillo);
      tiraBestias.show();
  }
}

//Selector ------
void selector() {
    int rosca = analogRead(A0);
    int bestia = map(rosca, 0, 1030, 1, numBestias+1);
    Serial.println(rosca);
    Serial.println(bestia);

  int posicion = bestia;
  int factor = posicion-1;
  
    for (int i = 0+factor*ledPorBestia; i <= ledPorBestia*posicion; i++) {
    tiraBestias.setPixelColor(i, azul);
    tiraBestias.show();
    }
      tiraBestias.clear();
}

void setup()
{
  Serial.begin(9600);
  Serial.println("++Starting Cielo++");
  iniled();
}

void loop()
{
  selector(); 
}
