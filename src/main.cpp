#include <Arduino.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MQTT.h"
#include "light.h"

// put global variables here:

//PH传感器
#define PHSensorPIN 33    //模拟引脚
#define Offset 0.15
unsigned long int avgValue;

//浊度（TDS）传感器
  //已被确定为32引脚，如需改动则进入include\GravityTDS.h
extern GravityTDS gravityTDS;
unsigned long tdsValue;

//Water Sensor水位传感器
#define Water_Sensor_PIN 34

//水泵
#define PumpPin 25

//温度传感器ds18b20
#define ONE_WIRE_PIN 4
OneWire onewire(ONE_WIRE_PIN);
DallasTemperature sensors(&onewire);

//加热棒
#define HEAT_PIN 16

//温度调节
float temp_stand;

//定时
static unsigned long currentTime;
static unsigned long reportTime;

// put function declarations here:
float PHValueRead();            //PH值读取
float TDSValueRead();           //浊度值读取
int waterLevelRead();           //水位数据读取
float TempRead();               //温度数据读取

/**
 * Function:  setup
 */
void setup() {
  // put your setup code here, to run once:
  strip.begin();                    //初始化灯带
  strip.show();

  Serial.begin(115200);             //初始化串口

  gravityTDS.setup();               //初始化浊度传感器

  pinMode(PumpPin,OUTPUT);          //初始化水泵
  digitalWrite(PumpPin,LOW);

  sensors.begin();//初始化总线

  pinMode(HEAT_PIN,OUTPUT);         //初始化加热棒

  temp_stand = 20;                  //设定温度参考值

  currentTime = millis();
  reportTime = millis();

  heat_state = 0;                   //设定加热棒状态
  pump_state = 0;                   //设定水泵状态
  pump_state_control=0;
  light_mode = LIGHT_NORM;          //设定灯带状态

  //-----------MQTT连接---------------------
  // Serial.begin(115200,SERIAL_8N1);
  // mySerial.begin(115200,SERIAL_8N1,19,23);
  analogReadResolution(10);

  setupWiFi();//连接WiFi
  setupMQTT();//初始化MQTT
}

int light_now;
void loop() {
  // put your main code here, to run repeatedly:
  //检查MQTT连接，断开则重连
  if (!client.connected()){
    reconnectMQTT();
  }
  client.loop();
  

  //----------------传感器读取--------------------
  tdsCalibrationProcess();
  gravityTDS.update();
  if(millis()-currentTime>1000){      //每5s读取一次所有传感器的数据
     currentTime = millis();
    //浊度传感器
    tdsValue=TDSValueRead();
    // Serial.print("浊度：");
    // Serial.print(tdsValue);
    // Serial.println("ppm");
    //DS18B20温度传感器
    temp = TempRead();
    // Serial.print("温度：");
    // Serial.println(temp);
    //PH传感器
    ph = PHValueRead();
    // Serial.print("PH值：");
    // Serial.println(ph);
    //水位传感器
    waterlevel =waterLevelRead();
    // Serial.print("水位：");
    // Serial.println(waterlevel);
    // Serial.println(light_now);
    // Serial.println("-----------------");
  }

  //每5s上报传感器数据
  if(millis()-reportTime >5000){
    reportTime = millis();
    publishSensorData();
  }

//-----------------灯带控制---------------------
if(light_mode==LIGHT_CLOSE){
  light_close();
  light_now=0;
}else if(light_mode == LIGHT_NORM){
  if(
    temp > temp_stand+10||tdsValue > 4500 || ph<4 || waterlevel >1000
  )
  {
    light_error();
    light_now = 1;
  }else{
    light_norm();
    light_now =0;
  }
}else if(light_mode== LIGHT_MODE1){
  if(
    temp > temp_stand+10||tdsValue > 4500|| ph<4 || waterlevel >1000
  )
  {  
    light_error();
  }else{
    light_mode1();
  }
}

  
//----------------水温控制---------------
  if(temp > temp_stand+5.0){
    heat_state = 0;
  }else if(temp < temp_stand-5.0){
    heat_state = 1;
  }

  if(heat_state){
    digitalWrite(HEAT_PIN,HIGH);
  }else{
    digitalWrite(HEAT_PIN,LOW);
  }

//------------水泵控制---------------
  if(pump_state_control && waterlevel >0){
    pump_state = 1;
  }else if(tdsValue > 4500 ||ph<4 || waterlevel >1000){
    pump_state = 1;
  }else{
    pump_state = 0;
  }

  if(pump_state){
    digitalWrite(PumpPin,HIGH);
  }else{
    digitalWrite(PumpPin,LOW);
  }
}

// put function definitions here:
/**
 * @brief 获取PH值
 */
float PHValueRead(){
    int buf[10];
  for (int i = 0; i < 10; i++){
    buf[i] = analogRead(PHSensorPIN);
    delay(10);
  }
  for (int i = 0; i < 9; i++)
  {
    for(int j = i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        int temp = buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue = 0;
  for(int i=2;i<8;i++)
    avgValue+=buf[i];
  float phValue=(float)avgValue*5/1024/6;
  float PH = 14-5.112*(phValue-1.758)*Offset;
  return PH;
}
/**
 * Function: TDSValueRead
 * @brief 获取浊度传感器的数据
 */
float TDSValueRead(){
  return gravityTDS.getValue();
}
/**
 * @brief 获取水位传感器的数据
 */
int waterLevelRead(){
  int h = analogRead(Water_Sensor_PIN);
  return h;
}
/**
 * @brief DS28B20温度传感器数据
 */
float TempRead(){
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}