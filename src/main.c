#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "../include/imageHandler.h"
#include "../include/cryptography.h"
#include "../include/utils.h"

#include "../include/functions.h"

int main(int argc, char* argv[]){

    short choice;

    printf("1. Encrypt Text/File into image\n");
    printf("2. Decrypt Data from Image\n");
    printf("Enter Your Choice: "); scanf(" %1hd", &choice);

    switch(choice){
        case 1:
            encrypt();
            break;
        case 2:
            decrypt();
            break;
        default:
            printf("Invalid Choice, Please Try Again!!\n");
    }
    printf("Press Enter to exit...");
    fflush(stdout);
    getchar();

}
