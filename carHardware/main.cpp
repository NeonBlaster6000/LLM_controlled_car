#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <sstream>

#define ENA 4
#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 15
#define ENB 16

// pwm settings
// ledc channel
 const int PWM_CHANNELA = 0;
 const int PWM_CHANNELB = 1;
 // 20 0000 kHz
 const int PWM_FREQ = 2000;
 //8 bit (0-255)
 const int PWM_RESOLUTION = 8;
 
//Functions declarations
void reverse();
void forward();
void stop();
void right();
void left();
void speedX(uint8_t speed);
//UUIDS
#define SERVICE_UUID "27d3bd94-4270-4c76-847f-b56fe601d169"
#define CHAR_UUID "24a3cded-5ed9-469e-ab63-a69d97aacccc"

BLECharacteristic* pCharacteristic ;
BLEServer* pServer;

class Mycallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) override{
    std::string value = pCharacteristic->getValue();
    std::string direction,speed,duration_ms;
    if(!value.empty()){
      Serial.print("Received via Ble \n");
      Serial.println(value.c_str());
      if(value == "forward"){
        Serial.print("Forward receveid");
      }
      std::stringstream ss(value.c_str());
      std::getline(ss,direction,',');
      std::getline(ss,speed,',');
      std::getline(ss,duration_ms,',');

      if(speed == "slow") speed = "190";
      else if(speed == "medium") speed = "222";
      else if(speed == "fast") speed = "255";
      else speed = "0";

       speedX((unsigned long) stoi(speed));
      

      if(direction == "forward") forward();
      else if (direction == "reverse")reverse();
      else if (direction == "left")left();
      else if (direction == "right")right();
      else stop();

     delay(stoi(duration_ms));
     stop();
    }

  }
};

//Callbacks handeling connection and disconnection
class MyServerCallback: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer) override{
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer* pServer) override{
    Serial.println("Client disconnected, restarting adverstising...");
    pServer->startAdvertising();
  }
};

//set speed for both mottors
void speedX(uint8_t speed){
  ledcWrite(PWM_CHANNELA,speed);
  ledcWrite(PWM_CHANNELB,speed);

}

void reverse(){
  Serial.print("reverse");
  digitalWrite(IN1,1);
  digitalWrite(IN2,0);
  digitalWrite(IN3,1);
  digitalWrite(IN4,0);
}
void forward(){
  Serial.println("forward");
  digitalWrite(IN1,0);
  digitalWrite(IN2,1);
  digitalWrite(IN3,0);
  digitalWrite(IN4,1);
}
void stop(){
  Serial.println("stop");
  digitalWrite(IN1,0);
  digitalWrite(IN2,0);
  digitalWrite(IN3,0);
  digitalWrite(IN4,0);
}
void right(){
  Serial.print("right");
  digitalWrite(IN1,0);
  digitalWrite(IN2,1);
  digitalWrite(IN3,0);
  digitalWrite(IN4,0);
}
void left(){
  Serial.print("left");
  digitalWrite(IN1,0);
  digitalWrite(IN2,0);
  digitalWrite(IN3,0);
  digitalWrite(IN4,1);
}

void setup() {
  Serial.begin(9600);
 
  //Initializing server
  BLEDevice::init("esp32-car");

  //Create server
  pServer = BLEDevice::createServer();

  //Create service 
  BLEService* pService = pServer->createService(SERVICE_UUID);
  
  // Create characteristic
  pCharacteristic = pService->createCharacteristic(
                                        CHAR_UUID,BLECharacteristic::PROPERTY_READ |
                                                  BLECharacteristic::PROPERTY_WRITE);
  // set characteristic value
  pCharacteristic->setCallbacks(new Mycallback());
  pCharacteristic->setValue("Liberty");

  //Start service
  pService->start();

  //Adverstising
  pServer->getAdvertising()->start();
  Serial.println("BLE Server is running and advertising...");

  //pin outputs

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  //ledc PWM for mottor A
  ledcSetup(PWM_CHANNELA,PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, PWM_CHANNELA);

  //ledc PWM for mottor B
  ledcSetup(PWM_CHANNELB,PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENB, PWM_CHANNELB);
  stop();
  speedX(0);

}

void loop() {
}
