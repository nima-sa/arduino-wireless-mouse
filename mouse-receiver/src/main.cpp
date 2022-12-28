
#include <Arduino.h>
// #include <Mouse.h>
#include "CONFIG_VARS.h"
#include "Communication.h"
#include "MouseInterface.h"

#include "Mouse.h"
Mouse_ mouse;

void setup(void)
{
  Serial.begin(115200);
  NRF::begin();
  mouse.begin();
}
void loop(void)
{
  String inp = NRF::read();
  if (inp != "")
  {
    TDCoord command = MouseInterface::getCommand(inp);
    if (command.z != 0)
      mouse.click();
    else if (command.x != 0 || command.y != 0)
      mouse.move(command.x, command.y);

    // Serial.print(command.x);
    // Serial.print(" ");
    // Serial.print(command.y);
    // Serial.print(" ");
    // Serial.println(command.z);
    // Serial.println(inp);
  }

  // NRF::send("hi");
  // delay(1000);
}



// #include <Arduino.h>
// #include <SPI.h>
// // #include <nRF24L01.h>
// #include <RF24.h>
// RF24 radio(7, 8); // CE, CSN
// // RF24 radio(5, 7); // CE, CSN
// // RF24 radio(9, 10); // CE, CSN
// const uint64_t address = 0xE8E8F0F0E1;
// void setup()
// {
//   Serial.begin(115200);
//   radio.begin();
//   radio.openReadingPipe(0, address);
//   radio.setPALevel(RF24_PA_MIN);
//         radio.setDataRate(RF24_250KBPS);

//   radio.startListening();
// }
// void loop()
// {
//   // Serial.println("asdfasdfaf");
//   if (radio.available())
//   {
//     Serial.print(millis());
//     // Serial.println("hi");

//     char text[32] = "";
//     radio.read(&text, sizeof(text));
//     Serial.println(text);
//     delay(1000);
//   }
// }
// // void setup()
// // {
// //   Serial.begin(19200);
// //   radio.begin();
// //   radio.openWritingPipe(address);
// //   radio.setPALevel(RF24_PA_MIN);
// //   radio.setDataRate(RF24_250KBPS);
// //   radio.stopListening();
// // }
// // void loop()
// // {
// //   String n = "nioma";
// //   char text[32] = "hello world";

// //   radio.write(n.c_str(), sizeof(n));
// //   // radio.write(&text, sizeof(text));
// //   Serial.println(millis());
// //   delay(1000);
// // }

// #include <Arduino.h>
// #include <SPI.h>
// #include <RF24.h>

// /****************** User Config ***************************/
// /***      Set this radio as radio number 0 or 1         ***/
// bool radioNumber = 0;

// /* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
// RF24 radio(7, 8);
// /**********************************************************/

// byte addresses[][6] = {"1Node", "2Node"};

// // Used to control whether this node is sending or receiving
// bool role = 1;

// void setup()
// {
//   Serial.begin(115200);

//   Serial.println(F("RF24/examples/GettingStarted"));
//   Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

//   radio.begin();
//   DDRB |= (1 << PB0);

//   // Set the PA Level low to prevent power supply related issues since this is a
//   // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
//   radio.setPALevel(RF24_PA_LOW);

//   // Open a writing and reading pipe on each radio, with opposite addresses
//   if (radioNumber)
//   {
//     radio.openWritingPipe(addresses[1]);
//     radio.openReadingPipe(1, addresses[0]);
//   }
//   else
//   {
//     radio.openWritingPipe(addresses[0]);
//     radio.openReadingPipe(1, addresses[1]);
//   }

//   // Start the radio listening for data
//   radio.startListening();
// }

// void loop()
// {

//   /****************** Ping Out Role ***************************/
//   if (role == 1)
//   {

//     radio.stopListening(); // First, stop listening so we can talk.

//     Serial.println(F("Now sending"));

//     unsigned long start_time = micros(); // Take the time, and send it.  This will block until complete
//     if (!radio.write(&start_time, sizeof(unsigned long)))
//     {
//       Serial.println(F("failed"));
//     }

//     radio.startListening(); // Now, continue listening

//     unsigned long started_waiting_at = micros(); // Set up a timeout period, get the current microseconds
//     boolean timeout = false;                     // Set up a variable to indicate if a response was received or not

//     while (!radio.available())
//     { // While nothing is received
//       if (micros() - started_waiting_at > 200000)
//       { // If waited longer than 200ms, indicate timeout and exit while loop
//         timeout = true;
//         break;
//       }
//     }

//     if (timeout)
//     { // Describe the results
//       Serial.println(F("Failed, response timed out."));
//     }
//     else
//     {
//       unsigned long got_time; // Grab the response, compare, and send to debugging spew
//       radio.read(&got_time, sizeof(unsigned long));
//       unsigned long end_time = micros();

//       // Spew it
//       Serial.print(F("Sent "));
//       Serial.print(start_time);
//       Serial.print(F(", Got response "));
//       Serial.print(got_time);
//       Serial.print(F(", Round-trip delay "));
//       Serial.print(end_time - start_time);
//       Serial.println(F(" microseconds"));
//     }

//     // Try again 1s later
//     delay(1000);
//   }

//   /****************** Pong Back Role ***************************/

//   if (role == 0)
//   {
//     unsigned long got_time;

//     if (radio.available())
//     {
//       // Variable for the received timestamp
//       while (radio.available())
//       {                                               // While there is data ready
//         radio.read(&got_time, sizeof(unsigned long)); // Get the payload
//       }

//       radio.stopListening();                         // First, stop listening so we can talk
//       radio.write(&got_time, sizeof(unsigned long)); // Send the final one back.
//       radio.startListening();                        // Now, resume listening so we catch the next packets.
//       Serial.print(F("Sent response "));
//       Serial.println(got_time);
//     }
//   }

//   /****************** Change Roles via Serial Commands ***************************/

//   if (Serial.available())
//   {
//     char c = toupper(Serial.read());
//     if (c == 'T' && role == 0)
//     {
//       Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
//       role = 1; // Become the primary transmitter (ping out)
//     }
//     else if (c == 'R' && role == 1)
//     {
//       Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
//       role = 0; // Become the primary receiver (pong back)
//       radio.startListening();
//     }
//   }

// } // Loop
