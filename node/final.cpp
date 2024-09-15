#include <DHT.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <ArduinoJson.h>

// TODO: replace placeholders with actual values
#define THRESHOLD_PASS "THRESHOLD_PASS"

// TODO: replace placeholders with actual values
#define WiFi_SSID "SSID"
#define WiFi_PASS "PASSWORD"

#define FLAME_SEN_DIGITAL_PIN 5
#define FLAME_SEN_ANALOG_PIN 34

#define SMOKE_SEN_DIGITAL_PIN 2
#define SMOKE_SEN_ANALOG_PIN 35

#define DHTPIN 23
#define DHTTYPE DHT11

#define BUZZER_PIN 22

#define LED_R 21
#define LED_G 19
#define LED_B 18

DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

// TODO: replace placeholders with actual values
unsigned long dataChannelNumber = 123456789;
const char *dataWriteAPIKey = "WRITE_API_KEY";

// TODO: replace placeholders with actual values
unsigned long thresholdChannelNumber = 123456789;
const char *thresholdReadAPIKey = "READ_API_KEY";

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

void fireDetected()
{
    for (int i = 0; i < 256; i++)
    {
        analogWrite(LED_R, i);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
        delay(5);
    }
}

void initializePins()
{
    pinMode(FLAME_SEN_DIGITAL_PIN, INPUT);
    pinMode(FLAME_SEN_ANALOG_PIN, INPUT);

    pinMode(SMOKE_SEN_DIGITAL_PIN, INPUT);
    pinMode(SMOKE_SEN_ANALOG_PIN, INPUT);

    pinMode(DHTPIN, INPUT);

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
    dht.begin();

    connectToNetwork();

    ThingSpeak.begin(client);

    delay(2000);
}

int previousHumidityReading = -1;

// Initialize the threshold values
float temperatureThreshold = 50;
float heatIndexThreshold = 55;
int humidityChangeThreshold = 20;
int smokeAnalogThreshold = 1000;
float flameThreshold = 1500.0;

void loop()
{
    connectToNetwork();

    float humidityReading = dht.readHumidity();
    float temperatureReadingCelsius = dht.readTemperature();
    float temperatureReadingFahrenheit = dht.readTemperature(true);

    if (isnan(humidityReading) || isnan(temperatureReadingCelsius) || isnan(temperatureReadingFahrenheit))
    {
        Serial.println("Failed to read from DHT sensor!");
        error();
    }
    else
    {

        float heatIndexFahrenheit = dht.computeHeatIndex(temperatureReadingFahrenheit, humidityReading);
        float heatIndexCelsius = dht.computeHeatIndex(temperatureReadingCelsius, humidityReading, false);

        Serial.print("Humidity: ");
        Serial.print(humidityReading);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(temperatureReadingCelsius);
        Serial.print(" *C ");
        Serial.print(temperatureReadingFahrenheit);
        Serial.print(" *F\t");
        Serial.print("Heat index: ");
        Serial.print(heatIndexCelsius);
        Serial.print(" *C ");
        Serial.print(heatIndexFahrenheit);
        Serial.println(" *F");

        int smokeDigitalReading = digitalRead(SMOKE_SEN_DIGITAL_PIN);
        int smokeAnalogReading = analogRead(SMOKE_SEN_ANALOG_PIN);

        Serial.print("Smoke Digital: ");
        Serial.print(smokeDigitalReading);
        Serial.print("\t");
        Serial.print("Smoke Analog: ");
        Serial.println(smokeAnalogReading);

        int flameDigitalReading = digitalRead(FLAME_SEN_DIGITAL_PIN);
        float flameAnalogReading = analogRead(FLAME_SEN_ANALOG_PIN);

        Serial.print("Flame Digital: ");
        Serial.print(flameDigitalReading);
        Serial.print("\t");
        Serial.print("Flame Analog: ");
        Serial.println(flameAnalogReading);

        // fetch the thresholds from thingspeak server
        int status_code = ThingSpeak.readMultipleFields(thresholdChannelNumber, thresholdReadAPIKey);
        if (status_code == 200)
        {
            std::string result = ThingSpeak.readStringField(thresholdChannelNumber, 5, thresholdReadAPIKey).c_str();
            std::string expected = THRESHOLD_PASS;

            if (result == expected)
            {
                float tempTemperatureThreshold = ThingSpeak.readStringField(thresholdChannelNumber, 1, thresholdReadAPIKey).toFloat();
                float tempHeatIndexThreshold = ThingSpeak.readStringField(thresholdChannelNumber, 2, thresholdReadAPIKey).toFloat();
                int tempHumidityChangeThreshold = ThingSpeak.readStringField(thresholdChannelNumber, 3, thresholdReadAPIKey).toInt();
                int tempSmokeAnalogThreshold = ThingSpeak.readStringField(thresholdChannelNumber, 4, thresholdReadAPIKey).toInt();
                float tempFlameThreshold = ThingSpeak.readStringField(thresholdChannelNumber, 6, thresholdReadAPIKey).toFloat();
                if (!tempTemperatureThreshold || !tempHeatIndexThreshold || !tempHumidityChangeThreshold || !tempSmokeAnalogThreshold || !tempFlameThreshold)
                    Serial.println("Invalid threshold value(s) fetched, skipping update.");
                else
                {
                    temperatureThreshold = tempTemperatureThreshold;
                    heatIndexThreshold = tempHeatIndexThreshold;
                    humidityChangeThreshold = tempHumidityChangeThreshold;
                    smokeAnalogThreshold = tempSmokeAnalogThreshold;
                    flameThreshold = tempFlameThreshold;
                    Serial.println();
                    Serial.print("Temperature threshold: ");
                    Serial.println(temperatureThreshold);
                    Serial.print("Heat Index threshold: ");
                    Serial.println(heatIndexThreshold);
                    Serial.print("Humidity threshold: ");
                    Serial.println(humidityChangeThreshold);
                    Serial.print("Smoke threshold: ");
                    Serial.println(smokeAnalogThreshold);
                    Serial.print("Flame threshold: ");
                    Serial.println(flameThreshold);
                    Serial.println();
                }
            }
            else
            {
                Serial.print("Invalid threshold password, ");
                Serial.println(result.c_str());
            }
        }
        else
        {
            Serial.print(status_code);
            Serial.print("\t");
            Serial.println("Error reading ThingSpeak channel");
        }

        bool fire = false;

        ThingSpeak.setField(1, temperatureReadingCelsius);
        ThingSpeak.setField(2, heatIndexCelsius);
        ThingSpeak.setField(3, humidityReading);
        ThingSpeak.setField(4, smokeDigitalReading);
        ThingSpeak.setField(5, smokeAnalogReading);
        ThingSpeak.setField(7, flameAnalogReading);
        ThingSpeak.setField(8, flameDigitalReading);

        if (temperatureReadingCelsius > temperatureThreshold || heatIndexCelsius > heatIndexThreshold || (previousHumidityReading != -1 && abs(previousHumidityReading - humidityReading) > humidityChangeThreshold) || smokeAnalogReading > smokeAnalogThreshold || flameAnalogReading < flameThreshold)
        {
            fire = true;
            digitalWrite(BUZZER_PIN, LOW);
            ThingSpeak.setField(6, 1);
        }
        else
        {
            digitalWrite(BUZZER_PIN, HIGH);
            ThingSpeak.setField(6, 0);
        }

        previousHumidityReading = humidityReading;

        int code = ThingSpeak.writeFields(dataChannelNumber, dataWriteAPIKey);
        if (code == 200)
        {
            Serial.println("Channel update successful.");
            if (fire)
                fireDetected();
            else
                success();
        }
        else
        {
            Serial.println("Problem updating channel. HTTP error code " + String(code));
            thingspeakError();
        }
    }

    delay(30000);
}
