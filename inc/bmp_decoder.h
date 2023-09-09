#ifndef BMP_DECODER
#define BMP_DECODER
#include <stdint.h>
#include "st7789.h"

#define SCREEN_WIDTH   ST7789_WIDTH 
#define SCREEN_HEIGHT  ST7789_HEIGHT

#define BMP_INFO_LENGTH (sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER))

typedef uint8_t     BYTE;   //1字节
typedef uint16_t    WORD;   //2字节
typedef uint32_t    DWORD;  //4字节
typedef int32_t     LONG;   //4字节

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}BITMAPINFOHEADER;

#pragma pack(2)             //2字节对齐
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
}BITMAPFILEHEADER;
#pragma pack()

/* constants for the biCompression field */
#define BI_RGB       0L
#define BI_RLE8      1L
#define BI_RLE4      2L
#define BI_BITFIELDS 3L


typedef struct tagRGBQUAD
{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
}RGBQUAD;

typedef enum
{
    Success = 0,
    OpenFileFailed,
    AllocMemFailed,
    UnspportedBMPType,
    BufferSizeTooSmall,
    ScreenNotFit,
}DecodeRet;

void PrintFileInfo(const char* filePath);
void ReadBMPtoBuf(const char* filePath, BYTE* buffer, DWORD bufLen, DecodeRet* ret);
void ShowBMPImage(const char *filePath, uint16_t x, uint16_t y, DecodeRet *ret);

static void LCD_DrawPic(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);



#endif // !BMP_DECODER
