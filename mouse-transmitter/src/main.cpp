
#include <Arduino.h>
#include "CONFIG_VARS.h"
#include "mouseHandler.h"
#include "Communication.h"

MouseHandler mh;

#define pot A7

void setup(void)
{
  Serial.begin(19200);
  NRF::begin();
  pinMode(pot, INPUT);

}
void loop(void)
{

  int potVal = map(analogRead(pot), 0, 1023, 1, 10);
  // Serial.println(potVal);

  TDCoord c = mh.loop(potVal, potVal);

  if (c.z == 1)
  {
    // Serial.println("click");
    NRF::send("L00");

    while (mh.isTouchDown())
      ;
    delay(100);
  }
  else if (c.x != 0 || c.y != 0)
  {
    NRF::send("M00" + String(c.x) + "," + String(c.y));
    // delay(1);
    // Serial.print(c.x);
    // Serial.print(" ");
    // Serial.println(c.y);
  }
}
