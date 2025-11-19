// ImageHandler.h
#pragma once

#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <stdint.h>
/*
typedef enum{
    JPG = 0,
    PNG,
    BMP
}IMAGEFORMAT;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Pixel;
*/
unsigned char* getImage(char* filename, int* Width, int* Height, int* Channels);
void createImage(const char* outputFileName, char* imageData, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS);
unsigned char* normalizeImage(unsigned char* data, const uint16_t WIDTH, const uint16_t HEIGHT, int* channels, const char* inputPath, const char* outputPath);

//Pixel** UnrollRGB(unsigned char *data, const uint16_t WIDTH, const uint16_t HEIGHT);
//unsigned char* RollRGB(Pixel** data, const uint16_t WIDTH, const uint16_t HEIGHT);

//void iterateOverPixel(const Pixel** colors, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS);
void iterateOverColors(const unsigned char* colors, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS);

//void freePixels(Pixel** pixels, uint16_t Width);
void freeImage(unsigned char* img);

unsigned char* RGBA_To_RGB(unsigned char* RGBAdata, const uint16_t WIDTH, const uint16_t HEIGHT);
unsigned char* RGB_To_RGBA(unsigned char* RGBAdata, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t AlphaValue);

#endif