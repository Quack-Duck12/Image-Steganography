// encryption.h
#pragma once

#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include <stdint.h>

unsigned char* sha256_bin(const unsigned char* data, size_t len);

void sha256Bin(const char* input, unsigned char out_hash[32]);
unsigned char* sha256FileContent(const unsigned char* content, size_t size);

unsigned char* encryptFileContent(const unsigned char* fileContent, size_t fileSizeBytes, const unsigned char* hash);
unsigned char* decryptFileContent(const unsigned char* encryptedContent, size_t fileSizeBytes, const unsigned char* hash);

#endif