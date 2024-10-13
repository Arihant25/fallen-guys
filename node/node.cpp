#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include <TinyGPS++.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define LED 35
#define BUTTON_PIN_INPUT 32
#define BUTTON_PIN_OUTPUT 33
#define BUZZER_PIN 32 // Added buzzer pin
#define LED_ALERT 33  // Added alert LED pin
#define LED_REVERSE 4 // Added reverse LED pin

// GPS module pins
#define GPS_RX 25
#define GPS_TX 26

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;

// Default Thresholds (will be updated by central node)
int fallAcc_threshold = 30;
int alert_threshold = 10;
int emergencyContact = 112;

const unsigned long SEND_INTERVAL = 5000; // Send data every 5 seconds
unsigned long lastSendTime = 0;

// Variables to track maximum values
float maxAccX = 0, maxAccY = 0, maxAccZ = 0;
float maxAccelMagnitude = 0;
int maxNetGyro = 0;
bool fallDetected = false;
String lastValidGPSLocation = "17.447315,78.348787"; // Default GPS location

unsigned long alertDelayStart = 0;
bool isDelayStarted = false;

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    pinMode(BUTTON_PIN_INPUT, INPUT);
    pinMode(BUTTON_PIN_OUTPUT, OUTPUT);
    digitalWrite(BUTTON_PIN_OUTPUT, HIGH);

    // Initialize new pins
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_ALERT, OUTPUT);
    pinMode(LED_REVERSE, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_ALERT, LOW);
    digitalWrite(LED_REVERSE, HIGH); // Reverse LED starts ON

    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    Wire.begin();

    // MPU6050 setup
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            digitalWrite(LED, HIGH);
            delay(100);
            digitalWrite(LED, LOW);
            delay(100);
        }
    }
    Serial.println("MPU6050 Found!");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Initialize LoRa
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }
    LoRa.setSyncWord(0xAB);

    delay(2000);
}

void activateAlerts()
{
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_ALERT, HIGH);
    digitalWrite(LED_REVERSE, LOW);
}

void sendLoRaMessage(String message)
{
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    Serial.println("LoRa message sent: " + message);
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
    // Process GPS data
    while (Serial2.available() > 0)
        if (gps.encode(Serial2.read()))
            if (gps.location.isValid())
                lastValidGPSLocation = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);

    // Read sensor data and update maximum values
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float accelMagnitude = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
    int netGyro = sqrt(pow(g.gyro.x, 2) + pow(g.gyro.y, 2) + pow(g.gyro.z, 2));

    maxAccX = max(maxAccX, abs(a.acceleration.x));
    maxAccY = max(maxAccY, abs(a.acceleration.y));
    maxAccZ = max(maxAccZ, abs(a.acceleration.z));
    maxAccelMagnitude = max(maxAccelMagnitude, accelMagnitude);
    maxNetGyro = max(maxNetGyro, netGyro);

    if (accelMagnitude >= fallAcc_threshold && !fallDetected)
    {
        isDelayStarted = true;
        Serial.println(String(alert_threshold));
        alertDelayStart = millis();
        activateAlerts();
    }

    unsigned long currentTime = millis();

    if(isDelayStarted && ((currentTime - alertDelayStart) >= alert_threshold*1000))
      fallDetected = true;

    if (currentTime - lastSendTime >= SEND_INTERVAL)
    {
        String dataMessage = String(maxAccX, 2) + "," +
                             String(maxAccY, 2) + "," +
                             String(maxAccZ, 2) + "," +
                             String(maxAccelMagnitude, 2) + "," +
                             String(maxNetGyro) + "," +
                             lastValidGPSLocation + "," +
                             String(fallDetected ? 1 : 0);

        uint16_t checksum = calculateChecksum(dataMessage);

        String fullMessage = dataMessage + ":" + String(checksum);
        sendLoRaMessage(fullMessage);

        // Reset maximum values after sending, but keep fallDetected state
        maxAccX = maxAccY = maxAccZ = maxAccelMagnitude = 0;
        maxNetGyro = 0;

        lastSendTime = currentTime;
    }

    // Check for incoming LoRa messages (updated thresholds)
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String message = "";
        while (LoRa.available())
            message += (char)LoRa.read();

        // Parse and update thresholds
        // Format: "THRESHOLDS:fallAcc,alert,emergency"
        if (message.startsWith("THRESHOLDS:"))
        {
            String thresholds = message.substring(11);
            int firstComma = thresholds.indexOf(',');
            int secondComma = thresholds.indexOf(',', firstComma + 1);

            fallAcc_threshold = thresholds.substring(0, firstComma).toInt();
            alert_threshold = thresholds.substring(firstComma + 1, secondComma).toInt();
            emergencyContact = thresholds.substring(secondComma + 1).toInt();

            Serial.println("Updated thresholds: " + String(fallAcc_threshold) + ", " + String(alert_threshold) + ", " + String(emergencyContact));
        }
    }

    delay(10);
}
