# 智能物联网AIoT大作业——智能鱼缸

这是一个智能鱼缸，预期目标是能够实时监测鱼缸内的环境变量，在本地进行调控；同时能够将数据上云，并下发各种预定的命令。

目前该项目实现了约90%的功能，基本算是一个成品。

---
## 功能概览
|功能|器件|是否实现|
|:--:|:--|:--:|
|逻辑控制|ESP32|✅|
|水温监测&恒温|DS18B20 + 5V加热棒|✅|
|水质检测|TDS浊度传感器+PH传感器|✅|
|水位检测|水位传感器WaterSensor|✅|
|换水|水泵|✅|
|灯光效果|WS2812B流水灯|✅|
|数据上云|华为云IoTDA + MQTT|✅|
|远程控制|华为云网页|✅|

---
## 硬件清单
- 主控：ESP32-Denky32
- 传感器：DS18B20温度传感器、浊度探头、水位传感器、PH传感器
- 执行器：水泵（继电器控制）、加热棒（继电器控制）、WS2812B灯带

### 引脚连接
实验中使用稳压模块输出5V电压供电，防止
|DSS18B20引脚（驱动模块）|连接方式|
|:--:|:---:|
|VCC|5V|
|GND|GND|
|DAT|GPIO 4|

> 该传感器模块使用的是ONE_WIRE库，连接引脚为ONE_WIRE总线。

|浊度传感器引脚（驱动模块）|连接方式|
|:--:|:--:|
|V|5V|
|G|GND|
|A|32（模拟引脚）|

|PH传感器引脚（驱动模块）|连接方式|
|:--:|:--:|
|V+|5V|
|G|GND|
|Po|33（模拟引脚）|
> 该模块有很多引脚，有许多使用方式，还有温度补偿引脚，但是本实验只需要三个引脚

|水位传感器引脚|连接方式|
|:--:|:--:|
|VCC|5V|
|GND|GND|
|S|34（模拟引脚）|

|WS2812B灯带引脚|连接方式|
|:--:|:--:|
|红线|5V|
|黑线|GND|
|黄线|5|

---

## 📦 软件依赖
```ini
; platformio.ini
[env:denky32]
platform = espressif32
board = denky32
framework = arduino
lib_deps = 
	adafruit/Adafruit NeoPixel@^1.15.2
	paulstoffregen/OneWire@^2.3.8
	milesburton/DallasTemperature@^4.0.5
	knolleary/PubSubClient@^2.7
	adafruit/Adafruit BMP280 Library@^2.6.8
	rweather/Crypto@^0.4.0
	adafruit/Adafruit BusIO@1.16.1
	bblanchon/ArduinoJson@6.21.5
	arduino-libraries/Arduino_ESP32_OTA@0.2.0
	arduino-libraries/Arduino_DebugUtils@^1.5.0
```

---

## 如何适配
1. **克隆**
   
   将仓库克隆到本地
2. **修改机密**  
   
   在`MQTT.cpp`中找到`变量定义（WIFI变量）`，修改成自己的WiFi账号密码，用于esp32连接；
   
   在`MQTT.cpp`中找到`华为云设备信息`，将其下的变量都填充成自己定义的MQTT连接参数；

   在`MQTT.h`中找到`华为云设备信息（三元组）`，将其下的`DEVICE_ID`和`DEVICE_SECRET`填充为自己的设备ID以及参数。
3. **编译 & 烧录**
   
4. **上电** → 串口 显示「Connecting...」→ 云端上线成功

---

## 📊 华为云物联网平台配置
1. 创建产品 → 添加功能（温度、TDS、浊度、泵状态...），添加命令（开关水泵、修改灯状态...）
2. 创建设备 → 记下 **DeviceId / Secret**，填入代码中

---

## 🌈 灯光玩法
```cpp
#include "Adafruit_NeoPixel.h"
// 设定第i个灯珠的颜色
Adafruit_NeoPixel strip = Adafruit_NeoPixel( MAX_LED, PIN, NEO_RGB + NEO_KHZ800 );
strip.setPixelColor(i,strip.Color(G,R,B));
strip.show();
```

---

## 🗂️ 项目结构
```
AIoT_FishTank
├─ src/
│  ├─ main.cpp
│  ├─ MQTT.cpp             // 华为云 MQTT
│  ├─ GravityTDS.cpp       // 浊度传感器官方库（商家提供）
│  └─ light.cpp            // 灯光模式自定义
├─ include/
│  ├─ GravityTDS.h         // 浊度传感器官方库
│  ├─ light.h              // 灯光模式自定义
│  └─ MQTT.h               // 华为云MQTT
├─ README.md
└─ platformio.ini
```

---

## 🤝 二次开发
- **添加新传感器**
- **拓展更多云端**：搭建微信小程序或APP
- **换云平台**：改 `MQTT.cpp` 和 `MQTT.h` 里的MQTT 参数即可