#include <SPI.h> // handles SPI communication
#include <nRF24L01.h> // controls module
#include <RF24.h> // controls module

// Diagrams:
// https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/
// Code:
// https://www.seeedstudio.com/blog/2019/11/21/nrf24l01-getting-started-arduino-guide/

// TNOREM02 INDOOR/OUTDOOR REMOTE CONTROLLER
// receiver frequency: 315 MHz so it does NOT work with RF24L01

RF24 radio(9, 8); // CE (Chip enable) on pin 9, CSN (Chip Select Not) on pin 8

// address through which two modules communicate
const byte address[6] = "00001";

// buffer
char text[32] = {0};

void setup()
{
  while (!Serial)
  {
    Serial.begin(9600);
  }
  
  radio.begin();
  radio.openReadingPipe(1, address); // set pipe on stream 0, sets the address
  radio.startListening(); // set module as receiver
}

void loop()
{
  if (radio.available()) // read data if available in buffer
  {
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
