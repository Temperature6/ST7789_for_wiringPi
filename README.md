# ST7789 for wiringPi

### 简介

从https://github.com/Floyd-Fish/ST7789-STM32移植到wiringPi上的

附带一个bmp解码并显示的程序（写的很乱，没有参考价值，不过能用，建议老老实实取模，不会有问题）

### 配置

在‘’st7789.h‘’中配置SPI外设以及用到的RST，DC，CS等引脚

```C
/* choose a Hardware SPI port to use. */
#define ST7789_SPI_CHANNAL  1           //配置要用的SPI外设
#define ST7789_SPI_PORT     0           //
#define ST7789_SPI_SPEED    900000000   //通信速率
/* If u need CS control, comment below*/
//#define CFG_NO_CS

/* Pin connection*/
#define ST7789_RST_PIN      16      //PC10
#define ST7789_DC_PIN       8       //PC8

#ifndef CFG_NO_CS
#define ST7789_CS_PIN       7       //PC5
#endif
```

### 编译

```
make
```

编译并运行

```
make run
```

