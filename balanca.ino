#include <TM1637.h>
#include <HX711.h>

#define pin1DT 14
#define pin1SCK 12 


const int CLK = 32;
const int DIO = 33;

HX711 balanca;
TM1637 tm(CLK, DIO);

float medida = 0;
int a, b, c, d;

void setup() {
  Serial.begin(57600);
  tm.init();
  tm.set(BRIGHT_TYPICAL);
  balanca.begin(pin1DT, pin1SCK);
  balanca.set_scale(92431);
  delay(2000);
  balanca.tare();
}



void loop() {
  Serial.println("Balanca: ");
  medida = balanca.get_units(5);
  Serial.println(medida, 2);

  if(medida < 0){
    medida = medida * (-1);
  }

  medida = medida * 100;
  a = medida / 1000;
  b = (medida - (a*1000)) / 100;
  c = (medida - (a*1000) - (b*100)) / 10;
  d = (medida - (a*1000) - (b*100) - (c*10));

  tm.display(0, a);
  tm.display(1, b);
  tm.display(2, c);
  tm.display(3, d);

  delay(100);
  balanca.power_down();
  delay(1000);
  balanca.power_up();
}
