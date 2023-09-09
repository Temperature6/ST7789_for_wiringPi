#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include "st7789.h"
#include "bmp_decoder.h"

extern const unsigned char gImage_ys_320_172[];

int main()
{
    wiringPiSetup();
    //控制背光的引脚，高电平点亮背光，不需要可以不用
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    ST7789_Init();  //初始化屏幕，初始化的配置请看st7789.h

    //驱动库自带的测试函数
    ST7789_Test();
    sleep(1);

    //测试显示取模后的图片
    ST7789_Fill_Color(BLACK);
    ST7789_DrawImage(0, 0, 320, 172, (uint16_t*)gImage_ys_320_172);
    sleep(2);

    //BMP图像解码测试
    ST7789_Fill_Color(BLACK);
    DecodeRet ret = -1;
    ShowBMPImage("./ys_logo.bmp", 0, 0, &ret);
    printf("%d\n", ret);
}
