#include <RH_ASK.h>

///////////////
///////////////
// Data meaning

// S1 = 111
// S0 = 000
// L1 = 111111111
// L0 = 000000000

// uint8_t max value = 255

// 111000 = 0x38 = 56
// 111111 = 0x3F = 63

///////////
///////////
// Preamble
// S1 L0 L1 S0 L1 S0 S1 L0 L1 S0
// L1 S0 L1 S0 S1 L0 S1 L0 L1 S0
// L1 S0 S1 L0 L1 S0 S1 L0 L1 S0
// S1 L0 L1 S0 L1 S0 L1 S0 L1 S0

// 111000 000000 111111 111000 111111 111000 111000 000000 111111 111000 
// 111111 111000 111111 111000 111000 000000 111000 000000 111111 111000 
// 111111 111000 111000 000000 111111 111000 111000 000000 111111 111000 
// 111000 000000 111111 111000 111111 111000 111111 111000 111111 111000 

// 0x38 0x00 0x3F 0x38 0x3F 0x38 0x38 0x00 0x3F 0x38 
// 0x3F 0x38 0x3F 0x38 0x38 0x00 0x38 0x00 0x3F 0x38 
// 0x3F 0x38 0x38 0x00 0x3F 0x38 0x38 0x00 0x3F 0x38 
// 0x38 0x00 0x3F 0x38 0x3F 0x38 0x3F 0x38 0x3F 0x38 

//////////////
//////////////
// Balcony off
// S1 L0 L1 S0

// 111000 000000 111111 111000

// 0x38 0x00 0x3F 0x38

/////////////
/////////////
// Balcony on
// L1 S0 S1 L0

// 111111 111000 111000 000000

// 0x3F 0x38 0x38 0x00

////////////
////////////
// Postamble
// S1 L0 S1 L0 S1 L0

// 111000 000000 111000 000000 111000 000000

// 0x38 0x00 0x38 0x00 0x38 0x00

////////////
////////////

// Data was too short when S0/S1=1 bit and L0/L1=3 bits
// uint8_t BalconyOff[13] = { 0x8E, 0xE8, 0xEE, 0xE8, 0x8E, 0xE8, 0xE8, 0xE8, 0xEE, 0xEE, 0x8E, 0x88, 0x8 };
// uint8_t BalconyOn[13]  = { 0x8E, 0xE8, 0xEE, 0xE8, 0x8E, 0xE8, 0xE8, 0xE8, 0xEE, 0xEE, 0xE8, 0x88, 0x8 }; 

// Data is much better when S0/S1=3 bits and L0/L1=9 bits
// uint8_t BalconyOff[] = {
//     // Preamble
//     0x38, 0x00, 0x3F, 0x38, 0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38,
//     0x3F, 0x38, 0x3F, 0x38, 0x38, 0x00, 0x38, 0x00, 0x3F, 0x38,
//     0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38,
//     0x38, 0x00, 0x3F, 0x38, 0x3F, 0x38, 0x3F, 0x38, 0x3F, 0x38,

//     // Balcony off
//     0x38, 0x00, 0x3F, 0x38,

//     // Postamble
//     0x38, 0x00, 0x38, 0x00, 0x38, 0x00
// }; // Total 50

// uint8_t BalconyOn[]  = {
//     // Preamble
//     0x38, 0x00, 0x3F, 0x38, 0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38,
//     0x3F, 0x38, 0x3F, 0x38, 0x38, 0x00, 0x38, 0x00, 0x3F, 0x38,
//     0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38, 0x38, 0x00, 0x3F, 0x38,
//     0x38, 0x00, 0x3F, 0x38, 0x3F, 0x38, 0x3F, 0x38, 0x3F, 0x38,

//     // Balcony on
//     0x3F, 0x38, 0x38, 0x00,

//     // Postamble
//     0x38, 0x00, 0x38, 0x00, 0x38, 0x00
// }; // Total 50

uint8_t BalconyOff[] = { 0 };
uint8_t BalconyOn[] = { 1 };

const uint8_t len = sizeof(BalconyOff);

const int repeat = 1;

const int BUTTON_PIN = 13;
int buttonValue = LOW;

RH_ASK driver(/*speed*/ 12000, /*rxPin*/ 11, /*txPin*/ 12, /*pttPin*/ 10, /*pttInverted*/ false);

int count;

// For serial messages
const int msgSize = 50;
char msg[msgSize];


bool lastValue = false;

void setup()
{
    count = 0;
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT);
    if (!driver.init())
    {
        Serial.println("init failed");
    }
    else
    {
        Serial.println("Ready to transmit...");
    }
}

void loop()
{
    buttonValue = digitalRead(BUTTON_PIN);
    if (buttonValue == HIGH)
    {
        uint8_t* selected;
        snprintf(msg, msgSize, "%d - Turning balcony lights ", count);
        Serial.print(msg);

        if (lastValue == false)
        {
            Serial.println("ON...");
            selected = BalconyOn;
            lastValue = true;
        }
        else if (lastValue == true)
        {
            Serial.println("OFF...");
            selected = BalconyOff;
            lastValue = false;
        }

        for (int i = 0; i < repeat; i++)
        {
            driver.send(selected, len);
            driver.waitPacketSent();
            delay(250);
        }
        Serial.println("Done! Ready to transmit again...");
        buttonValue = LOW;
        count++;
    }
}