STM32 OV7670(有FIFO)驱动

# OV7670
---
## 调试进度
* 2016-04-23 20:50:05
  SCCB时序调试完成

## 原子例程解读
### 原子例程连线
|  D0~D7  | SCL | SDA | WRST | RCLK | RRST |  OE  | WEN  | VSYNC |
|---------|-----|-----|------|------|------|------|------|-------|
| PB0~PB7 | PC4 | PC5 | PA0  | PA1  | PA4  | PA11 | PA12 | PA15  |

1.PB0~PB7是数据线
2.PC4是SCCB时钟线
3.PC5是SCCB数据线
4.在exit.c中，PA0被配置为上升沿触发，PC5被配置为下降沿触发，PA15被配置为下降沿触发

  * 流程图
![流程图](http://7xtbjh.com1.z0.glb.clouddn.com/16-4-24/58066469.jpg)
## OV7670时序解读
![ov7670行输出时序](http://7xtbjh.com1.z0.glb.clouddn.com/16-4-24/91707402.jpg)

  - HREF上升沿开始传输数据，HREF为高的时候，传输1B的数据，数据在PCLK的上升沿有效，每传输完1B的数据，HREF产生一个下降沿。
![ov7670帧输出时序](http://7xtbjh.com1.z0.glb.clouddn.com/16-4-24/73223270.jpg)

  - 从图中可以看出，传输一帧数据的时候，VSYNC始终为低电平，VSYNC产生一个下降沿代表帧传输开始，VSYNC产生一个上升沿代表一帧图像传输结束；
    具体传输多少数据需要根据设置的图像分辨率来确定。
  - （猜测）可以用三个引脚分别捕获PCLK，HREF，VSYNC产生的电平跳变，每传输1B的数据，PCLK引脚产生8次中断，HREF产生1次中断，每传输一帧图像，
     VSYNC产生一次中断

## \lib\ov7670\ov7670.c分析
1. ov7670_rcc_config()初始化ov7670的端口时钟，其中包括：SCL,SDA,VSYNC,XCLK的端口时钟，即RCC_APB2Periph_AFIO，RCC_APB2Periph_GPIOA
2. ov7670_port_config()初始化端口，SCL(PA4),SDA(PA5),VSYNC(PA6),XCLK(PA8)
3. ov7670_init()初始化ov7670
  * ov7670_rcc_config();
  * ov7670_port_config();
  * ov7670_window_set(...);
  * ov7670_reg_config()-->return SUCCESS or ERROR;
4. VSYNC端口还需要设置中断服务函数（nvic在sys.c中设置）
 