#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

#pragma pack(push, 1)

typedef struct{
    char FORMAT[5];
    uint32_t SIZE;
}HEADER;

#pragma pack(pop)

unsigned char* loadFile(const char* filePath, size_t* outputSIZE);
void createFile(const char* filePath, char* fileContent, const size_t outputSIZE);

char** getFilename(const char* filepath);
char* getFileFormat(const char* filepath);
size_t getFileSize(const char* filePath);

HEADER assignHeader(const char* format, uint32_t sizeBits, unsigned short BitsPerChannel, short isText);

void bitPlace(uint32_t* _buffer, uint8_t offset, uint8_t bits, uint32_t value);
uint32_t bitGet(uint32_t buffer, uint8_t offset, uint8_t bits);

unsigned char* packBits(const unsigned char* content, size_t contentSizeBytes,
                           unsigned char* imgData, size_t imgDataSize,
                           unsigned short BPC, size_t* imgDataIndex);
unsigned char* unpackBits(const unsigned char* imgData, size_t imgDataSize,
                            size_t contentSizeBytes, unsigned short BPC,
                            size_t* imgDataIndex);

void ImagePack(unsigned char* imgData, size_t imgDataSize, HEADER head,
               const unsigned char* content, size_t fileSizeBytes,
               const unsigned char* fileHash, unsigned short BitsPerChannel);
void ImageUnpack(const unsigned char* imgData, size_t imgDataSize, HEADER* outHeader,
                unsigned char** outContent, size_t* outFileSizeBit,
                unsigned char* outHash);

#endif