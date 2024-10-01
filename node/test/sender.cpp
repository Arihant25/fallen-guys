#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

int counter = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("LoRa Sender Test");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    Serial.println("LoRa Initializing OK!");
}

void loop()
{
    Serial.print("Sending packet: ");
    Serial.println(counter);

    // Send packet
    LoRa.beginPacket();
    LoRa.print("Hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;

    delay(5000);
}