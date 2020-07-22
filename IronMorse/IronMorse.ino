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

void a(){punto(); raya(); delay(1000);}
void b(){raya(); punto(); punto(); punto(); delay(1000);}
void c(){raya(); punto(); raya(); punto(); delay(1000);}
void d(){raya(); punto(); punto(); delay(1000);}
void e(){punto(); delay(1000);}
void f(){punto(); punto(); raya(); punto(); delay(1000);}
void g(){raya(); raya(); punto(); delay(1000);}
void h(){punto(); punto(); punto(); punto(); delay(1000);}
void i(){punto(); punto(); delay(1000);}
void j(){punto(); raya(); raya();raya(); delay(1000);}
void k(){raya(); punto(); raya(); delay(1000);}
void l(){punto(); raya(); punto(); punto(); delay(1000);}
void m(){raya(); raya(); delay(1000);}
void n(){raya(); punto(); delay(1000);}
void o(){raya(); raya(); raya(); delay(1000);}
void p(){punto(); raya(); raya(); punto(); delay(1000);}
void q(){raya(); raya(); punto(); raya(); delay(1000);}
void r(){punto(); raya(); punto(); delay(1000);}
void s(){punto(); punto(); punto(); delay(1000);}
void t(){raya(); delay(1000);}
void u(){punto(); punto; raya(); delay(1000);}
void v(){punto(); punto(); punto(); raya(); delay(1000);}
void w(){punto(); raya(); raya(); delay(1000);}
void x(){raya(); punto(); punto(); raya(); delay(1000);}
void y(){raya(); punto(); raya(); raya(); delay(1000);}
void z(){raya(); raya(); punto(); punto(); delay(1000);}

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
         
         
      j(); Serial.print("- j -");
      a(); Serial.print(" a -");
      r(); Serial.print(" r -");
      v(); Serial.print(" v -");
      i(); Serial.print(" i -");
      s(); Serial.println(" s -");
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
