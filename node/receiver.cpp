#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define LED 22
#define BUZZER_IO 27 // Changed to match sender code

unsigned long lastMessageTime = 0;
const unsigned long MESSAGE_TIMEOUT = 30000; // 30 seconds

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    Serial.println("LoRa Receiver");

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    pinMode(LED, OUTPUT);
    pinMode(BUZZER_IO, OUTPUT);

    digitalWrite(LED, LOW);
    digitalWrite(BUZZER_IO, HIGH); // Changed to HIGH to match sender code (buzzer off)

    Serial.println("Receiver initialized and waiting for messages...");
}

void loop()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String message = "";
        while (LoRa.available())
            message += (char)LoRa.read();

        Serial.print("Received message: ");
        Serial.println(message);
        Serial.print("RSSI: ");
        Serial.println(LoRa.packetRssi());

        lastMessageTime = millis();

        if (message == "FALL")
            activateAlarm("Fall detected by sender");
        else if (message == "OK")
            deactivateAlarm("Received OK signal");
        else
            Serial.println("WARNING: Unexpected message received: " + message);
    }

    // Check for timeout
    if (millis() - lastMessageTime > MESSAGE_TIMEOUT)
        activateAlarm(("No signal received for " + String((millis() - lastMessageTime) / 1000) + " seconds").c_str());

    // Print time since last message every second
    static unsigned long lastPrintTime = 0;
    if (millis() - lastPrintTime > 1000)
    {
        Serial.print("Time since last message: ");
        Serial.print((millis() - lastMessageTime) / 1000);
        Serial.println(" seconds");
        lastPrintTime = millis();
    }
}

void activateAlarm(const char *reason)
{
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER_IO, LOW); // Changed to LOW to activate buzzer
    Serial.print("ALARM ACTIVATED: ");
    Serial.println(reason);
    Serial.println("LED ON and Buzzer ON");
}

void deactivateAlarm(const char *reason)
{
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER_IO, HIGH); // Changed to HIGH to deactivate buzzer
    Serial.print("ALARM DEACTIVATED: ");
    Serial.println(reason);
    Serial.println("LED OFF and Buzzer OFF");
}
