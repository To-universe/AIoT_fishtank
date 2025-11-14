#include "MQTT.h"
//-----------------变量定义（WIFI变量）-----------------
const char* wifi_ssid = "XJJ's Find X7";           //WiFi名称
const char* wifi_password = "u2cqxxr3";       //WiFi密码
//---------------华为云设备信息------------------
const char* CLIENT_ID     = "6913458c2447a4269a5d91b4_ESP32_0_0_2025111311";
const char* MQTT_USER     = "6913458c2447a4269a5d91b4_ESP32";
const char* MQTT_PASSWORD = "b2a16bb6b3f14b49c09371e45170791c8178adaa393d49869665efb75c1a17ef";
const char* MQTT_SERVER   = "14aac91662.st1.iotda-device.cn-east-3.myhuaweicloud.com";  // 华为云MQTT服务器地址
const int   MQTT_PORT     = 1883;

//-------------全局变量定义（传感器变量）---------------
float temp;                 //温度传感器数据
int waterlevel;             //水位数据
int gravity_adc=0;          //浊度数据
float ph;                   //ph数据
int heat_state;             //加热棒状态，0表示关闭，1表示开启
int pump_state;             //水泵状态，0表示关闭，1表示开启
int pump_state_control;
int light_mode;             //灯带模式

//-----------连接模块对象定义----------------
WiFiClient espClient;
PubSubClient client(espClient);
HardwareSerial mySerial(1);  // 串口1

/**
 * @brief 连接WiFi网络
 */
void setupWiFi(){
    Serial.printf("正在连接WiFi: %s ...\n", wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
}

/**
 * @brief 初始化MQTT客户端
 */
void setupMQTT(){
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  
  // 订阅必要的主题
  client.subscribe(MQTT_TOPIC_COMMANDS);
  client.subscribe(MQTT_TOPIC_GET);
}

/**
 * @brief 重连MQTT服务器
 */
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("尝试连接MQTT服务器...");
    
    if (client.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("MQTT连接成功");
      
      // 重新订阅主题
      client.subscribe(MQTT_TOPIC_COMMANDS);
      client.subscribe(MQTT_TOPIC_GET);
    } else {
      Serial.printf("MQTT连接失败，错误代码: %d，3秒后重试...\n", client.state());
      delay(3000);
    }
  }
}

/**
 * @brief 上报传感器数据到华为云平台
 * @details 先将传感器得到的数据打印到串口用于本地检测，接着组合出将要发送给华为云的MQTT信息（用JSON表示）
 */
void publishSensorData() {
// 假设传感器数据已通过其他函数更新到全局变量
    // 打印传感器数据到串口
    Serial.printf("温度: %.2f *C\n", temp);
    Serial.printf("水位: %d\n", waterlevel);
    Serial.printf("浊度: %d\n", tdsValue);
    Serial.printf("pH值: %.2f\n", ph);
    Serial.printf("加热棒状态: %d\n", heat_state);
    Serial.printf("水泵状态: %d\n", pump_state);
    Serial.printf("灯带模式: %d\n", light_mode);
  
    // 构建JSON消息
    char properties[256];
    char jsonBuf[512];
  
    sprintf(properties, 
            "\"温度\":%.2f,\"水位\":%d,\"浊度\":%d,\"PH\":%.2f,\"加热棒状态\":%d,\"水泵状态\":%d,\"灯带颜色\":%d}}]}",
            temp, waterlevel, tdsValue, ph, heat_state, pump_state, light_mode);
  
    sprintf(jsonBuf, MQTT_BODY_FORMAT, properties);
  
    // 发布消息
    client.publish(MQTT_TOPIC_REPORT, jsonBuf);
    Serial.println("上报数据:");
    Serial.println(jsonBuf);
    Serial.println("MQTT数据上报成功");
}

/**
 * @brief MQTT消息回调函数，处理平台下发的命令
 * @details 解析华为云平台下发的命令，主要包括4个内容：1.水泵开启与关闭。2.加热棒开启与关闭。3.灯带开启、关闭、模式选择。4.预设温度的自定义（用于选择鱼缸保温到哪个温度）
 */
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("\n接收平台下发消息");
    Serial.print("主题: ");
    Serial.println(topic);
  
    // 为payload添加结束符
    payload[length] = '\0';
    Serial.print("内容: ");
    Serial.println((char*)payload);
  
    // 处理命令下发
    if (strstr(topic, MQTT_TOPIC_COMMANDS)) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, payload);
    
        if (error) {
            Serial.println("JSON解析失败");
            return;
        }
    
        // 提取控制参数
        if (doc["paras"].containsKey("HEATER_STATE")) {
            heat_state = doc["paras"]["HEATER_STATE"];
        }
        if (doc["paras"].containsKey("PUMP_STATE")) {
            pump_state_control = doc["paras"]["PUMP_STATE"];
        }
        if (doc["paras"].containsKey("LIGHT_STATE")) {
            light_mode = doc["paras"]["LIGHT_STATE"];
        }
        if(doc["paras"].containsKey("TEMP_SETTING")){
          temp_stand = doc["paras"]["TEMP_SETTING"];
        }
    
        // 打印接收到的参数
        Serial.println("解析的参数:");
        Serial.printf("HeatState: %d, PumpState: %d, LightMode: %d,温度阈值：%f\n", 
                      heat_state, pump_state_control, light_mode,temp_stand);
    
        // （假设硬件控制逻辑由其他函数处理）
        // 这里仅更新全局变量，实际控制需根据硬件实现
            
        // 提取request_id用于响应
        char requestId[100] = {0};
        char* pstr = topic;
        char* p = requestId;
        int flag = 0;
    
        while (*pstr) {
            if (flag) *p++ = *pstr;
            if (*pstr == '=') flag = 1;
            pstr++;
        }
        *p = '\0';
    
        Serial.print("Request ID: ");
        Serial.println(requestId);
    
        // 构建响应主题并发送响应
        char responseTopic[200] = {0};
        strcat(responseTopic, MQTT_TOPIC_CMD_RESPONSE);
        strcat(responseTopic, requestId);
    
        sendCommandResponse(responseTopic);
    }
    // 处理其他下行消息
    else if (strstr(topic, MQTT_TOPIC_GET)) {
        Serial.println("接收到下行消息，未处理");
    }
}

/**
 * @brief 发送命令响应到平台
 */
void sendCommandResponse(char* topic) {
  if (client.publish(topic, RESPONSE_DATA)) {
    Serial.println("命令响应发送成功");
  } else {
    Serial.println("命令响应发送失败");
  }
  Serial.println("------------------------");
}