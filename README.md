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

引脚连接
|功能|ESP32 GPIO|
|--|--|
|DS18B20|4|
| TDS | 32 |
| 浊度 | 33 |
| 加热器 | 16 |
| 灯带 DI | 5 |
| 水泵泵继电器 | 25 |

---

## 📦 软件依赖
```ini
; platformio.ini
[env:denky32]
platform = espressif32
board = denky32
framework = arduino
lib_deps =
    OneWire
    DallasTemperature
    FastLED
    PubSubClient
    GravityTDS
```

---

## 如何适配
1. **克隆**
   ```bash
   git clone https://github.com/YOUR_NAME/AIoT_FishTank.git
   cd AIoT_FishTank
   ```
2. **修改机密**  
   复制 `src/config_template.h` → `src/config.h` 并填写：
   - Wi-Fi 名称/密码
   - 华为云 IoTDA 三元组
3. **编译 & 烧录**
   ```bash
   pio run -t upload
   ```
4. **上电** → 串口 显示「Connecting...」→ 云端上线成功

---

## 📊 华为云物联网平台配置
1. 创建产品 → 添加功能（温度、TDS、浊度、泵状态...）
2. 创建设备 → 记下 **DeviceId / Secret**
3. 在线调试 → 实时看曲线 / 下发指令

---

## 🌈 灯光玩法
```cpp
// 流水灯速度 200 ms/格，颜色 HSV 随水温自动变化
setFlowSpeed(200);
setHue(map(temp, 20, 30, 120, 0)); // 20℃ 绿色 → 30℃ 红色
```

---

### 🗂️ 项目结构
```
AIoT_FishTank
├─ src/
│  ├─ main.cpp
│  ├─ sensors.cpp          // 传感器封装
│  ├─ actors.cpp           // 执行器封装
│  ├─ cloud.cpp            // 华为云 MQTT
│  └─ config.h             // 机密配置（gitignore）
├─ docs/
│  ├─ cover.jpg
│  └─ wiring.png
├─ README.md
└─ platformio.ini
```

---

### 🤝 二次开发
- **添加新传感器**
- **接入 HomeAssistant**：把 MQTT topic 改成 `homeassistant/...` 即可自动发现
- **换云平台**：改 `cloud.cpp` 里 3 行 MQTT 参数即可