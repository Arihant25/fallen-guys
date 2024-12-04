# Central Node - Fall Detection System Hub 🏢

## Overview 🌟

The Central Node serves as the primary hub in our fall detection system, managing communication between user nodes and the ThingSpeak cloud platform. It processes incoming sensor data, manages thresholds, and coordinates system-wide alerts.

## System Architecture 🏗️

- WiFi connectivity for cloud communication
- LoRa radio for user node interaction
- Real-time data processing
- Cloud data synchronization

## Hardware Requirements 🛠️

- ESP32 Microcontroller
- LoRa Module
- Status LEDs
- Buzzer
- Stable power supply

## Pin Configuration 📌

```cpp
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2
#define LED 22
#define REVERSE_LED 4
#define BUZZER_IO 27
```

## Key Features 🔑

### 1. Network Communication 🌐

- WiFi connectivity with timeout handling
- LoRa radio communication
- ThingSpeak integration
- Robust error handling

### 2. Data Management 📊

- Real-time data processing
- Threshold management
- Secure data transmission
- Checksum verification

### 3. Cloud Integration ☁️

- ThingSpeak channel management
- API key authentication
- Multi-field data updates
- Threshold synchronization

### 4. Security Features 🔒

- Password protection
- Checksum validation
- Secure API keys
- Data verification

## ThingSpeak Configuration 📡

### Channels

1. **Data Write Channel**

   - Channel Number: 2684114
   - 8 data fields for sensor metrics

2. **Threshold Read Channel**
   - Channel Number: 2678150
   - Stores system configuration

### Data Fields 📊

1. Acceleration X
2. Acceleration Y
3. Acceleration Z
4. Maximum Acceleration Magnitude
5. Maximum Net Gyro
6. Fall Detection Status
7. GPS Location
8. System Status

## Communication Protocols 🤝

### LoRa Data Format

```
Incoming: sensorData:checksum
Outgoing: THRESHOLDS:fallAcc,alert,emergency
```

### ThingSpeak Integration

- 15-second update interval
- Multi-field updates
- Threshold synchronization
- Error handling

## System Operations ⚙️

### Initialization

1. WiFi connection
2. LoRa setup
3. ThingSpeak configuration
4. Threshold synchronization

### Runtime Operations

- Data processing
- Cloud synchronization
- Alert management
- Threshold updates

## Error Handling 🔧

- WiFi connection failures
- ThingSpeak communication errors
- Data validation
- Checksum verification

## Performance Optimization 🚀

- Efficient data processing
- Optimized update intervals
- Memory management
- Battery efficiency

## Maintenance Guidelines 🔍

### Regular Checks

- WiFi connectivity
- ThingSpeak channel status
- LoRa communication
- System logs

### Troubleshooting

1. Check WiFi status
2. Verify ThingSpeak credentials
3. Monitor LoRa communication
4. Review system alerts

## Configuration Variables ⚡

```cpp
const unsigned long WIFI_TIMEOUT = 10000;
const unsigned long THINGSPEAK_INTERVAL = 15000;
```

## Security Considerations 🛡️

- Secure API key storage
- Password protection
- Data encryption
- Access control

## Development Notes 📝

- Regular code updates
- Performance monitoring
- Security patches
- Feature enhancements

## Contributing 🤝

Contributions are welcome! Please read our contributing guidelines.
