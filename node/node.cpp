#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

// TODO: replace placeholders with actual values
#define WiFi_SSID "WIFI_SSID"
#define WiFi_PASS "WIFI_PASS"

#define LED 35
#define BUZZER_IO 27
#define BUTTON_PIN_INPUT 32
#define BUTTON_PIN_OUTPUT 33

// LoRa module pins
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

// GPS module pins
#define GPS_RX 25
#define GPS_TX 26

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
WiFiClient client;

// TODO: replace placeholders with actual values
unsigned long dataWriteChannelNumber = 123456789;
const char *myWriteAPIKey = "WRITE_API_KEY";

unsigned long thresholdReadChannel = 2678150;
const char *myReadAPIKey = "RZH4FA34SCB2XOQX";

// TODO: replace with actual password
String readPassword = "PASSWD";

bool alarmActive = false;

// Default Thresholds
int fallAcc_threshold = 15;
int alert_threshold = 300;
int emergencyContact = 112;

void setup()
{
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    pinMode(BUZZER_IO, OUTPUT);
    digitalWrite(BUZZER_IO, HIGH);
    pinMode(BUTTON_PIN_INPUT, INPUT);
    pinMode(BUTTON_PIN_OUTPUT, OUTPUT);
    digitalWrite(BUTTON_PIN_OUTPUT, HIGH);

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

    delay(100);

    connectToNetwork();

    ThingSpeak.begin(client);

    // Initialize LoRa
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(433E6))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    // Read Thresholds from ThingSpeak
    readThingSpeakThresholds();

    delay(2000);
}

void connectToNetwork()
{
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    Serial.print("Connecting to WiFi");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
    {
        digitalWrite(LED, HIGH);
        delay(100);
        digitalWrite(LED, LOW);
        delay(100);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED, HIGH);
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi");
        digitalWrite(LED, LOW);
    }
}

void sendLoRaMessage(String message)
{
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    Serial.println("LoRa message sent: " + message);
}

void readThingSpeakThresholds()
{
    // Reading Thresholds
    String checkPassword = ThingSpeak.readStringField(thresholdReadChannel, 4, myReadAPIKey);
    if(readPassword == checkPassword)
    {
      Serial.println("Authentication Successful.");
      Serial.println("Reading and Updating Thresholds");
      Serial.println();

      ThingSpeak.readMultipleFields(thresholdReadChannel, myReadAPIKey);

      // Update Thresholds
      fallAcc_threshold = ThingSpeak.getFieldAsInt(1);
      alert_threshold = ThingSpeak.getFieldAsInt(2);
      emergencyContact = ThingSpeak.getFieldAsInt(3);
      
      Serial.println("Received thresholds from Server");
      Serial.println("fallAcc = " + fallAcc_threshold);
      Serial.println("alert = " + alert_threshold);
      Serial.println("emergencyContact = " + emergencyContact);
      Serial.println();

    }
    else
    {
      Serial.println("Authentication Failed.");
      Serial.println("Reverting to default/previously obtained values...");
      Serial.println();
    }
}

int counter = 0;
float highest_value = 0;
float prevLat = 0.0;
float prevLng = 0.0;
bool gpsAvailable = false;

void loop()
{
    if (digitalRead(BUTTON_PIN_INPUT) == HIGH)
    {
        alarmActive = false;
        digitalWrite(BUZZER_IO, HIGH);
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    Serial.print("(");
    Serial.print(counter);
    Serial.println(")");

    Serial.print("\tAcceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    float accelMagnitude = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
    Serial.print("\tAcceleration Magnitude: ");
    Serial.print(accelMagnitude);
    Serial.println(" m/s^2");

    highest_value = max(highest_value, accelMagnitude);

    Serial.print("\tRotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("\tTemperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

    Serial.println("");

    // Read GPS data
    while (Serial2.available() > 0)
        gps.encode(Serial2.read());

    if (counter++ == 300)
    {
        if (WiFi.status() != WL_CONNECTED)
            connectToNetwork();

        // Reading Thresholds
        readThingSpeakThresholds();

        int fallDetected = highest_value >= fallAcc_threshold;
        int netGyro = sqrt(pow(g.gyro.x, 2) + pow(g.gyro.y, 2) + pow(g.gyro.z, 2));
        String GPSloc = String(gps.location.lat(), 6) + " " + String(gps.location.lng(), 6);

        ThingSpeak.setField(1, a.acceleration.x);
        ThingSpeak.setField(2, a.acceleration.y);
        ThingSpeak.setField(3, a.acceleration.z);
        ThingSpeak.setField(4, highest_value);
        ThingSpeak.setField(5, netGyro);
        // ThingSpeak.setField(6, NULL);
        ThingSpeak.setField(7, GPSloc);
        ThingSpeak.setField(8, fallDetected);

        int code = ThingSpeak.writeFields(dataWriteChannelNumber, myWriteAPIKey);

        if (code == 200)
        {
            Serial.println("Channel update successful.");
            digitalWrite(LED, HIGH);
        }
        else
        {
            Serial.println("Problem updating channel. HTTP error code " + String(code));
            digitalWrite(LED, LOW);
        }

        String loraMessage = "OK";
        if (highest_value >= fallAcc_threshold)
        {
            alarmActive = true;
            digitalWrite(BUZZER_IO, LOW);
            Serial.println("\n\nFall detected!\n\n");
            loraMessage = "FALL";
        }

        // Add GPS coordinates to LoRa message if available
        if (gps.location.isValid())
            loraMessage += " LAT:" + String(gps.location.lat(), 6) + " LON:" + String(gps.location.lng(), 6);
        sendLoRaMessage(loraMessage);

        counter = 0;
        highest_value = 0;
    }

    if (alarmActive)
        digitalWrite(BUZZER_IO, LOW);
    else
        digitalWrite(BUZZER_IO, HIGH);

    delay(50);
}