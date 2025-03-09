#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdlib.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "SwitecX25.h" //stepper motor


#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
unsigned long previousMillis = 0;
const long interval = 1000;

// TODO: add new global variables for your sensor readings and processed data

// TODO: Change the UUID to your own (any specific one works, but make sure they're different from others'). You can generate one here: https://www.uuidgenerator.net/
#define SERVICE_UUID        "5f32fce0-c0f2-4a44-b9b4-ef295eb5362f"
#define CHARACTERISTIC_UUID "49d957ed-68ff-43e4-9ede-e695f5bbae8b"

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

// TODO: add DSP algorithm functions here

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");



    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }  

    // TODO: add codes for handling your sensor setup (pinMode, etc.)

    // TODO: name your device to avoid conflictions
    BLEDevice::init("XIAO_ESP32S3");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());
    // pCharacteristic->setValue("Hello World");

    char initialData[100];
    snprintf(initialData, sizeof(initialData), "Temp: %.2f*C, Pres: %.2f hPa, Alt: %.2f m, Hum: %.2f%%",
            bme.readTemperature(),
            bme.readPressure() / 100.0F,
            bme.readAltitude(SEALEVELPRESSURE_HPA),
            bme.readHumidity());
    pCharacteristic->setValue(initialData);

    
    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

// void loop() {
    // TODO: add codes for handling your sensor readings (analogRead, etc.)

    // TODO: use your defined DSP algorithm to process the readings
    
    // if (deviceConnected) {
    //     // Send new readings to database
    //     // TODO: change the following code to send your own readings and processed data
    //     unsigned long currentMillis = millis();
    //     if (currentMillis - previousMillis >= interval) {
    //     pCharacteristic->setValue("Hello World");
    //     pCharacteristic->notify();
    //     Serial.println("Notify value: Hello World");
    //     }
    // }

void loop() {
    unsigned long currentMillis = millis();
    if (deviceConnected && (currentMillis - previousMillis >= interval)) {
        previousMillis = currentMillis;
        
        // read bme
        float temperature = bme.readTemperature();
        float pressure = bme.readPressure() / 100.0F;
        float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
        float humidity = bme.readHumidity();
        
        // 格式化数据字符串
        char sensorData[100];
        snprintf(sensorData, sizeof(sensorData), "Temp: %.2f*C, Pres: %.2f hPa, Alt: %.2f m, Hum: %.2f%%",
                 temperature, pressure, altitude, humidity);
        
        // 更新BLE特征值并发送通知
        pCharacteristic->setValue(sensorData);
        pCharacteristic->notify();
        
        Serial.print("Notify sensor data: ");
        Serial.println(sensorData);
    }


    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500);  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising();  // advertise again
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    delay(1000);
}



// code for BME280

// #include <Wire.h>
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>

// #define SEALEVELPRESSURE_HPA (1013.25)

// Adafruit_BME280 bme;

// void setup() {
//         Serial.begin(9600);

//         if (!bme.begin(0x76)) {
//                 Serial.println("Could not find a valid BME280 sensor, check wiring!");
//                 while (1);
//         }
// }

// void loop() {
//         Serial.print("Temperature = ");
//         Serial.print(bme.readTemperature());
//         Serial.println("*C");

//         Serial.print("Pressure = ");
//         Serial.print(bme.readPressure() / 100.0F);
//         Serial.println("hPa");

//         Serial.print("Approx. Altitude = ");
//         Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//         Serial.println("m");

//         Serial.print("Humidity = ");
//         Serial.print(bme.readHumidity());
//         Serial.println("%");

//         Serial.println();
//         delay(1000);
// }



