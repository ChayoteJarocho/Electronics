#include <RH_ASK.h>

RH_ASK driver(/*speed*/ 2000, /*rxPin*/ 11, /*txPin*/ 12, /*pttPin*/ 10, /*pttInverted*/ false);

uint8_t buffer[RH_ASK_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buffer);

int count;

const int msgSize = 20;
char msg[msgSize];

void setup()
{
    count = 0;
    Serial.begin(9600);
    if (!driver.init())
    {
        Serial.println("init failed");
    }
    else
    {
        Serial.println("Ready to receive...");
    }
}

void loop()
{

    if (driver.recv(buffer, &len)) // Non-blocking
    {
        // Message with a good checksum received, dump it.
        buffer[len] = 0;
        snprintf(msg, msgSize, "%d - Received: ", count);
        driver.printBuffer(msg, buffer, len);
        count++;
    }
}