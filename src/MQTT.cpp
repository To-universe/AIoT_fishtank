#include "MQTT.h"

/**
 * @brief 连接WiFi网络
 */
void setupWiFi(){
    Serial.printf("正在连接WiFi: %s ...\n", ssid);
    WiFi.begin(ssid, password);
  
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
 */
void publishSensorData() {
//   // 读取传感器数据
//   photoResistorValue = analogRead(PHOTORESISTOR_PIN);
//   temperature = bmp280.readTemperature();
  
//   // 打印传感器数据到串口
//   Serial.printf("光敏电阻值: %d\n", photoResistorValue);
//   Serial.printf("温度: %.2f *C\n", temperature);
  
//   // 构建JSON消息
//   char properties[256];
//   char jsonBuf[512];
  
//   sprintf(properties, 
//           "\"Temperature\":%.2f,\"Photores\":%d,\"RED\":%d,\"GREEN\":%d,\"BLUE\":%d,\"Switch\":%d}}]}",
//           temperature, photoResistorValue, redValue, greenValue, blueValue, deviceSwitch);
  
//   sprintf(jsonBuf, MQTT_BODY_FORMAT, properties);
  
//   // 发布消息
//   client.publish(MQTT_TOPIC_REPORT, jsonBuf);
//   Serial.println("上报数据:");
//   Serial.println(jsonBuf);
//   Serial.println("MQTT数据上报成功");
}

/**
 * @brief MQTT消息回调函数，处理平台下发的命令
 */
void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.println("\n接收平台下发消息");
//   Serial.print("主题: ");
//   Serial.println(topic);
  
//   // 为payload添加结束符
//   payload[length] = '\0';
//   Serial.print("内容: ");
//   Serial.println((char*)payload);
  
//   // 处理命令下发
//   if (strstr(topic, MQTT_TOPIC_COMMANDS)) {
//     DynamicJsonDocument doc(256);
//     DeserializationError error = deserializeJson(doc, payload);
    
//     if (error) {
//       Serial.println("JSON解析失败");
//       return;
//     }
    
//     // 提取RGB颜色值和开关状态
//     redValue = doc["paras"]["RED"];
//     greenValue = doc["paras"]["GREEN"];
//     blueValue = doc["paras"]["BLUE"];
//     deviceSwitch = doc["paras"]["Switch"];
    
//     // 打印接收到的参数
//     Serial.println("解析的参数:");
//     Serial.printf("RED: %d, GREEN: %d, BLUE: %d, Switch: %d\n", 
//                   redValue, greenValue, blueValue, deviceSwitch);
    
//     // 控制LED
//     if (deviceSwitch) {
//       analogWrite(RED_LED, 255 - redValue);
//       analogWrite(GREEN_LED, 255 - greenValue);
//       analogWrite(BLUE_LED, 255 - blueValue);
//     } else {
//       // 关闭LED
//       analogWrite(RED_LED, 255);
//       analogWrite(GREEN_LED, 255);
//       analogWrite(BLUE_LED, 255);
//     }
    
//     // 提取request_id用于响应
//     char requestId[100] = {0};
//     char* pstr = topic;
//     char* p = requestId;
//     int flag = 0;
    
//     while (*pstr) {
//       if (flag) *p++ = *pstr;
//       if (*pstr == '=') flag = 1;
//       pstr++;
//     }
//     *p = '\0';
    
//     Serial.print("Request ID: ");
//     Serial.println(requestId);
    
//     // 构建响应主题并发送响应
//     char responseTopic[200] = {0};
//     strcat(responseTopic, MQTT_TOPIC_CMD_RESPONSE);
//     strcat(responseTopic, requestId);
    
//     sendCommandResponse(responseTopic);
//   }
//   // 处理其他下行消息
//   else if (strstr(topic, MQTT_TOPIC_GET)) {
//     Serial.println("接收到下行消息，未处理");
//   }
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