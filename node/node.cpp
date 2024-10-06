#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

#define LED 35
#define BUTTON_PIN_INPUT 32
#define BUTTON_PIN_OUTPUT 33

Adafruit_MPU6050 mpu;

// Default Thresholds (will be updated by central node)
int fallAcc_threshold = 15;
int alert_threshold = 300;
int emergencyContact = 112;

const unsigned long SEND_INTERVAL = 5000; // Send data every 5 seconds
unsigned long lastSendTime = 0;

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    pinMode(BUTTON_PIN_INPUT, INPUT);
    pinMode(BUTTON_PIN_OUTPUT, OUTPUT);
    digitalWrite(BUTTON_PIN_OUTPUT, HIGH);

    Serial.begin(115200);
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

    delay(2000);
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
    unsigned long currentTime = millis();

    if (currentTime - lastSendTime >= SEND_INTERVAL)
    {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        float accelMagnitude = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
        int netGyro = sqrt(pow(g.gyro.x, 2) + pow(g.gyro.y, 2) + pow(g.gyro.z, 2));

        String dataMessage = String(accelMagnitude, 2) + "," + String(netGyro);
        uint16_t checksum = calculateChecksum(dataMessage);

        String fullMessage = dataMessage + ":" + String(checksum);
        sendLoRaMessage(fullMessage);

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