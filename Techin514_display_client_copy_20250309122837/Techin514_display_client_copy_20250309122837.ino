// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// #include "BLEDevice.h" //#include "BLEScan.h"
// #include <Stepper.h>  // stepper motor library
// #include <SwitecX25.h> 

// #define STEPS_PER_REV 315
// SwitecX25 motor(STEPS_PER_REV, D0, D1, D2, D3);

// #define LED_PIN 10

// // TODO: change the service UUID to the one you are using on the server side.
// static BLEUUID serviceUUID("5f32fce0-c0f2-4a44-b9b4-ef295eb5362f");
// static BLEUUID    charUUID("49d957ed-68ff-43e4-9ede-e695f5bbae8b");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) override {
//     Serial.println("Connected to BLE Server");
//   }

//   void onDisconnect(BLEClient* pclient) override {
//     Serial.println("Disconnected from BLE Server");
//     connected = false;
//   }
// };

// // ADD NEW CODE FOR TEM CONTROL STP (DON'T CHANGE)
// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
//     void onResult(BLEAdvertisedDevice advertisedDevice) {
//         Serial.print("BLE Advertised Device found: ");
//         Serial.println(advertisedDevice.toString().c_str());

//         if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
//             BLEDevice::getScan()->stop();
//             myDevice = new BLEAdvertisedDevice(advertisedDevice);
//             doConnect = true;
//             doScan = true;
//         }
//     }
// };

// // temperature and humidity setup 
// void adjustMotorPosition(float temp, float humidity) {
//     int targetAngle = 0;  // 默认 0° 代表正常状态

//     if (temp > 24) {
//         targetAngle = 90;  // 温度高 → 90°
//     } else if (temp < 18) {
//         targetAngle = 270;  // 温度低 → 270°
//     } else if (humidity < 80) {
//         targetAngle = 180;  // 湿度低 → 180°
//     }
    
//     // **控制 LED**
//     if (humidity < 80) {
//         digitalWrite(LED_PIN, HIGH);  // 湿度 < 80%，LED 亮
//         Serial.println("Humidity < 80%, LED ON");
//     } else {
//         digitalWrite(LED_PIN, LOW);   // 湿度正常，LED 熄灭
//         Serial.println("Humidity >= 80%, LED OFF");
//     }

//     Serial.print("Moving motor to: ");
//     Serial.print(targetAngle);
//     Serial.println(" degrees");

//     motor.setPosition(targetAngle);  // 让 Switec X27.168 旋转到这个角度
// }

// // BLE (DON'T CHANGE)
// static void notifyCallback(
//     BLERemoteCharacteristic* pBLERemoteCharacteristic,
//     uint8_t* pData,
//     size_t length,
//     bool isNotify) {

//     Serial.print("Received BLE data: ");
//     Serial.write(pData, length);
//     Serial.println();

//     String dataStr = String((char*)pData);  // 把 BLE 数据转换成 String
//     float temp = 0.0, humidity = 0.0;

//     // **解析温湿度数据**
//     int tempIndex = dataStr.indexOf("Temp: ");
//     int humIndex = dataStr.indexOf("Hum: ");

//     if (tempIndex != -1 && humIndex != -1) {
//         temp = dataStr.substring(tempIndex + 6, dataStr.indexOf("*", tempIndex)).toFloat();
//         humidity = dataStr.substring(humIndex + 5, dataStr.indexOf("%", humIndex)).toFloat();

//         Serial.print("Parsed Temperature: ");
//         Serial.print(temp);
//         Serial.println("°C");

//         Serial.print("Parsed Humidity: ");
//         Serial.print(humidity);
//         Serial.println("%");

//         // adjust motor 
//         adjustMotorPosition(temp, humidity);
//         }
//    }     

// /// Stepper motor
// void setup() {
//     Serial.begin(115200);
//     Serial.println("Starting Arduino BLE Client with Stepper Motor...");

//     pinMode(LED_PIN, OUTPUT);  // 设置 LED 引脚为输出模式
//     digitalWrite(LED_PIN, LOW); // 初始状态 LED 关闭

//     motor.zero();  // 让电机归零（回到 0°）

//     BLEDevice::init("");
//     BLEScan* pBLEScan = BLEDevice::getScan();
//     pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//     pBLEScan->setInterval(1349);
//     pBLEScan->setWindow(449);
//     pBLEScan->setActiveScan(true);
//     pBLEScan->start(5, false);
// }

// //     // Control the stepper motor based on BLE data
// //     if (length > 0) {
// //         if (pData[0] == '1') {  // If received '1', move stepper forward
// //             Serial.println("Moving stepper forward...");
// //             // stepperMotor.step(100);  // Move 100 steps forward
// //             motorForward = true;
// //             motorBackward = false;        
// //         } 
// //         else if (pData[0] == '0') {  // If received '0', move stepper backward
// //             Serial.println("Moving stepper backward...");
// //             // stepperMotor.step(-100);  // Move 100 steps backward
// //             motorForward = false;
// //             motorBackward = true;
// //         }
// //         else {  // **任何其他数据，停止电机**
// //             Serial.println("Stopping stepper motor...");
// //             motorForward = false;
// //             motorBackward = false;
// //         }
// //     }
// // }

// // BLE connection (Don't change)
// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) {
//   }

//   void onDisconnect(BLEClient* pclient) {
//     connected = false;
//     Serial.println("onDisconnect");
//   }
// };

// // BLE connection (Don't change)
// bool connectToServer() {
//     Serial.print("Forming a connection to ");
//     Serial.println(myDevice->getAddress().toString().c_str());

//     BLEClient* pClient = BLEDevice::createClient();
//     Serial.println(" - Created client");

//     pClient->setClientCallbacks(new BLEClientCallbacks());
//     pClient->connect(myDevice);
//     Serial.println(" - Connected to server");
//     pClient->setMTU(517);

//     BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//     if (pRemoteService == nullptr) {
//         Serial.println("Failed to find our service UUID!");
//         pClient->disconnect();
//         return false;
//     }
//     Serial.println(" - Found our service");

//     pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//     if (pRemoteCharacteristic == nullptr) {
//         Serial.println("Failed to find our characteristic UUID!");
//         pClient->disconnect();
//         return false;
//     }
//     Serial.println(" - Found our characteristic");

//     if (pRemoteCharacteristic->canNotify()) {
//         pRemoteCharacteristic->registerForNotify(notifyCallback);
//     }

//     connected = true;
//     return true;
// }
// /**
//  * Scan for BLE servers and find the first one that advertises the service we are looking for.
//  */
// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//   /**
//    * Called for each advertising BLE server.
//    */
//   void onResult(BLEAdvertisedDevice advertisedDevice) {
//     Serial.print("BLE Advertised Device found: ");
//     Serial.println(advertisedDevice.toString().c_str());

//     // We have found a device, let us now see if it contains the service we are looking for.
//     if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

//       BLEDevice::getScan()->stop();
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
//       doConnect = true;
//       doScan = true;

//     } // Found our server
//   } // onResult
// }; // MyAdvertisedDeviceCallbacks

// ////orginal stepper motor code
//   // // Initialize the stepper motor
//   // stepperMotor.setSpeed(60);  // Set stepper speed (RPM)

//   // Retrieve a Scanner and set the callback we want to use to be informed when we
//   // have detected a new device.  Specify that we want active scanning and start the
//   // scan to run for 5 seconds.
// //   BLEScan* pBLEScan = BLEDevice::getScan();
// //   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
// //   pBLEScan->setInterval(1349);
// //   pBLEScan->setWindow(449);
// //   pBLEScan->setActiveScan(true);
// //   pBLEScan->start(5, false);
// // } // End of setup.

// // This is the Arduino main loop function.
// void loop() {
//     if (doConnect) {
//         doConnect = !connectToServer();
//     }
//     motor.update();  //  让 Switec X27.168 平滑旋转
// }


//   // if (connected) {
//   //   String newValue = "Time since boot: " + String(millis()/1000);
//   //   Serial.println("Setting new characteristic value to \"" + newValue  + "\"");

//   //   // Set the characteristic's value to be the array of bytes that is actually a string.
//   //   pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
//   // }else if(doScan){
//   //   BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
//   // }




///2nd try

// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// #include <Stepper.h>
// #include <SwitecX25.h> 

// #define STEPS_PER_REV 315
// // 避免使用 D0、D1（串口引脚），将电机引脚改为2, 3, 4, 5
// SwitecX25 motor(STEPS_PER_REV, D0, D1, D2, D3);

// #define LED_PIN 8

// // 修改服务 UUID 和特征 UUID 为你实际使用的值
// static BLEUUID serviceUUID("5f32fce0-c0f2-4a44-b9b4-ef295eb5362f");
// static BLEUUID charUUID("49d957ed-68ff-43e4-9ede-e695f5bbae8b");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

// // 客户端回调：处理连接和断开事件
// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) override {
//     Serial.println("Connected to BLE Server");
//   }

//   void onDisconnect(BLEClient* pclient) override {
//     Serial.println("Disconnected from BLE Server");
//     connected = false;
//   }
// };

// // 广播设备回调：当扫描到设备时检查服务 UUID
// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
//   void onResult(BLEAdvertisedDevice advertisedDevice) override {
//     Serial.print("BLE Advertised Device found: ");
//     Serial.println(advertisedDevice.toString().c_str());
//     if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
//       BLEDevice::getScan()->stop();
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
//       doConnect = true;
//       doScan = true;
//     }
//   }
// };

// // 根据温湿度数据调整电机位置，并控制 LED 状态
// void adjustMotorPosition(float temp, float humidity) {
//   int targetAngle = 0;  // 默认角度

//   if (temp > 24) {
//     targetAngle = 90;  // 温度高 → 90°
//   } else if (temp < 18) {
//     targetAngle = 270;  // 温度低 → 270°
//   } 
//   // 如有需要也可以加湿度判断

//   // LED 控制：湿度低时点亮 LED
//   if (humidity < 80) {
//     digitalWrite(LED_PIN, HIGH);
//     Serial.println("Humidity < 80%, LED ON");
//   } else {
//     digitalWrite(LED_PIN, LOW);
//     Serial.println("Humidity >= 80%, LED OFF");
//   }

//   Serial.print("Moving motor to: ");
//   Serial.print(targetAngle);
//   Serial.println(" degrees");
//   motor.setPosition(targetAngle);
// }

// // BLE 通知回调函数：解析温湿度数据，并调用电机调整函数
// static void notifyCallback(
//   BLERemoteCharacteristic* pBLERemoteCharacteristic,
//   uint8_t* pData,
//   size_t length,
//   bool isNotify) {

//   Serial.print("Received BLE data: ");
//   Serial.write(pData, length);
//   Serial.println();

//   String dataStr = String((char*)pData);
//   float temp = 0.0, humidity = 0.0;
//   int tempIndex = dataStr.indexOf("Temp: ");
//   int humIndex = dataStr.indexOf("Hum: ");

//   if (tempIndex != -1 && humIndex != -1) {
//     temp = dataStr.substring(tempIndex + 6, dataStr.indexOf("*", tempIndex)).toFloat();
//     humidity = dataStr.substring(humIndex + 5, dataStr.indexOf("%", humIndex)).toFloat();

//     Serial.print("Parsed Temperature: ");
//     Serial.print(temp);
//     Serial.println("°C");

//     Serial.print("Parsed Humidity: ");
//     Serial.print(humidity);
//     Serial.println("%");

//     adjustMotorPosition(temp, humidity);
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting Arduino BLE Client with Stepper Motor...");

//   pinMode(LED_PIN, OUTPUT);
//   digitalWrite(LED_PIN, LOW);

//   motor.zero();  // 电机归零

//   BLEDevice::init("");
//   BLEScan* pBLEScan = BLEDevice::getScan();
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//   pBLEScan->setInterval(1349);
//   pBLEScan->setWindow(449);
//   pBLEScan->setActiveScan(true);
//   pBLEScan->start(5, false);
// }

// // 连接到 BLE 服务器，并注册通知回调
// bool connectToServer() {
//   Serial.print("Forming a connection to ");
//   Serial.println(myDevice->getAddress().toString().c_str());

//   BLEClient* pClient = BLEDevice::createClient();
//   Serial.println(" - Created client");

//   pClient->setClientCallbacks(new MyClientCallback());
//   pClient->connect(myDevice);
//   Serial.println(" - Connected to server");
//   pClient->setMTU(517);

//   BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//   if (pRemoteService == nullptr) {
//     Serial.println("Failed to find our service UUID!");
//     pClient->disconnect();
//     return false;
//   }
//   Serial.println(" - Found our service");

//   pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//   if (pRemoteCharacteristic == nullptr) {
//     Serial.println("Failed to find our characteristic UUID!");
//     pClient->disconnect();
//     return false;
//   }
//   Serial.println(" - Found our characteristic");

//   if (pRemoteCharacteristic->canNotify()) {
//     pRemoteCharacteristic->registerForNotify(notifyCallback);
//   }

//   connected = true;
//   return true;
// }

// void loop() {
//   if (doConnect) {
//     doConnect = !connectToServer();
//   }
//   motor.update();  // 需要频繁调用以驱动电机
// }



////3rd try
// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEScan.h>
// #include <BLEAdvertisedDevice.h>
// #include <Stepper.h>
// #include <SwitecX25.h> 

// #define STEPS_PER_REV 315
// // 使用数字引脚2,3,4,5控制电机
// SwitecX25 motor(STEPS_PER_REV, 2, 3, 4, 5);

// // 定义温度指示LED引脚：红色和蓝色
// #define RED_LED_PIN 8
// #define BLUE_LED_PIN 9

// // 修改服务 UUID 和特征 UUID 为你实际使用的值
// static BLEUUID serviceUUID("5f32fce0-c0f2-4a44-b9b4-ef295eb5362f");
// static BLEUUID charUUID("49d957ed-68ff-43e4-9ede-e695f5bbae8b");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

// // 全局变量用于存储最新的温湿度数据及电机控制标志
// bool shouldRotate = false;
// int currentAngle = 0;
// float latestTemp = 0.0;
// float latestHumidity = 100.0;  // 默认湿度较高，不旋转

// // 客户端回调：处理连接和断开事件
// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) override {
//     Serial.println("Connected to BLE Server");
//   }
//   void onDisconnect(BLEClient* pclient) override {
//     Serial.println("Disconnected from BLE Server");
//     connected = false;
//   }
// };

// // 广播设备回调：扫描到设备后判断是否包含目标服务UUID
// class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
//   void onResult(BLEAdvertisedDevice advertisedDevice) override {
//     Serial.print("BLE Advertised Device found: ");
//     Serial.println(advertisedDevice.toString().c_str());
//     if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
//       BLEDevice::getScan()->stop();
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
//       doConnect = true;
//       doScan = true;
//     }
//   }
// };

// // BLE通知回调：解析温湿度数据，并更新LED状态和电机旋转标志
// static void notifyCallback(
//   BLERemoteCharacteristic* pBLERemoteCharacteristic,
//   uint8_t* pData,
//   size_t length,
//   bool isNotify) {

//   Serial.print("Received BLE data: ");
//   Serial.write(pData, length);
//   Serial.println();

//   String dataStr = String((char*)pData);
//   float temp = 0.0, humidity = 0.0;
//   int tempIndex = dataStr.indexOf("Temp: ");
//   int humIndex = dataStr.indexOf("Hum: ");

//   if (tempIndex != -1 && humIndex != -1) {
//     // 解析温度和湿度数据（假设格式类似 "Temp: 28.54*C, Hum: 34.90%"）
//     temp = dataStr.substring(tempIndex + 6, dataStr.indexOf("*", tempIndex)).toFloat();
//     humidity = dataStr.substring(humIndex + 5, dataStr.indexOf("%", humIndex)).toFloat();

//     Serial.print("Parsed Temperature: ");
//     Serial.print(temp);
//     Serial.println("°C");

//     Serial.print("Parsed Humidity: ");
//     Serial.print(humidity);
//     Serial.println("%");

//     latestTemp = temp;
//     latestHumidity = humidity;

//     // 温度LED指示：温度 > 24°C 红色LED亮，<= 24°C 蓝色LED亮
//     if (temp > 27) {
//       digitalWrite(RED_LED_PIN, HIGH);
//       digitalWrite(BLUE_LED_PIN, LOW);
//       Serial.println("Temperature > 25°C: Red LED ON, Blue LED OFF");
//     } else {
//       digitalWrite(RED_LED_PIN, LOW);
//       digitalWrite(BLUE_LED_PIN, HIGH);
//       Serial.println("Temperature <= 24°C: Blue LED ON, Red LED OFF");
//     }

//     // 湿度判断：湿度 < 80% 时电机自动旋转
//     if (humidity < 80) {
//       shouldRotate = true;
//       Serial.println("Humidity < 80%: Motor will rotate");
//     } else {
//       shouldRotate = false;
//       Serial.println("Humidity >= 80%: Motor stops rotating");
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting Arduino BLE Client with Stepper Motor...");

//   // 初始化温度LED引脚
//   pinMode(RED_LED_PIN, OUTPUT);
//   pinMode(BLUE_LED_PIN, OUTPUT);
//   digitalWrite(RED_LED_PIN, LOW);
//   digitalWrite(BLUE_LED_PIN, LOW);

//   // 电机初始化（归零）
//   motor.zero();

//   // 初始化BLE
//   BLEDevice::init("");
//   BLEScan* pBLEScan = BLEDevice::getScan();
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//   pBLEScan->setInterval(1349);
//   pBLEScan->setWindow(449);
//   pBLEScan->setActiveScan(true);
//   pBLEScan->start(5, false);
// }

// // 连接到BLE服务器，并注册通知回调
// bool connectToServer() {
//   Serial.print("Forming a connection to ");
//   Serial.println(myDevice->getAddress().toString().c_str());

//   BLEClient* pClient = BLEDevice::createClient();
//   Serial.println(" - Created client");

//   pClient->setClientCallbacks(new MyClientCallback());
//   pClient->connect(myDevice);
//   Serial.println(" - Connected to server");
//   pClient->setMTU(517);

//   BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//   if (pRemoteService == nullptr) {
//     Serial.println("Failed to find our service UUID!");
//     pClient->disconnect();
//     return false;
//   }
//   Serial.println(" - Found our service");

//   pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//   if (pRemoteCharacteristic == nullptr) {
//     Serial.println("Failed to find our characteristic UUID!");
//     pClient->disconnect();
//     return false;
//   }
//   Serial.println(" - Found our characteristic");

//   if (pRemoteCharacteristic->canNotify()) {
//     pRemoteCharacteristic->registerForNotify(notifyCallback);
//   }

//   connected = true;
//   return true;
// }

// void loop() {
//   if (doConnect) {
//     doConnect = !connectToServer();
//   }
  
//   // 如果湿度 < 80%，则让电机持续旋转
//   if (shouldRotate) {
//     currentAngle = (currentAngle + 10) % 360;  // 每次增量10度
//     motor.setPosition(currentAngle);
//     Serial.print("Rotating motor to: ");
//     Serial.print(currentAngle);
//     Serial.println(" degrees");
//   }
  
//   motor.update();  // 持续调用以驱动电机
//   delay(10);
// }


///4th correction
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// --- 步进电机控制部分 ---
// 定义电机控制引脚（注意：部分开发板D0、D1可能保留给串口通信，如有需要请修改）
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
    for (int i = 0; i < 200; i++) { // 200个步（可根据实际电机步数调整）
        for (int step = 0; step < 4; step++) {
            int s = reverse ? (3 - step) : step; // 反向控制
            digitalWrite(COIL_A1, stepSequence[s][0]);
            digitalWrite(COIL_A2, stepSequence[s][1]);
            digitalWrite(COIL_B1, stepSequence[s][2]);
            digitalWrite(COIL_B2, stepSequence[s][3]);
            delay(stepDelay); // 转速控制（延时越短转得越快）
        }
    }
}

// --- LED定义 ---
// 红色LED和蓝色LED分别用于温度指示
#define RED_LED_PIN 8
#define BLUE_LED_PIN 9

// --- BLE相关定义 ---
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

// --- BLE客户端回调 ---
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) override {
    Serial.println("Connected to BLE Server");
  }
  void onDisconnect(BLEClient* pclient) override {
    Serial.println("Disconnected from BLE Server");
    connected = false;
  }
};

// --- BLE广播设备回调 ---
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// --- BLE通知回调 ---
// 预期数据格式示例："Temp: 28.54*C, Hum: 34.90%"
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

        // 温度LED指示：温度 > 24°C时红色LED亮，温度 < 24°C时蓝色LED亮
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

        // 湿度判断：湿度 < 80%时启动电机旋转
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

    // 初始化LED引脚
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, LOW);

    // 初始化BLE
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

// 连接到BLE服务器，并注册通知回调
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

    // 如果湿度 < 80%，则调用 stepMotor() 旋转电机（本例为正转，延时10ms）
    if (shouldRotate) {
        stepMotor(20, false);
    } else {
        // 停止电机：关闭所有相
        digitalWrite(COIL_A1, LOW);
        digitalWrite(COIL_A2, LOW);
        digitalWrite(COIL_B1, LOW);
        digitalWrite(COIL_B2, LOW);
        delay(10);
    }
}
