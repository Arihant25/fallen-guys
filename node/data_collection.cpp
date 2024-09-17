#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// TODO: replace placeholders with actual values
#define WiFi_SSID "SSID"
#define WiFi_PASS "PASSWORD"

#define LED_BUILTIN 2
#define BUZZER_PIN 27        // Piezo buzzer connected to pin 27
#define BUTTON_PIN_INPUT 32  // Button input pin
#define BUTTON_PIN_OUTPUT 33 // Button output pin (for constant voltage)

Adafruit_MPU6050 mpu;

WiFiClient client;

// TODO: replace placeholders with actual values
unsigned long myChannelNumber = 123456789;
const char *myWriteAPIKey = "WRITE_API_KEY";

bool alarmActive = false;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_PIN_INPUT, INPUT);
    pinMode(BUTTON_PIN_OUTPUT, OUTPUT);
    digitalWrite(BUTTON_PIN_OUTPUT, HIGH);

    Serial.begin(115200);
    Wire.begin();

    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
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

    delay(2000);
}

void connectToNetwork()
{
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    Serial.print("Connecting to WiFi");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nWiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi");
        digitalWrite(LED_BUILTIN, LOW);
    }
}

int counter = 0;
float highest_value = 0;

void loop()
{
    if (digitalRead(BUTTON_PIN_INPUT) == HIGH)
    {
        alarmActive = false;
        digitalWrite(BUZZER_PIN, HIGH);
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

    if (counter++ == 300)
    {
        if (WiFi.status() != WL_CONNECTED)
            connectToNetwork();

        ThingSpeak.setField(1, a.acceleration.x);
        ThingSpeak.setField(2, a.acceleration.y);
        ThingSpeak.setField(3, a.acceleration.z);
        ThingSpeak.setField(4, g.gyro.x);
        ThingSpeak.setField(5, g.gyro.y);
        ThingSpeak.setField(6, g.gyro.z);
        ThingSpeak.setField(7, temp.temperature);
        ThingSpeak.setField(8, highest_value);

        int code = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

        if (code == 200)
        {
            Serial.println("Channel update successful.");
            digitalWrite(LED_BUILTIN, HIGH);
        }
        else
        {
            Serial.println("Problem updating channel. HTTP error code " + String(code));
            digitalWrite(LED_BUILTIN, LOW);
        }

        if (highest_value >= 15)
        {
            alarmActive = true;
            digitalWrite(BUZZER_PIN, LOW);
        }

        counter = 0;
        highest_value = 0;
    }

    if (alarmActive)
        digitalWrite(BUZZER_PIN, LOW);
    else
        digitalWrite(BUZZER_PIN, HIGH);

    delay(50);
}