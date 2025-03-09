#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "BLEDevice.h" //#include "BLEScan.h"
#include <Stepper.h>  // stepper motor library

////orginal stepper motor code
// Define stepper motor parameters
// #define STEPS_PER_REV 200  // Steps per revolution (adjust per motor specs)
// #define STEP_PIN_1 0       // Stepper motor pin 1
// #define STEP_PIN_2 1       // Stepper motor pin 2
// #define STEP_PIN_3 2      // Stepper motor pin 3
// #define STEP_PIN_4 3      // Stepper motor pin 4

// NEW: Define motor control pins
#define COIL_A1 D0
#define COIL_A2 D1
#define COIL_B1 D2
#define COIL_B2 D3

// NEW: Stepper motor step sequence (full step mode)
const int stepSequence[4][4] = {
    {1, 0, 1, 0}, // Step 1
    {0, 1, 1, 0}, // Step 2
    {0, 1, 0, 1}, // Step 3
    {1, 0, 0, 1}  // Step 4
};

//NEW 
bool motorForward = false;  
bool motorBackward = false; 

Stepper stepperMotor(STEPS_PER_REV, STEP_PIN_1, STEP_PIN_2, STEP_PIN_3, STEP_PIN_4);

// Client Code
// TODO: change the service UUID to the one you are using on the server side.
// The remote service we wish to connect to.
static BLEUUID serviceUUID("5f32fce0-c0f2-4a44-b9b4-ef295eb5362f");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("49d957ed-68ff-43e4-9ede-e695f5bbae8b");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

/// Stepper motor
void setup() {
    Serial.begin(115200);
    Serial.println("Starting Arduino BLE Client with Stepper Motor...");

    pinMode(COIL_A1, OUTPUT);
    pinMode(COIL_A2, OUTPUT);
    pinMode(COIL_B1, OUTPUT);
    pinMode(COIL_B2, OUTPUT);

    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

////NEW code for Stepper motor
void stepMotor(int stepDelay, bool reverse) {
    for (int i = 0; i < 200; i++) { // Adjust steps per revolution
        for (int step = 0; step < 4; step++) {
            int s = reverse ? (3 - step) : step; // Reverse direction if needed
            digitalWrite(COIL_A1, stepSequence[s][0]);
            digitalWrite(COIL_A2, stepSequence[s][1]);
            digitalWrite(COIL_B1, stepSequence[s][2]);42
            digitalWrite(COIL_B2, stepSequence[s][3]);
            delay(stepDelay); // Adjust speed (smaller delay = faster motor)
        }
    }
}


// TODO: define new global variables for data collection

// TODO: define a new function for data aggregation

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    // TODO: add codes to handle the data received from the server, and call the data aggregation function to process the data

    // TODO: change the following code to customize your own data format for printing
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.write(pData, length);
    Serial.println();


    // Control the stepper motor based on BLE data
    if (length > 0) {
        if (pData[0] == '1') {  // If received '1', move stepper forward
            Serial.println("Moving stepper forward...");
            // stepperMotor.step(100);  // Move 100 steps forward
            motorForward = true;
            motorBackward = false;        
        } 
        else if (pData[0] == '0') {  // If received '0', move stepper backward
            Serial.println("Moving stepper backward...");
            // stepperMotor.step(-100);  // Move 100 steps backward
            motorForward = false;
            motorBackward = true;
        }
        else {  // **任何其他数据，停止电机**
            Serial.println("Stopping stepper motor...");
            motorForward = false;
            motorBackward = false;
        }
    }

}


// BLE connection (Don't change)
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// BLE connection (Don't change)
bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      String value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify()) {
      // pRemoteCharacteristic->registerForNotify(notifyCallback);
      String value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());      
    }
    if (pRemoteCharacteristic->canNotify()){
      pRemoteCharacteristic->registerForNotify(notifyCallback);  
    }
    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Starting Arduino BLE Client application...");
//   BLEDevice::init("");


////orginal stepper motor code
  // // Initialize the stepper motor
  // stepperMotor.setSpeed(60);  // Set stepper speed (RPM)

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

// This is the Arduino main loop function.
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
 
  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue  + "\"");

    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }


  ///orginal code
  // stepperMotor.step(100);
  // delay(1000); // Delay a second between loops.

  //NEW
  if (motorForward) {
    Serial.println("Stepper Motor Forward...");
    stepMotor(10, false);  // **正转**
  } 
  else if (motorBackward) {
    Serial.println("Stepper Motor Backward...");
    stepMotor(10, true);  // **反转**
  }


} // End of loop