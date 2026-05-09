#include <SPI.h> // handles SPI communication
#include <nRF24L01.h> // controls module
#include <RF24.h> // controls module

// Diagrams:
// https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
// Code:
// https://www.seeedstudio.com/blog/2019/11/21/nrf24l01-getting-started-arduino-guide/

RF24 radio(9, 8); // CE (Chip enable), CSN (Chip Select Not)

// address through which two modules communicate
const byte address[6] = "00001";

// message to send
const char text[] = "Hello world";


void setup()
{
  radio.begin();
  radio.openWritingPipe(address); // sets the address
  radio.stopListening(); // set module as transmitter
}

void loop()
{
  radio.write(&text, sizeof(text));
  delay(1000);
}
