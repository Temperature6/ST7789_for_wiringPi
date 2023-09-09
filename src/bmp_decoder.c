#include "bmp_decoder.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

char infoBuf[BMP_INFO_LENGTH];
#define CLEAR_BEFORE_WRITE

void PrintFileInfo(const char* filePath)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    FILE *fp;

    fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        printf("Fail to Open File: %s\n", filePath);
        return;
    }

    fread(infoBuf, 1, BMP_INFO_LENGTH, fp);
    fclose(fp);

    memcpy(&fileHeader, infoBuf, sizeof(BITMAPFILEHEADER));
    memcpy(&infoHeader, infoBuf + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
    
	printf("-------------FileInfo-------------\n");
	printf("FileType:%c%c\n", (fileHeader.bfType >> 8) & 0xff, fileHeader.bfType & 0xFF);
	printf("FileSzie:%u (Bytes)\n", fileHeader.bfSize);
	printf("bfOffBits:%u\n", fileHeader.bfOffBits);
	printf("-------------BMPInfo-------------\n");
	printf("BITMAPINFOHEADER Size:%u\n", infoHeader.biSize);
	printf("Width:%d\n", infoHeader.biWidth);
	printf("Height:%d\n", infoHeader.biHeight);
	printf("Planes:%d\n", infoHeader.biPlanes);
	printf("BitCount:%d\n", infoHeader.biBitCount);
	switch (infoHeader.biCompression)
	{
	case BI_RGB:
		printf("Compression:BI_RGB\n");
		break;
	case BI_RLE8:
		printf("Compression:BI_RLE8\n");
		break;
	case BI_RLE4:
		printf("Compression:BI_RLE4\n");
		break;
	case BI_BITFIELDS:
		printf("Compression:BI_BITFIELDS\n");
		break;
	}
	printf("SizeImage:%u\n", infoHeader.biSizeImage);
	printf("XPelsPerMeter:%d\n", infoHeader.biXPelsPerMeter);
	printf("YPelsPerMeter:%d\n", infoHeader.biYPelsPerMeter);
	printf("Color Used:%u\n", infoHeader.biClrUsed);
	printf("Color Important:%u\n", infoHeader.biClrImportant);
}

void ReadBMPtoBuf(const char* filePath, BYTE* buffer, DWORD bufLen, DecodeRet* ret)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
    FILE *fp;

    memset(buffer, 0, bufLen);

    fp = fopen(filePath, "r");
	if (fp == NULL)
	{
		if (ret != NULL) *ret = OpenFileFailed;
        fclose(fp);
		return;
	}
	else
	{

        fread(infoBuf, 1, BMP_INFO_LENGTH, fp);
        fclose(fp);
		memcpy(&fileHeader, infoBuf, sizeof(BITMAPFILEHEADER));
		memcpy(&infoHeader, infoBuf + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
		if (infoHeader.biCompression != BI_RGB || infoHeader.biClrUsed != 0)
		{
			if (ret != NULL) *ret = UnspportedBMPType;
			return;
		}

        if (infoHeader.biSizeImage > bufLen)
        {
            if (ret != NULL) *ret = BufferSizeTooSmall;
            return;
        }

        fp = fopen(filePath, "r");
        fseek(fp, fileHeader.bfOffBits, SEEK_SET);
        fread(buffer, 1, infoHeader.biSizeImage, fp);
        fclose(fp);

        if (infoHeader.biBitCount == 16)    //16
        {
            uint8_t temp1 = 0, temp2 = 0;
            for (DWORD i = 0; i < infoHeader.biSizeImage; i += 2)
            {
                temp1 = ((buffer[i + 1] >> 2) << 3) | ((((buffer[i + 1] & 0b11) << 3) | ((buffer[i] >> 5) & 0b111)) >> 2);
                temp2 = (((buffer[i] >> 5) & 0b111) << 6) | (buffer[i] & 0b11111);
                buffer[i] = temp1;
                buffer[i + 1] = temp2;
            }
            if (ret != NULL) *ret = Success;
        }
        else if (infoHeader.biBitCount == 24)
        {

        }
        else
        {
            if (ret != NULL) *ret = UnspportedBMPType;
            return;
        }
	}
}

void ShowBMPImage(const char *filePath, uint16_t x, uint16_t y, DecodeRet *ret)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    FILE* fp = 0;

    fp = fopen(filePath, "r");
    if (fp == NULL)
    {
        if (ret != NULL) *ret = OpenFileFailed;
        return;
    }
    else
    {
        fread(infoBuf, 1, BMP_INFO_LENGTH, fp);
        fclose(fp);
        memcpy(&fileHeader, infoBuf, sizeof(BITMAPFILEHEADER));
        memcpy(&infoHeader, infoBuf + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));

        //不支持的文件类型
        if (infoHeader.biCompression != BI_RGB || infoHeader.biClrUsed != 0)
        {
            if (ret != NULL) *ret = UnspportedBMPType;
            return;
        }
        //图像不适合目标屏幕设备
        if (infoHeader.biWidth > SCREEN_WIDTH || infoHeader.biHeight > SCREEN_HEIGHT)
        {
            if (ret != NULL) *ret = ScreenNotFit;
        }
        


        if (infoHeader.biBitCount == 16)
        {
            fopen(filePath, "r");
            uint8_t temp1 = 0, temp2 = 0;
            BYTE* linePixels = (BYTE*)malloc(sizeof(BYTE) * infoHeader.biWidth * 2);
            if (linePixels == NULL)
            {
                if (ret != NULL) *ret = AllocMemFailed;
                return;
            }

            if (infoHeader.biWidth % 4 != 0)
            {
                infoHeader.biWidth += 4 - infoHeader.biWidth % 4;
            }

            for (DWORD i = 0; i < infoHeader.biHeight; i++)
            {
#ifdef CLEAR_BEFORE_WRITE
                memset(linePixels, 0, sizeof(BYTE) * infoHeader.biWidth * 2);
#endif //CLEAR_BEFORE_WRITE
                fseek(fp, fileHeader.bfOffBits + i * infoHeader.biWidth * 2, SEEK_SET);
                fread(linePixels, 1, infoHeader.biWidth * 2, fp);

                for (DWORD j = 0; j < infoHeader.biWidth * 2; j += 2)
                {
                    temp1 = ((linePixels[j + 1] >> 2) << 3) | ((((linePixels[j + 1] & 0b11) << 3) | ((linePixels[j] >> 5) & 0b111)) >> 2);
                    temp2 = (((linePixels[j] >> 5) & 0b111) << 6) | (linePixels[j] & 0b11111);
                    linePixels[j] = temp1;
                    linePixels[j + 1] = temp2;
                }
                LCD_DrawPic(x, y + infoHeader.biHeight - i - 1, infoHeader.biWidth, 1, (const uint16_t*)linePixels);
            }
            free(linePixels);
            fclose(fp);
        }
        else if (infoHeader.biBitCount == 24)
        {
            BYTE* rawLinePix = (BYTE*)malloc(sizeof(BYTE) * infoHeader.biWidth * 3);
            BYTE* linePixels = (BYTE*)malloc(sizeof(BYTE) * infoHeader.biWidth * 2);

            if (rawLinePix == NULL || linePixels == NULL)
            {
                if (ret != NULL) *ret = AllocMemFailed;
                fclose(fp);
                return;
            }
            
            if (infoHeader.biWidth % 4 != 0)
            {
                infoHeader.biWidth += 4 - infoHeader.biWidth % 4;
            }

            fp = fopen(filePath, "r");

            for (DWORD i = 0; i < infoHeader.biHeight; i++)
            {
#ifdef CLEAR_BEFORE_WRITE
                memset(rawLinePix, 0, sizeof(BYTE) * infoHeader.biWidth * 3);
                memset(linePixels, 0, sizeof(BYTE) * infoHeader.biWidth * 2);
#endif //CLEAR_BEFORE_WRITE
                fseek(fp, fileHeader.bfOffBits + infoHeader.biWidth * i * 3, SEEK_SET);
                fread(rawLinePix, 1, infoHeader.biWidth * 3, fp);

                for (DWORD j = 0; j < infoHeader.biWidth; j++)
                {
                    linePixels[j * 2] = ((rawLinePix[j * 3 + 2] >> 3) << 3) | (rawLinePix[j * 3 + 1] >> 5);
                    linePixels[j * 2 + 1] = (((rawLinePix[j * 3 + 1] >> 2) & 0b111) << 5) | (rawLinePix[j * 3] >> 3);
                }
                LCD_DrawPic(x, y + infoHeader.biHeight - i - 1, infoHeader.biWidth, 1, (const uint16_t*)linePixels);
            }

            free(linePixels);
            free(rawLinePix);
            fclose(fp);

        }
        else
        {
            fclose(fp);
            if (ret != NULL) *ret = UnspportedBMPType;
            return;
        }


    }
}

void LCD_DrawPic(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
    //ST7735_DrawImage(x, y, w, h, (uint8_t*)data);
    ST7789_DrawImage(x, y, w, h, data);
}
