# STM32 嵌入式开发实践：RFID / 超声波测距 / 温湿度检测

基于 **STM32F103C8T6** 开发板的三个嵌入式系统设计实践项目，使用 **STM32CubeMX** 生成工程框架、**Keil MDK-ARM 5** 开发编译，外设驱动全部基于 **STM32 LL 库**（Low Layer，直接操作寄存器，比 HAL 更贴近底层）。

三个项目共用同一套板级外设（OLED 显示、按键、LED），各自挂载不同的传感器模块。

---

## 目录

| 项目 | 传感器 / 模块 | 核心功能 |
|---|---|---|
| [rfid-card-system](#1-rfid-卡系统) | MFRC522（RFID 读卡） | 模拟储值卡，刷卡充值 / 消费并写回卡片 |
| [ultrasonic-rangefinder](#2-超声波测距系统) | HC-SR04（超声波） | 非阻塞测距，OLED 实时显示距离 |
| [dht11-environment-monitor](#3-dht11-温湿度检测系统) | DHT11（温湿度） | 单总线读取温湿度，OLED 实时显示 |

---

## 硬件平台

| 项目 | 型号 |
|---|---|
| MCU | STM32F103C8T6（Cortex-M3，72 MHz，64 KB Flash / 20 KB SRAM） |
| 晶振 | 8 MHz HSE，PLL ×9 → 72 MHz 系统时钟 |
| 显示 | 0.96" OLED（SSD1306，软件 I2C） |
| 开发环境 | Keil MDK-ARM 5 + STM32CubeMX |

### 公共外设引脚（三个工程一致）

| 外设 | 引脚 | 说明 |
|---|---|---|
| OLED SCL | PB6 | 软件 I2C 时钟 |
| OLED SDA | PB7 | 软件 I2C 数据 |
| OLED PWR | PB5 | OLED 电源控制 |
| LED | PB0 ~ PB3 | 状态指示 |
| 按键 SW1 | PC15 | 功能键 |
| 按键 SW2 | PC14 | 加 |
| 按键 SW3 | PC13 | 减 |
| 按键 SW4 | PB4 | 备用 |

---

## 1. RFID 卡系统

用 MFRC522 读写 **Mifare Classic 1K** 卡，实现一个模拟储值卡系统。

**引脚**

| 信号 | 引脚 |
|---|---|
| SCK | PA2 |
| MOSI | PA1 |
| MISO | PA0 |
| NSS（片选） | PA3 |
| RST | PA4 |

**功能**

- 软件模拟 SPI 时序驱动 MFRC522（不占用硬件 SPI 外设）
- 两种工作模式，按 **SW1** 切换：
  - **charge（消费模式）**：刷卡扣除 `d` 元
  - **recharge（充值模式）**：刷卡增加 `p` 元
- **SW2 / SW3** 在当前模式下增减金额（消费 0~10，充值 0~20）
- 余额存放在卡片 **1 扇区第 0 块**（`snr=1`，地址 `snr*4+0`），密钥为 Mifare 出厂默认 KEYA `FF FF FF FF FF FF`
- 读卡 → 改余额 → 写回，成功后点亮 LED 并在 OLED 第二行显示剩余金额
- `WaitCardOff()` 等待卡片离开感应区，避免一次刷卡重复扣费

**RFID 读卡流程**（[main.c](rfid-card-system/Src/main.c)）

```
PcdRequest(REQ_ALL)  →  PcdAnticoll()  →  PcdSelect()  →  PcdAuthState(KEYA)
  寻卡                    防冲撞            选卡             验证扇区密码
                                                              ↓
                          PcdWrite()  ←  修改 buf[0]  ←  PcdRead()
                            写回余额                        读取当前余额
```

**主要文件**

- [Src/mfrc522.C](rfid-card-system/Src/mfrc522.C) — MFRC522 驱动
- [Src/main.c](rfid-card-system/Src/main.c) — 业务逻辑（充值 / 消费）
- [Src/spi.c](rfid-card-system/Src/spi.c) — 软件 SPI
- [Src/oled.c](rfid-card-system/Src/oled.c)、[Src/i2c_oled.c](rfid-card-system/Src/i2c_oled.c) — OLED 显示
- [Src/button.c](rfid-card-system/Src/button.c) — 按键扫描

---

## 2. 超声波测距系统

用 HC-SR04 超声波模块测距，配合 **TIM4** 做微秒级计时。

**引脚**

| 信号 | 引脚 | 说明 |
|---|---|---|
| TRIG | PA0 | 触发脉冲输出 |
| ECHO | PA1 | 回响输入，**EXTI1 双边沿中断** |

**功能**

- 主循环发送 10 µs 高电平触发脉冲
- ECHO 引脚上升沿 / 下降沿触发中断，用 TIM4（预分频 72 → 1 MHz）记录高电平持续时间 `sonic_time`（单位 µs）
- 距离换算：`距离 = sonic_time × 0.017`（cm），即 `时间 × 340 m/s ÷ 2`
- OLED 第二行实时刷新显示距离，保留一位小数

> 注：`Src/ds18b20.c` 为温度补偿预留的 DS18B20 驱动（同样占用 PA0），当前版本 main 流程未启用。

**主要文件**

- [Src/ultra.c](ultrasonic-rangefinder/Src/ultra.c) — 超声波驱动 + EXTI1 中断服务
- [Src/main.c](ultrasonic-rangefinder/Src/main.c) — 主流程 + TIM4 初始化

---

## 3. DHT11 温湿度检测系统

用 DHT11 单总线温湿度传感器采集环境数据。

**引脚**

| 信号 | 引脚 | 说明 |
|---|---|---|
| DATA | PA0 | 单总线（开漏 + 上拉，需外接 4.7 kΩ 上拉电阻） |

**功能**

- 软件延时（`Delay_us`，按 72 MHz 换算）+ 单总线协议手动收发
- 时序：主机拉低 >18 ms 起始信号 → 等待从机 80 µs 低 + 80 µs 高响应 → 逐位读取 40 bit 数据（湿度整数 + 湿度小数 + 温度整数 + 温度小数 + 校验和）
- OLED 第二行显示温度与湿度：`TH&RH:<温度> ,<湿度>`
- 每 200 ms 采集一次，PB0 LED 同步翻转作为心跳指示

**主要文件**

- [Src/dht11.c](dht11-environment-monitor/Src/dht11.c) — DHT11 单总线驱动
- [Src/main.c](dht11-environment-monitor/Src/main.c) — 主流程

---

## 目录结构

三个工程结构一致（STM32CubeMX 标准布局）：

```
<project>/
├── Src/                  # 应用源码（本项目主要工作在这里）
├── Inc/                  # 应用头文件
├── Drivers/
│   ├── STM32F1xx_HAL_Driver/   # ST 官方 HAL 库（工程未直接调用，随 CubeMX 生成）
│   └── CMSIS/                  # Cortex-M3 内核头文件 + STM32F1 器件头文件
└── MDK-ARM/
    ├── template.uvprojx  # Keil 工程文件（双击打开）
    ├── template.uvoptx   # Keil 工程配置
    ├── startup_stm32f103xb.s     # 启动文件（中断向量表）
    ├── DebugConfig/      # 调试器配置
    └── RTE/              # Keil RTE 组件配置
```

编译产物（`.o` `.crf` `.axf` `.map` 等）和 Keil 界面布局文件（`.uvguix.*`）已通过 [.gitignore](.gitignore) 排除，不会提交。

## 编译与烧录

1. 安装 **Keil MDK-ARM 5**，并安装 **STM32F1xx_DFP** 器件支持包
2. 用 Keil 打开对应工程，例如 `rfid-card-system/MDK-ARM/template.uvprojx`
3. 编译（F7），确认 0 Error
4. 用 ST-Link / J-Link 连接开发板，下载（F8）即可运行

> 源码中的中文注释统一为 **UTF-8（带 BOM）** 编码，GitHub 与 Keil uVision 均可正常显示。
> 若 Keil 中仍显示乱码，可在 `Edit → Configuration → Editor → Encoding` 中手动选为 `UTF-8`。

## 说明

`Drivers/` 目录下的 STM32 HAL 驱动与 CMSIS 组件版权归 **STMicroelectronics** 所有，遵循 **BSD 3-Clause** 许可协议；`Src/`、`Inc/` 下的应用代码为本项目实现。
