#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MQTT.h"

// put global variables here:
//RGB灯带
#define PIN 5                        // Define the pins of the RGB light
#define MAX_LED 10                   //64 RGB light

Adafruit_NeoPixel strip = Adafruit_NeoPixel( MAX_LED, PIN, NEO_RGB + NEO_KHZ800 );
uint8_t i = 0;                            
uint32_t color = strip.Color(75,156,215);   //绿、红、蓝

//PH传感器
#define PHSensorPIN 35    //模拟引脚
#define Offset 0.15
unsigned long int avgValue;
float ph;

//浊度（TDS）传感器
  //已被确定为32引脚，如需改动则进入include\GravityTDS.h
extern GravityTDS gravityTDS;
unsigned long tdsValue;
int gravity_adc = 0;

//Water Sensor水位传感器
#define Water_Sensor_PIN 34
int waterlevel;

//水泵
#define PumpPin 25

//温度传感器ds18b20
#define ONE_WIRE_PIN 4
OneWire onewire(ONE_WIRE_PIN);
DallasTemperature sensors(&onewire);
float temp;

//加热棒
#define HEAT_PIN 16

//温度调节
float temp_stand;

// put function declarations here:
float PHValueRead();
float TDSValueRead();
int waterLevelRead();
float TempRead();

/**
 * Function:  setup
 */
void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();

  pinMode(13,OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready");

  gravityTDS.setup();

  pinMode(PumpPin,OUTPUT);

  sensors.begin();//初始化总线

  pinMode(HEAT_PIN,OUTPUT);

  temp_stand = 25.0;
}

void loop() {
  // put your main code here, to run repeatedly:
  //----------------传感器读取--------------------
  tdsCalibrationProcess();
  gravityTDS.update();
  static unsigned long currentTime = millis();
  if(millis()-currentTime>1000U){
     currentTime = millis();
    //浊度传感器
    gravity_adc = analogRead(32);
    Serial.print("原始ADC值：");
    Serial.println(gravity_adc);
    tdsValue=TDSValueRead();
    Serial.println("-------------------");
    //DS18B20温度传感器
    temp = temperatureRead();
    Serial.print("温度为：");
    Serial.println(temp);
    //PH传感器
    ph = PHValueRead();
    Serial.println("--------------");
    //水位传感器
    waterlevel =waterLevelRead();
    Serial.print("当前水位值：");
    Serial.println(waterlevel);
    Serial.println("---------------------");
  }

  

//-----------------灯带控制---------------------
  for (int i = 0; i < MAX_LED; i++){
    strip.setPixelColor(i,color);
    strip.show();
    delay(500);
  }
  for (int i = 0;i <MAX_LED;i++){
    strip.setPixelColor(i,strip.Color(0,0,0));
    strip.show();
    delay(500);
  }
  
//----------------水温控制---------------
  if(temp > temp_stand+5.0){
    digitalWrite(HEAT_PIN,LOW);
  }else if(temp < temp_stand-5.0){
    digitalWrite(HEAT_PIN,HIGH);
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
  float phValue=(float)avgValue*5.0/1024/6;
  float PH = 14-5.112*(phValue-1.758)*Offset;
  Serial.print("phValue:");
  Serial.print(phValue,4);
  Serial.print("     pH:");
  Serial.print(PH,2);
  Serial.println(" ");
  return PH;
}
/**
 * Function: TDSValueRead
 * @brief 获取浊度传感器的数据
 */
float TDSValueRead(){
  Serial.print(gravityTDS.getValue());
  Serial.println("  ppm");
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
  Serial.println("温度数据请求完成");
  Serial.println(sensors.getTempCByIndex(0));
  return sensors.getTempCByIndex(0);
}