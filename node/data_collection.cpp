#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>

// TODO: replace placeholders with actual values
#define WiFi_SSID "SSID"
#define WiFi_PASS "PASSWORD"

#define BUZZER_PIN 22

#define LED_R 21
#define LED_G 19
#define LED_B 18

Adafruit_MPU6050 mpu;

WiFiClient client;

// TODO: replace placeholders with actual values
unsigned long myChannelNumber = 123456789;
const char *myWriteAPIKey = "WRITE_API_KEY";

void success()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, i);
        analogWrite(LED_G, 255 - i);
        analogWrite(LED_B, 255 - i);
        delay(5);
    }

    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, 255 - i);
        analogWrite(LED_G, i);
        analogWrite(LED_B, 255 - i);
        delay(5);
    }

    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, 255 - i);
        analogWrite(LED_G, 255 - i);
        analogWrite(LED_B, i);
        delay(5);
    }

    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_B, 0);
}

void connectingToNetwork()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, i);
        analogWrite(LED_G, i);
        analogWrite(LED_B, 0);
        delay(5);
    }
}

void connectedToNetwork()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, 255 - i);
        analogWrite(LED_G, 255 - i);
        analogWrite(LED_B, 0);
        delay(5);
    }
}

void error()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, i);
        analogWrite(LED_G, min(0, 200 - i));
        analogWrite(LED_B, min(0, 200 - i));
        delay(5);
    }
}

void initializePins()
{
    pinMode(BUZZER_PIN, OUTPUT);

    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);

    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);

    digitalWrite(BUZZER_PIN, HIGH);
}

void connectToNetwork()
{
    if (WiFi.status() == WL_CONNECTED)
        return;
    connectingToNetwork();
    WiFi.begin(WiFi_SSID, WiFi_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    connectedToNetwork();
}

void thingspeakError()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, i);
        analogWrite(LED_G, i / 2);
        analogWrite(LED_B, 0);
        delay(5);
    }
}

void setup()
{
    initializePins();

    Serial.begin(115200);
    Wire.begin();

    // Try to initialize MPU6050
    if (!mpu.begin(0x69))
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    // Set up the accelerometer range
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    // Set up the gyroscope range
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    // Set filter bandwidth
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    delay(100);

    connectToNetwork();

    ThingSpeak.begin(client);

    delay(2000);
}

int counter = 0;

void loop()
{
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

    float val = pow(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2), 0.5);
    Serial.print("\tSpecial Value ;) : ");
    Serial.print(val);
    Serial.println("m/s^2");

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

    if (counter++ == 60)
    {
        counter = 0;
        connectToNetwork();

        ThingSpeak.setField(1, a.acceleration.x);
        ThingSpeak.setField(2, a.acceleration.y);
        ThingSpeak.setField(3, a.acceleration.z);
        ThingSpeak.setField(4, g.gyro.x);
        ThingSpeak.setField(5, g.gyro.y);
        ThingSpeak.setField(6, g.gyro.z);
        ThingSpeak.setField(7, temp.temperature);
        ThingSpeak.setField(8, val);

        int code = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

        if (code == 200)
        {
            Serial.println("Channel update successful.");
            success();
        }
        else
        {
            Serial.println("Problem updating channel. HTTP error code " + String(code));
            thingspeakError();
        }
    }

    delay(500);
}
