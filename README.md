# 电控组第三次培训作业

**姓名：** 汪君奥　　**学号：** 20263001769

---

## 一、核心思路

整个程序只围绕**一份共享状态 `Sys`** 展开。所有功能模块都不直接互相调用，而是统一读写这一份数据：

```
KeyTask ──写──┐
UartTask ─写──┼──→  Sys（由 SysLock 互斥锁保护）
               │        │
               │        ├──→ LEDTask（读 State/Dir/Step，写 Idx）
               │        └──→ OLEDTask（只读）
               │
LEDTask ──任务通知──→ OLEDTask
```

### 1. 共享状态

| 字段 | 含义 | 写 | 读 |
|---|---|---|---|
| `State` | 运行 / 暂停 | KeyTask、UartTask | LEDTask、OLEDTask |
| `Dir` | 正向 / 反向 | KeyTask、UartTask | LEDTask、OLEDTask |
| `Idx` | 当前 LED 序号 0~2 | LEDTask | OLEDTask |
| `Step` | 切换间隔（ms） | UartTask | LEDTask、OLEDTask |

### 2. 四个任务

| 任务 | 优先级 | 职责 |
|---|---|---|
| `KeyTask` | 5 | 10ms 扫描按键，松手瞬间切换暂停/方向，并串口反馈 |
| `LEDTask` | 4 | 按 `Step` 间隔推进 `Idx`，点灯，通知 OLED |
| `UartTask` | 4 | 从队列取字节，状态机拼包、校验、更新状态、反馈 |
| `OLEDTask` | 3 | 等待通知，读 `Sys` 刷新屏幕 |

### 3. 关键机制

- **互斥锁**：所有对 `Sys` 的读写都在 `xSemaphoreTake/Give` 之间；锁内只做读写，延时、串口发送、画屏一律放锁外。
- **任务通知**：`OLEDTask` 用 `ulTaskNotifyTake` 挂起，`LEDTask` 每走一步用 `xTaskNotifyGive` 唤醒它。
- **中断轻量化**：串口接收中断只做"字节入队列 + 重启接收"两件事，拼包、校验、状态更新全部放到 `UartTask` 里。

## 二、串口反馈格式

串口**接收**使用作业规定的 6 字节数据包（`AA RUN DIR STEP_L STEP_H CHECK`）。下面是我自行定义的**上行反馈格式**：

```
STATE=RUN,DIR=FWD,STEP=300,LED=1\r\n
```

| 字段 | 取值 | 含义 |
|---|---|---|
| `STATE` | `RUN` / `PAUSE` | 运行 / 暂停 |
| `DIR` | `FWD` / `REV` | 正向（Forward）/ 反向（Reverse） |
| `STEP` | 100~2000 | 相邻 LED 切换间隔，单位 ms |
| `LED` | 1~3 | 当前点亮的 LED 编号 |

- **发送时机**：数据包校验通过并应用设置后。
- **发送方式**：`HAL_UART_Transmit_IT()` 中断发送。

## 三、参考资料与驱动来源

| 内容 | 来源 |
|---|---|
| 主控框架 | STM32F103C8T6  + FreeRTOS |
| OLED 驱动 | 江协科技软件 I2C 驱动，`oled.c / oled.h / oled_font.h` |
|  视频         |江协科技I2C视频，铁头山羊FreeRTOS教程                                                                          |

## 四、选做说明

本次**未完成选做内容**
