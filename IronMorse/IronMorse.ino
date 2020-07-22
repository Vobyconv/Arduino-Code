#include <Adafruit_NeoPixel.h>

int tira1 = 3;
int puntosled = 4;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(puntosled, tira1, NEO_GRB + NEO_KHZ800);

int mediluz = A1;

int corto = 250;
int largo = 750;

 
void iniled(){
  strip1.begin();
  strip1.setBrightness(200);
  strip1.show();
  strip1.clear();
}

void s(){punto(); punto(); punto(); delay(1000);}
void o(){raya(); raya(); raya(); delay(1000);}

void punto(){
  for (int i = 0; i <= puntosled; i++) {
        strip1.setPixelColor(i, 10, 255, 10);
        strip1.show();
          }
          delay(corto);
          strip1.clear();
          strip1.show();
          delay(corto);
}

void raya(){
  for (int i = 0; i <= puntosled; i++) {
        strip1.setPixelColor(i, 10, 255, 10);
        strip1.show();
          }
          delay(largo);
          strip1.clear();
          strip1.show();
          delay(corto);
}


void start(){
 for (int i = 0; i <= puntosled; i++) {
    strip1.setPixelColor(i, 255, 10, 10);
    strip1.show();
    }
    delay(1000);
    strip1.clear();
    strip1.show();
    delay(1000);
}

void sensorluz(){
  
int mediluz = analogRead(A1);
  
int medipos = map(mediluz, 6, 679, 0, 10);
  
  Serial.print("{poten: ");
  Serial.println(medipos);


     if (medipos > 5){
       start();
         
         
      s();
        o();
        s();
         
          
             
        }
        else {
        strip1.clear();
        strip1.show();  
        }
}


void setup(){
  Serial.begin(9600);
  Serial.println("iniciando a jarvis");
  iniled();
  
}

void loop()
{
sensorluz();

}
