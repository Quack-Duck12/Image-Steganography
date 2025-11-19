#pragma once

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/imageHandler.h"
#include "../include/cryptography.h"
#include "../include/utils.h"

#define MAXFILESIZE 268435455 //Max File Size in Bits (33.5mb)
#define DEBUG 0

void encrypt();
void decrypt();

#endif 