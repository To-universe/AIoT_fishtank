#include <Arduino.h>
#include <ArduinoJson.h>
#include "Ticker.h"
#include <stdio.h>
#include <string.h>
#include "HardwareSerial.h"
#include <WiFi.h>
#include <PubSubClient.h>

// 全局对象定义
extern WiFiClient espClient;
extern PubSubClient client;
extern HardwareSerial mySerial;  // 串口1

//WiFi连接桉树
extern const char* wifi_ssid;          //WiFi名称
extern const char* wifi_password;      //WiFi密码

// 华为云设备信息（三元组）
// 三元组生成链接：https://iot-tool.obs-website.cn-north-4.myhuaweicloud.com/
#define DEVICE_ID        ""
#define DEVICE_SECRET    ""
extern const char* CLIENT_ID;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;
extern const char* MQTT_SERVER;  // 华为云MQTT服务器地址
extern const int   MQTT_PORT;

// MQTT主题和消息格式定义
#define SERVICE_ID        "\"Arduino\""
#define MQTT_BODY_FORMAT  "{\"services\":[{\"service_id\":" SERVICE_ID ",\"properties\":{%s"
#define MQTT_TOPIC_REPORT      "$oc/devices/" DEVICE_ID "/sys/properties/report"           // 属性上报主题
#define MQTT_TOPIC_GET         "$oc/devices/" DEVICE_ID "/sys/messages/down"               // 下行消息主题
#define MQTT_TOPIC_COMMANDS    "$oc/devices/" DEVICE_ID "/sys/commands/"                   // 命令下发主题
#define MQTT_TOPIC_CMD_RESPONSE "$oc/devices/" DEVICE_ID "/sys/commands/response/request_id="  // 命令响应主题
#define RESPONSE_DATA     "{\"result_code\": 0,\"response_name\": \"COMMAND_RESPONSE\",\"paras\": {\"result\": \"success\"}}"

//全局变量
extern float temp;                 //温度传感器数据
extern int waterlevel;             //水位数据
extern int gravity_adc;        //浊度数据
extern float ph;                   //ph数据
extern int heat_state;             //加热棒状态
extern int pump_state;             //水泵状态
extern int light_mode;             //灯带模式
extern float temp_stand;            //设定温度

#define LIGHT_CLOSE 0
#define LIGHT_EMERG 1
#define LIGHT_MODE1 2
#define LIGHT_MODE2 3
#define LIGHT_MODE3 4

//函数声明
void setupWiFi();
void setupMQTT();
void reconnectMQTT();
void publishSensorData();
void callback(char* topic, byte* payload, unsigned int length);
void sendCommandResponse(char* topic);