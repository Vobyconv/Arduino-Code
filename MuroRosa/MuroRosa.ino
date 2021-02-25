#include <Adafruit_NeoPixel.h>

int ledMujer = 11;
int feministas = 15;
int paciencia = 75;

Adafruit_NeoPixel Empoderamiento = Adafruit_NeoPixel(feministas, ledMujer, NEO_GRB + NEO_KHZ800);

//inicio de tioras led
void iniled() {
  
  Empoderamiento.begin();
  Empoderamiento.setBrightness(12);
  Empoderamiento.show();
  Empoderamiento.clear();
}

void RosaOn(){
  for (int i = 0; i <= 20 ; i++) {
    Empoderamiento.setPixelColor(i, 255, 0, 128);
    Empoderamiento.show();
    Empoderamiento.setPixelColor(i+1, 255, 0, 128);
    Empoderamiento.show();
    Empoderamiento.setPixelColor(i+2, 255, 0, 128);
    Empoderamiento.show();
    delay(paciencia);
    Empoderamiento.setPixelColor(i-1, 30, 10, 20);
    Empoderamiento.show();}
    Serial.print(" / * Feminismo - ON * / ");
}

void setup()
{
  Serial.begin(9600);
  Serial.println(" La revolucion será feminista, o no será. ");
  iniled();
}

void loop()
{
  RosaOn();
  //delay(500);
}
