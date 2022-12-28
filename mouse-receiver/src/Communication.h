#ifndef CM_H
#define CM_H

#include <Arduino.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "CONFIG_VARS.h"

RF24 radio(__RADIO_CE__, __RADIO_CSN__); // CE, CSN

// const uint64_t address = 0xE8E8F0F0E1;

class NRF
{
  // static uint64_t _ADR;
  // static String HIGH_PRIORITY_COMMAND;
  // static String BN;

public:
  static void begin();
  static String read();
  static void send(String);
  static void clear();
  static bool hasData();
  // static void dump(bool);
};

// uint64_t Communication::_ADR = 0;
// String Communication::HIGH_PRIORITY_COMMAND = "";
// String NRF::BN = String(__RADIO_BN__);

bool NRF::hasData()
{
  return radio.available();
}

void NRF::clear()
{
  char *a[32];
  while (radio.available())
    radio.read(a, sizeof(a));
}

void NRF ::begin()
{
  // _ADR = ADR;
  radio.begin();
  radio.openReadingPipe(__RADIO_CHANNEL_, __RADIO_ADDRESS__);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // radio.openWritingPipe(__RADIO_ADDRESS__);
  // radio.stopListening();
}

String NRF::read()
{
  if (radio.available())
  {
    char text[32] = "";
    // delay(10); // wait for message to complete
    while (radio.available())
      radio.read(&text, sizeof(text));

    return String(text);
  }
  return "";
}

void NRF::send(String msg)
{

  Serial.println("Radio out: " + msg);
  const char *txt;
  unsigned l;

  txt = msg.c_str();
  l = msg.length();

  radio.openWritingPipe(__RADIO_ADDRESS__);
  radio.stopListening();

  radio.write(txt, l);

  // if (toggle_state)
  // {
  //   radio.openReadingPipe(__RADIO_CHANNEL_, __RADIO_ADDRESS__);
  //   radio.startListening();
  // }
}

// void Communication::dump(bool _send)
// {
//   if (_send)
//     send("");
//   HIGH_PRIORITY_COMMAND = "";
// }

#endif