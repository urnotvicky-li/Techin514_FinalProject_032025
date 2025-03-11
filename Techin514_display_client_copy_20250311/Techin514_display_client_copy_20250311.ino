#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//
#define COIL_A1 D0
#define COIL_A2 D1
#define COIL_B1 D2
#define COIL_B2 D3

// 全步进模式步序
const int stepSequence[4][4] = {
    {1, 0, 1, 0}, // Step 1
    {0, 1, 1, 0}, // Step 2
    {0, 1, 0, 1}, // Step 3
    {1, 0, 0, 1}  // Step 4
};

// 初始化步进电机引脚
void setupMotorPins() {
    pinMode(COIL_A1, OUTPUT);
    pinMode(COIL_A2, OUTPUT);
    pinMode(COIL_B1, OUTPUT);
    pinMode(COIL_B2, OUTPUT);
}

// 按照给定的转速和方向转动电机（阻塞函数）
// 参数 stepDelay 控制步进延时（转速），reverse 为 true 时反转
void stepMotor(int stepDelay, bool reverse) {
    for (int i = 0; i < 200; i++) { // 200 个步（可根据实际电机步数调整）
        for (int step = 0; step < 4; step++) {
            int s = reverse ? (3 - step) : step; // 反向控制
            digitalWrite(COIL_A1, stepSequence[s][0]);
            digitalWrite(COIL_A2, stepSequence[s][1]);
            digitalWrite(COIL_B1, stepSequence[s][2]);
            digitalWrite(COIL_B2, stepSequence[s][3]);
            delay(stepDelay); // 延时越短，转速越快
        }
    }
}

// --- LED 定义 ---
// 红色 LED 和蓝色 LED 分别用于温度指示
#define BLUE_LED_PIN 8
#define RED_LED_PIN 9

// --- BLE 相关定义 ---
static BLEUUID serviceUUID("5f32fce0-c0f2-4a44-b9b4-ef295eb5362f");
static BLEUUID charUUID("49d957ed-68ff-43e4-9ede-e695f5bbae8b");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

// 全局变量：存储最新温湿度数据及电机旋转标志
float latestTemp = 0.0;
float latestHumidity = 100.0; // 默认湿度较高，不旋转
bool shouldRotate = false;

// --- BLE 客户端回调 ---
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) override {
    Serial.println("Connected to BLE Server");
  }
  void onDisconnect(BLEClient* pclient) override {
    Serial.println("Disconnected from BLE Server");
    connected = false;
  }
};

// --- BLE 广播设备回调 ---
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveServiceUUID() && 
        advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// --- BLE 通知回调 ---
// 数据格式示例："Temp: 28.54*C, Hum: 34.90%"
static void notifyCallback(
    BLERemoteCharacteristic* pRemoteCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify) {

    Serial.print("Received BLE data: ");
    Serial.write(pData, length);
    Serial.println();

    String dataStr = String((char*)pData);
    float temp = 0.0;
    float humidity = 0.0;
    int tempIndex = dataStr.indexOf("Temp: ");
    int humIndex = dataStr.indexOf("Hum: ");

    if (tempIndex != -1 && humIndex != -1) {
        temp = dataStr.substring(tempIndex + 6, dataStr.indexOf("*", tempIndex)).toFloat();
        humidity = dataStr.substring(humIndex + 5, dataStr.indexOf("%", humIndex)).toFloat();

        Serial.print("Parsed Temperature: ");
        Serial.print(temp);
        Serial.println("°C");

        Serial.print("Parsed Humidity: ");
        Serial.print(humidity);
        Serial.println("%");

        latestTemp = temp;
        latestHumidity = humidity;

        // 温度 LED 指示：温度 > 24°C 时红色 LED 亮，温度 < 24°C 时蓝色 LED 亮
        if (temp > 24) {
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(BLUE_LED_PIN, LOW);
            Serial.println("Temperature > 24°C: Red LED ON, Blue LED OFF");
        } else if (temp < 24) {
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, HIGH);
            Serial.println("Temperature < 24°C: Blue LED ON, Red LED OFF");
        } else {
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(BLUE_LED_PIN, LOW);
            Serial.println("Temperature == 24°C: Both LEDs OFF");
        }

        // 湿度判断：湿度 < 80% 时启动电机旋转
        if (humidity < 80) {
            shouldRotate = true;
            Serial.println("Humidity < 80%: Motor will rotate");
        } else {
            shouldRotate = false;
            Serial.println("Humidity >= 80%: Motor stops rotating");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Client with Stepper Motor...");

    // 初始化步进电机引脚
    setupMotorPins();

    // 初始化 LED 引脚
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);

    // 初始化 BLE
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

// 连接到 BLE 服务器，并注册通知回调
bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient* pClient = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());
    pClient->connect(myDevice);
    Serial.println(" - Connected to server");
    pClient->setMTU(517);

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.println("Failed to find our service UUID!");
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our service");

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.println("Failed to find our characteristic UUID!");
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");

    if (pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notifyCallback);
    }

    connected = true;
    return true;
}

void loop() {
    if (doConnect) {
        doConnect = !connectToServer();
    }

    // 如果湿度 < 80%，则调用 stepMotor() 旋转电机（正转，延时 20ms）
    if (shouldRotate) {
        stepMotor(10, false);
        delay(1000);
        stepMotor(10, true); // Move backward
        delay(1000);
    } else {
        // 停止电机：关闭所有相
        digitalWrite(COIL_A1, LOW);
        digitalWrite(COIL_A2, LOW);
        digitalWrite(COIL_B1, LOW);
        digitalWrite(COIL_B2, LOW);
        delay(10);
    }

}

