#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <LoRa.h>

// TODO: Replace with WiFi credentials

#define WiFi_SSID "WIFI_SSID"
#define WiFi_PASS "WIFI_PASS"

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define LED 22
#define REVERSE_LED 4 // New reverse LED pin
#define BUZZER_IO 27

#define WIFI_TIMEOUT 5000

#define PASSWORD "ENTER_SUPER_SECRET_PASSWD"

WiFiClient client;

// TODO: Replace with ThingSpeak channel details

unsigned long dataWriteChannelNumber;
const char *myWriteAPIKey;

unsigned long thresholdReadChannel;
const char *myReadAPIKey;

int fallAcc_threshold = 50;
int alert_threshold = 10;
int emergencyContact = 112;
String password;

const unsigned long THINGSPEAK_INTERVAL = 15000; // 15 seconds
unsigned long lastThingSpeakUpdate = 0;

// Variables to store the latest data
float latestAccX = 0, latestAccY = 0, latestAccZ = 0;
float maxAccelMagnitude = 0;
int maxNetGyro = 0;
String latestGPSLocation = "";
bool fallDetected = false;

void setup()
{
    Serial.begin(115200);

    pinMode(LED, OUTPUT);
    pinMode(REVERSE_LED, OUTPUT); // Initialize the reverse LED pin
    pinMode(BUZZER_IO, OUTPUT);
    digitalWrite(LED, LOW);
    digitalWrite(REVERSE_LED, LOW);
    digitalWrite(BUZZER_IO, LOW);

    connectToWiFi();
    ThingSpeak.begin(client);

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setSyncWord(0xAB);

    // Read initial thresholds from ThingSpeak
    readThingSpeakThresholds();

    digitalWrite(REVERSE_LED, HIGH); // Set reverse LED to opposite state
}

void connectToWiFi()
{
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    Serial.print("Connecting to WiFi");
    unsigned long start = millis();
    unsigned long now = millis();
    while (now - start < WIFI_TIMEOUT && WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        now = millis();
    }
    if(WiFi.status() != WL_CONNECTED){
      Serial.println("WiFi Unavailable! Falling back to last updated or default values");
      Serial.printf("f: %d, a: %d, e: %d\n", fallAcc_threshold, alert_threshold, emergencyContact);
      return;
    }
    Serial.println("\nWiFi connected");
}

void readThingSpeakThresholds()
{
    int response = ThingSpeak.readMultipleFields(thresholdReadChannel, myReadAPIKey);
    if(response == 200) 
      Serial.println("Got updated thresholds from ThingSpeak");
    else{
      Serial.println("Connection to ThingSpeak failed. Error: " + String(response));
      return;
    }
    fallAcc_threshold = ThingSpeak.getFieldAsInt(1);
    alert_threshold = ThingSpeak.getFieldAsInt(2);
    emergencyContact = ThingSpeak.getFieldAsInt(3);
    password = ThingSpeak.getFieldAsString(4);
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

        Serial.print("message: ");
        Serial.println(message);

        int colonIndex = message.lastIndexOf(':');
        if (colonIndex != -1)
        {
            String data = message.substring(0, colonIndex);
            uint16_t receivedChecksum = message.substring(colonIndex + 1).toInt();
            uint16_t calculatedChecksum = calculateChecksum(data);

            if (receivedChecksum == calculatedChecksum)
            {
                processData(data);
                unsigned long currentTime = millis();
                if (currentTime - lastThingSpeakUpdate >= THINGSPEAK_INTERVAL)
                {
                    if(WiFi.status() != WL_CONNECTED)
                        connectToWiFi();
                    updateThingSpeak();
                    lastThingSpeakUpdate = currentTime;
                }
            }
            else
                Serial.println("Checksum mismatch. Discarding data.");
        }
    }
}

void processData(const String &data)
{
    // Parse the incoming data
    // Format: accX,accY,accZ,accelMagnitude,netGyro,gpsLat,gpsLng,fallDetected
    int commaIndex = 0;
    int nextCommaIndex = data.indexOf(',');

    latestAccX = data.substring(commaIndex, nextCommaIndex).toFloat();
    commaIndex = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', commaIndex);

    latestAccY = data.substring(commaIndex, nextCommaIndex).toFloat();
    commaIndex = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', commaIndex);

    latestAccZ = data.substring(commaIndex, nextCommaIndex).toFloat();
    commaIndex = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', commaIndex);

    float accelMagnitude = data.substring(commaIndex, nextCommaIndex).toFloat();
    commaIndex = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', commaIndex);

    int netGyro = data.substring(commaIndex, nextCommaIndex).toInt();
    commaIndex = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', commaIndex);
    int temp = nextCommaIndex + 1;
    nextCommaIndex = data.indexOf(',', temp);

    int colonIndex = data.indexOf(':');

    latestGPSLocation = data.substring(commaIndex, nextCommaIndex);
    commaIndex = nextCommaIndex + 1;

    fallDetected = fallDetected || (data.substring(commaIndex, colonIndex).toInt() == 1);

    maxAccelMagnitude = max(maxAccelMagnitude, accelMagnitude);
    maxNetGyro = max(maxNetGyro, netGyro);

    if (fallDetected)
        activateAlarm("Fall detected");
}

void updateThingSpeak()
{
    ThingSpeak.setField(1, latestAccX);
    ThingSpeak.setField(2, latestAccY);
    ThingSpeak.setField(3, latestAccZ);
    ThingSpeak.setField(4, maxAccelMagnitude);
    ThingSpeak.setField(5, maxNetGyro);
    ThingSpeak.setField(6, (maxAccelMagnitude > fallAcc_threshold) ? 1 : 0);
    ThingSpeak.setField(7, latestGPSLocation);
    ThingSpeak.setField(8, fallDetected ? 1 : 0);

    if(password == PASSWORD){
        int response = ThingSpeak.writeFields(dataWriteChannelNumber, myWriteAPIKey);

        if (response == 200)
            Serial.println("ThingSpeak update successful");
        else
            Serial.println("ThingSpeak update failed. Error: " + String(response));
    }
    else 
        Serial.println("Auth Failed. Cannot update ThingSpeak Data");

    

    // Reset max values
    maxAccelMagnitude = 0;
    maxNetGyro = 0;
    fallDetected = false;

    // Read updated thresholds
    readThingSpeakThresholds();

    // Send updated thresholds to end-user node
    String thresholdMessage = "THRESHOLDS:" + String(fallAcc_threshold) + "," + String(alert_threshold) + "," + String(emergencyContact);
    Serial.println(thresholdMessage);
    LoRa.beginPacket();
    LoRa.print(thresholdMessage);
    LoRa.endPacket();
}

void activateAlarm(const char *reason)
{
    digitalWrite(LED, HIGH);
    digitalWrite(REVERSE_LED, LOW); // Set reverse LED to opposite state
    digitalWrite(BUZZER_IO, HIGH);
    Serial.println("ALARM ACTIVATED: " + String(reason));
}

void deactivateAlarm()
{
    digitalWrite(LED, LOW);
    digitalWrite(REVERSE_LED, HIGH); // Set reverse LED to opposite state
    digitalWrite(BUZZER_IO, LOW);
    Serial.println("ALARM DEACTIVATED");
}