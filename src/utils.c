#include "../include/utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to get all the content of a file in a string format
unsigned char* loadFile(const char* filePath, size_t* outputSIZE){
    FILE* fp = fopen(filePath, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    unsigned char* content = ( unsigned char*)malloc(sizeof(char) * (size + 1));
    if (!content){ fclose(fp); return NULL; }

    size_t read = fread(content, sizeof(char), size, fp);
    fclose(fp);

    if (read != size){ free(content); return NULL; }

    content[size] = '\0';
    
    if(outputSIZE) *outputSIZE = (size_t)size;
    return content;
}

// Function to create a file with the content in a string format
void createFile(const char* filePath, char* fileContent, const size_t outputSIZE){
    FILE* pfile = fopen(filePath, "wb");
    if(!pfile){ printf("Error: Coudn't Save content to file!\n"); exit(EXIT_FAILURE);}

    size_t written = fwrite(fileContent, 1, outputSIZE, pfile);
    if (written != outputSIZE){ printf("Error: Coudn't Save content to file!\n"); exit(EXIT_FAILURE);}

    fclose(pfile);
}

// Function to get a File's name in 3 parts, Directory - Name - Format
char** getFilename(const char* filepath){
    char* formatSeparator = strrchr(filepath, '.');
    char* lastSlash = (strrchr(filepath, '/') > strrchr(filepath, '\\')) ? strrchr(filepath, '/') : strrchr(filepath, '\\');

    if (!formatSeparator){
        printf("Invalid File Name!\n");
        exit(6);
    }

    size_t lastSlashPosition = lastSlash ? (lastSlash - filepath) : -1;
    size_t nameLen = formatSeparator - filepath - lastSlashPosition - 1;
    size_t dirLen = lastSlashPosition + 1;
    size_t extLen = strlen(formatSeparator + 1);

    char** Path = (char**)malloc(sizeof(char*) * 3);

    Path[0] = (char*)malloc(dirLen + 1);
    strncpy(Path[0], filepath, dirLen);
    Path[0][dirLen] = '\0';

    Path[1] = (char*)malloc(nameLen + 1);
    strncpy(Path[1], filepath + dirLen, nameLen);
    Path[1][nameLen] = '\0';

    Path[2] = (char*)malloc(extLen + 1);
    strcpy(Path[2], formatSeparator + 1);

    return Path;
}

//Function to get a file's format in string 
char* getFileFormat(const char* filepath){

    const char* dot = strrchr(filepath, '.');
    if (!dot || dot[1] == '\0') return strdup("");

    dot++;

    char* format = strdup(dot);
    if (!format) return NULL;

    for (char* p = format; *p; p++){
        *p = tolower((unsigned char)*p);
    }

    return format;
}

// funtion to get a file's size in Bytes
size_t getFileSize(const char* filePath){
    FILE* fp = fopen(filePath, "rb");
    if (!fp){
        printf("Error: Can't read File!!\n");
        exit(6);
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    
    fclose(fp);
    return(size);
}

//Funtion to assign the header it's values in the specifed format
HEADER assignHeader(const char* format, uint32_t sizeBits, unsigned short BitsPerChannel, short isText){
    HEADER H;
    strcpy(H.FORMAT, format);

    H.SIZE = 0;
    BitsPerChannel--;
    bitPlace(&H.SIZE, 0, 28, sizeBits);       // Bits 0–27 → size
    bitPlace(&H.SIZE, 28, 1, isText);         // Bit 28 → isText
    bitPlace(&H.SIZE, 29, 3, BitsPerChannel); // Bits 29–31 → BitsPerChannel

    return H;
}

//Funtion to slot specific values into certain bit positions
void bitPlace(uint32_t* _buffer, uint8_t offset, uint8_t bits, uint32_t value){
    uint32_t mask = (1 << bits) - 1;;
    *_buffer = (*_buffer & ~(mask << offset)) | ((value & mask) << offset);
}

//Funtion to read specific bit from a certain position in a value
uint32_t bitGet(uint32_t buffer, uint8_t offset, uint8_t bits){
    uint32_t mask = (1 << bits) - 1;
    return (buffer >> offset) & mask;
}

//Funtion to LSB encode a unsigned char* array
unsigned char* packBits(const unsigned char* content, size_t contentSizeBytes,
                           unsigned char* imgData, size_t imgDataSize,
                           unsigned short BPC, size_t* imgDataIndex){

    if (!imgData || !content || BPC == 0) return NULL;

    size_t imgByteIndex = imgDataIndex ? *imgDataIndex / 8 : 0;

    for (size_t i = 0; i < contentSizeBytes; i++){
        uint8_t byte = content[i];
        size_t bitsRemaining = 8;

        while (bitsRemaining > 0){
            if (imgByteIndex >= imgDataSize) return NULL; // overflow

            uint8_t bitsNow = bitsRemaining < BPC ? bitsRemaining : BPC;
            uint8_t mask = (1u << bitsNow) - 1u;

            uint8_t chunk = (byte >> (bitsRemaining - bitsNow)) & mask;

            imgData[imgByteIndex] &= ~mask;
            imgData[imgByteIndex] |= chunk;

            bitsRemaining -= bitsNow;
            imgByteIndex++;
        }
    }

    if (imgDataIndex) *imgDataIndex = imgByteIndex * 8;
    return imgData;
}

//Funtion to LSB decode a unsigned char* array
unsigned char* unpackBits(const unsigned char* imgData, size_t imgDataSize,
                             size_t contentSizeBytes, unsigned short BPC,
                             size_t* imgDataIndex){
    if (!imgData || BPC == 0) return NULL;

    size_t imgByteIndex = imgDataIndex ? *imgDataIndex / 8 : 0;

    unsigned char* content = calloc(contentSizeBytes, sizeof(unsigned char));
    if (!content) return NULL;

    for (size_t i = 0; i < contentSizeBytes; i++){
        uint8_t byte = 0;
        size_t bitsCollected = 0;

        while (bitsCollected < 8){
            if (imgByteIndex >= imgDataSize) break;

            uint8_t bitsNow = (8 - bitsCollected) < BPC ? (8 - bitsCollected) : BPC;
            uint8_t mask = (1u << bitsNow) - 1u;

            uint8_t val = imgData[imgByteIndex] & mask;
            byte = (byte << bitsNow) | val;

            bitsCollected += bitsNow;
            imgByteIndex++;
        }
        content[i] = byte;
    }

    if (imgDataIndex) *imgDataIndex = imgByteIndex * 8;
    return content;
}

void ImagePack(unsigned char* imgData, size_t imgDataSize, HEADER head,
               const unsigned char* content, size_t fileSizeBytes,
               const unsigned char* fileHash, unsigned short BitsPerChannel){

    size_t bitIndex = 0;

    // Pack HEADER at 1 BPC
    packBits((unsigned char*)&head, sizeof(HEADER), imgData, imgDataSize, 1, &bitIndex);

    packBits(content, fileSizeBytes, imgData, imgDataSize, BitsPerChannel, &bitIndex);

    packBits(fileHash, 32, imgData, imgDataSize, BitsPerChannel, &bitIndex);
}

void ImageUnpack(const unsigned char* imgData, size_t imgDataSize, HEADER* outHeader,
                 unsigned char** outContent, size_t* outFileSizeBit, unsigned char* outHash){

    if (!imgData || !outHeader || !outContent || !outFileSizeBit || !outHash){ printf("ImageUnpack: invalid arguments\n"); exit(EXIT_FAILURE); }

    size_t bitIndex = 0;

    unsigned char* headerBuf = unpackBits(imgData, imgDataSize, sizeof(HEADER), 1, &bitIndex);
    if (!headerBuf){ printf("ERROR: failed to unpack HEADER\n"); exit(EXIT_FAILURE); }

    memcpy(outHeader, headerBuf, sizeof(HEADER));
    free(headerBuf);

    uint32_t SIZE = outHeader->SIZE;
    *outFileSizeBit = (size_t)bitGet(SIZE, 0, 28);          // bits 0–27
    unsigned short BitsPerChannel = (unsigned short)bitGet(SIZE, 29, 3) + 1; // bits 29–31
    size_t outFileSizeBytes = (*outFileSizeBit + 7) / 8;    // Rounding up

    *outContent = unpackBits(imgData, imgDataSize, outFileSizeBytes, BitsPerChannel, &bitIndex);
    if (!*outContent){
        printf("ERROR: failed to unpack payload\n");
        exit(EXIT_FAILURE);
    }

    unsigned char* tmpHash = unpackBits(imgData, imgDataSize, 32, BitsPerChannel, &bitIndex);
    if (!tmpHash){ printf("ERROR: failed to unpack hash\n"); exit(EXIT_FAILURE); }
    
    memcpy(outHash, tmpHash, 32);
    free(tmpHash);

    printf("FORMAT: %.5s\n", outHeader->FORMAT);
}
