//encryption.c
#include "../include/cryptography.h"
#include "../include/sha256.h"

#include <stdio.h>
#include <stdlib.h>

// Wrapper Function to generate a SHA256 hash based on the sha256 standalone librabry from LekKit https://github.com/LekKit
unsigned char* sha256_bin(const unsigned char* data, size_t len){
    if (!data) return NULL;

    uint8_t hash[32];
    struct sha256_buff buff;

    sha256_init(&buff);
    sha256_update(&buff, data, len);
    sha256_finalize(&buff);
    sha256_read(&buff, hash);

    unsigned char* out = malloc(32);
    if (!out) return NULL;

    memcpy(out, hash, 32);
    return out;
}

void sha256Bin(const char* input, unsigned char out_hash[32]){
    sha256_easy_hash(input, strlen(input), out_hash);
}

unsigned char* sha256FileContent(const unsigned char* content, size_t sizeBytes){
    if (!content || sizeBytes == 0) return NULL;

    struct sha256_buff buff;
    sha256_init(&buff);

    sha256_update(&buff, content, sizeBytes);
    sha256_finalize(&buff);

    uint8_t hash[32];
    sha256_read(&buff, hash);

    unsigned char* hex_hash = malloc(65);
    if (!hex_hash) return NULL;

    for (int i = 0; i < 32; i++)
        snprintf((char*)hex_hash + i * 2, 3, "%02x", hash[i]);

    hex_hash[64] = '\0';
    return hex_hash;
}

unsigned char* encryptFileContent(const unsigned char* fileContent, size_t fileSizeBytes, const unsigned char* hash){
    if (!fileContent || !hash) return NULL;

    size_t hashLen = 32;

    unsigned char* encrypted = malloc(fileSizeBytes);
    if (!encrypted) return NULL;

    for (size_t i = 0; i < fileSizeBytes; i++) {
        encrypted[i] = fileContent[i] ^ hash[i % hashLen];
    }

    return encrypted;
}

unsigned char* decryptFileContent(const unsigned char* encryptedContent, size_t fileSizeBytes, const unsigned char* hash){
    if (!encryptedContent || !hash) return NULL;

    size_t hashLen = 32;
    if (hashLen == 0) return NULL;

    unsigned char* decrypted = malloc(fileSizeBytes);
    if (!decrypted) return NULL;

    for (size_t i = 0; i < fileSizeBytes; i++) {
        decrypted[i] = encryptedContent[i] ^ hash[i % hashLen];
    }

    return decrypted;
}

