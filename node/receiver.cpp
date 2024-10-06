#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <LoRa.h>

#define WiFi_SSID "drama queen"
#define WiFi_PASS "ojeo6887"

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define LED 22
#define BUZZER_IO 27

WiFiClient client;

unsigned long dataWriteChannelNumber = 2684114;
const char *myWriteAPIKey = "TTIBU3CIFKLESX0Z";

unsigned long thresholdReadChannel = 2678150;
const char *myReadAPIKey = "RZH4FA34SCB2XOQX";

int fallAcc_threshold = 15;
int alert_threshold = 300;
int emergencyContact = 112;

const unsigned long THINGSPEAK_INTERVAL = 15000; // 15 seconds
unsigned long lastThingSpeakUpdate = 0;

float maxAccelMagnitude = 0;
int maxNetGyro = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LED, OUTPUT);
    pinMode(BUZZER_IO, OUTPUT);
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER_IO, HIGH);

    connectToWiFi();
    ThingSpeak.begin(client);

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    // Read initial thresholds from ThingSpeak
    readThingSpeakThresholds();
}

void connectToWiFi()
{
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void readThingSpeakThresholds()
{
    ThingSpeak.readMultipleFields(thresholdReadChannel, myReadAPIKey);
    fallAcc_threshold = ThingSpeak.getFieldAsInt(1);
    alert_threshold = ThingSpeak.getFieldAsInt(2);
    emergencyContact = ThingSpeak.getFieldAsInt(3);
    Serial.println("Updated thresholds from ThingSpeak");
}

uint16_t calculateChecksum(const String &message)
{
    uint16_t checksum = 0;
    for (char c : message)
        checksum += c;
    return checksum;
}

void loop()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String message = "";
        while (LoRa.available())
            message += (char)LoRa.read();

        int colonIndex = message.lastIndexOf(':');
        if (colonIndex != -1)
        {
            String data = message.substring(0, colonIndex);
            uint16_t receivedChecksum = message.substring(colonIndex + 1).toInt();
            uint16_t calculatedChecksum = calculateChecksum(data);

            if (receivedChecksum == calculatedChecksum)
                processData(data);
            else
                Serial.println("Checksum mismatch. Discarding data.");
        }
    }

    unsigned long currentTime = millis();
    if (currentTime - lastThingSpeakUpdate >= THINGSPEAK_INTERVAL)
    {
        updateThingSpeak();
        lastThingSpeakUpdate = currentTime;
    }
}

void processData(const String &data)
{
    int commaIndex = data.indexOf(',');
    float accelMagnitude = data.substring(0, commaIndex).toFloat();
    int netGyro = data.substring(commaIndex + 1).toInt();

    maxAccelMagnitude = max(maxAccelMagnitude, accelMagnitude);
    maxNetGyro = max(maxNetGyro, netGyro);

    if (accelMagnitude >= fallAcc_threshold)
    {
        activateAlarm("Fall detected");
    }
}

void updateThingSpeak()
{
    ThingSpeak.setField(1, maxAccelMagnitude);
    ThingSpeak.setField(2, maxNetGyro);

    int response = ThingSpeak.writeFields(dataWriteChannelNumber, myWriteAPIKey);

    if (response == 200)
        Serial.println("ThingSpeak update successful");
    else
        Serial.println("ThingSpeak update failed. Error: " + String(response));

    // Reset max values
    maxAccelMagnitude = 0;
    maxNetGyro = 0;

    // Read updated thresholds
    readThingSpeakThresholds();

    // Send updated thresholds to end-user node
    String thresholdMessage = "THRESHOLDS:" + String(fallAcc_threshold) + "," + String(alert_threshold) + "," + String(emergencyContact);
    LoRa.beginPacket();
    LoRa.print(thresholdMessage);
    LoRa.endPacket();
}

void activateAlarm(const char *reason)
{
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER_IO, LOW);
    Serial.println("ALARM ACTIVATED: " + String(reason));
}

void deactivateAlarm()
{
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER_IO, HIGH);
    Serial.println("ALARM DEACTIVATED");
}