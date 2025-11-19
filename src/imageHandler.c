// ImageHandler.h
#include "../include/imageHandler.h"

#include "../include/utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

unsigned char* getImage(char* filename, int* Width, int* Height, int* Channels){

    char* format = getFileFormat(filename);
    if (!format) {
        printf("No file extension found!\n");
        exit(-2);
    }
    if(strcmp(format, "jpg") && strcmp(format, "jpeg") &&
       strcmp(format, "png") && strcmp(format, "bmp")){
        printf("Invalid input Image Format, Try Again!");
        exit(-2);
    }

    //Loading Image using the stb_image library
    unsigned char *data = stbi_load(filename, Width, Height, Channels, 0);

    // Error handeling
    if (!data){
        printf("Failed to load image: %s\n", stbi_failure_reason());
        exit(-1);
    }

    return data;
}

void createImage(const char* outputFileName, char* imageData, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS){
    char* format = getFileFormat(outputFileName);
    if (!format) {
        printf("No output file extension found!\n");
        return;
    }

    if(!strcmp(format, "jpg") || !strcmp(format, "jpeg")){
        stbi_write_jpg(outputFileName, WIDTH, HEIGHT, CHANNELS, imageData, 100);
    }
    else if(!strcmp(format, "png")){
        stbi_write_png(outputFileName, WIDTH, HEIGHT, CHANNELS, imageData, WIDTH * CHANNELS);
    }
    else if(!strcmp(format, "bmp")){
        stbi_write_bmp(outputFileName, WIDTH, HEIGHT, CHANNELS, imageData);
    }
    else if(!strcmp(format, "tga")){
        stbi_write_tga(outputFileName, WIDTH, HEIGHT, CHANNELS, imageData);
    }

    printf("Image Succesfully Saved!\n");
}

unsigned char* normalizeImage(unsigned char* data, const uint16_t WIDTH, const uint16_t HEIGHT, int* Channels, const char* inputPath, const char* outputPath){

    const char* inFormat = getFileFormat(inputPath);
    const char* outFormat = getFileFormat(outputPath);

    bool inPNG = !strcmp(inFormat, "png");
    bool outPNG = !strcmp(outFormat, "png");

    // To handle Cases When Input -> png (RGBA) but Output -> non-png (RGB)
    if(inPNG && !outPNG && *Channels == 4){
        unsigned char* rgb = RGBA_To_RGB(data, WIDTH, HEIGHT);
        freeImage(data);
        *Channels = 3;
        return rgb;
    }
    // To handle Cases when Input -> non-png (RGB) but Output -> png (RGBA)
    else if(!inPNG && outPNG && *Channels < 4){
        unsigned char* rgba = RGB_To_RGBA(data, WIDTH, HEIGHT, 255);
        freeImage(data);
        *Channels = 4;
        return rgba;
    }
    // Special Case To Handle when Input -> non-png with (RGBA)
    else if(!inPNG && !outPNG && *Channels == 4){
        unsigned char* rgb = RGBA_To_RGB(data, WIDTH, HEIGHT);
        freeImage(data);
        *Channels = 3;
        return rgb;
    }
    // Special Case To Handle pngs with 3 color Channels
    else if(inPNG && outPNG && *Channels == 3){
        unsigned char* rgba = RGB_To_RGBA(data, WIDTH, HEIGHT, 255);
        freeImage(data);
        *Channels = 4;
        return rgba;
    }
    return data;
}
/*
Pixel** UnrollRGB(unsigned char *data, const uint16_t WIDTH, const uint16_t HEIGHT){
    // Allocating memory to store all RGB (8 bytes * 3) data for rows and columns 
    Pixel **pixelData = malloc(WIDTH * sizeof(Pixel*));
    for (uint16_t i = 0; i < WIDTH; i++) {
        pixelData[i] = malloc(HEIGHT * sizeof(Pixel));  
    }

    uint64_t cnt = 0;

    // Converting a 1d RGB array into a 2d Pixel array
    for(uint16_t x = 0; x < WIDTH; x++){
        for(uint16_t y = 0; y < HEIGHT; y++){
            pixelData[x][y].red = data[cnt];
            pixelData[x][y].green = data[cnt + 1];
            pixelData[x][y].blue = data[cnt + 2];
            cnt += 3;
        }
    }

    return pixelData;
}

unsigned char* RollRGB(Pixel** data, const uint16_t WIDTH, const uint16_t HEIGHT){
    unsigned char* rolledRGBdata = (unsigned char*)malloc(WIDTH * HEIGHT * 3 * sizeof(unsigned char));

    uint64_t cnt = 0;

    for(uint16_t x = 0; x < WIDTH; x++){
        for(uint16_t y = 0; y < HEIGHT; y++){
            rolledRGBdata[cnt] = data[x][y].red;
            rolledRGBdata[cnt + 1] = data[x][y].green;
            rolledRGBdata[cnt + 2] = data[x][y].blue;
            cnt += 3;
        }
    }

    return rolledRGBdata;
}
*/
unsigned char* RGBA_To_RGB(unsigned char* RGBAdata, const uint16_t WIDTH, const uint16_t HEIGHT){

    size_t totalPixel = (size_t)WIDTH * (size_t)HEIGHT;
    unsigned char* RGBdata = (unsigned char*)malloc(totalPixel * 3 * sizeof(unsigned char));

    for(size_t i = 0; i < totalPixel; i++){
        RGBdata[i * 3] = RGBAdata[i * 4];
        RGBdata[i * 3 + 1] = RGBAdata[i * 4 + 1]; 
        RGBdata[i * 3 + 2] = RGBAdata[i * 4 + 2];
    }

    return RGBdata;
}

unsigned char* RGB_To_RGBA(unsigned char* RGBdata, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t AlphaValue){

    size_t totalPixel = (size_t)WIDTH * (size_t)HEIGHT;
    unsigned char* RGBAdata = (unsigned char*)malloc(totalPixel * 4 * sizeof(unsigned char));
    if(!RGBAdata) {printf("ERROR: Memory Allocation"); exit(EXIT_FAILURE);}

    for(size_t i = 0; i < totalPixel; i++){
        RGBAdata[i * 4] = RGBdata[i * 3];
        RGBAdata[i * 4 + 1] = RGBdata[i * 3 + 1]; 
        RGBAdata[i * 4 + 2] = RGBdata[i * 3 + 2];
        RGBAdata[i * 4 + 3] = AlphaValue;
    }

    return RGBAdata;
}
/*
void iterateOverPixel(const Pixel** colors, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS){
    //Printing Each and every pixel color for debug purposes (best works for small image sizes)
    for(uint16_t x = 0; x < WIDTH; x++){
        for(uint16_t y = 0; y < HEIGHT; y++){

            Pixel color = colors[x][y];
            printf("[%d, %d](%3d %3d %3d)\t\t", x, y, color.red, color.green, color.blue);
        }
        printf("\n");
    }
}
*/
void iterateOverColors(const unsigned char* colors, const uint16_t WIDTH, const uint16_t HEIGHT, const uint8_t CHANNELS){
    size_t totalPixel = (size_t)(WIDTH * HEIGHT);
    for(size_t i = 0; i < totalPixel; i++){
        printf("( ");
        for(int j = 0; j < CHANNELS; j++){
            printf("%3d ", colors[i * CHANNELS + j]);
        }
        printf(") ");
        if((i + 1) % WIDTH == 0) printf("\n");
    }
    printf("\n");
}
/*
inline void freePixels(Pixel** pixels, uint16_t Width) {
    for (uint16_t x = 0; x < Width; x++) {
        free(pixels[x]);
    }
    free(pixels);
}
*/
inline void freeImage(unsigned char* img){ stbi_image_free(img);}