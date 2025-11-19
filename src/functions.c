#include "../include/functions.h"

void decrypt(){
    char inputPath[512] ={0};
    char passkey[256] ={0};

    unsigned char* extractedContent = NULL;
    unsigned char  extractedHash[32] ={0};
    unsigned char  keyHash[32] ={0};
    unsigned char* computedHash = NULL;

    size_t contentSizeBit = 0;
    size_t contentSizeByte = 0;
    int Width = 0, Height = 0, Channels = 0;

    HEADER header;

    printf("Enter Input Image Path: ");
    scanf(" %511[^\n]", inputPath);

    printf("Enter Passkey (leave blank if none): ");
    getchar();
    fgets(passkey, sizeof(passkey), stdin);
    passkey[strcspn(passkey, "\n")] = '\0';

    unsigned char* imageData = getImage(inputPath, &Width, &Height, &Channels);
    size_t imgDataSize = (size_t)(Width * Height * Channels);

    ImageUnpack(imageData, imgDataSize, &header, &extractedContent, &contentSizeBit, extractedHash);
    contentSizeByte = (contentSizeBit / 8);

    if (!extractedContent){
        printf("ERROR: Failed to extract content from image!\n");
        freeImage(imageData);
        exit(2);
    }

    unsigned char* finalContent = extractedContent;

    if (strlen(passkey) > 0){

        sha256Bin(passkey, keyHash);

        unsigned char* decrypted = decryptFileContent(extractedContent, contentSizeBit, keyHash);

        if (!decrypted){
            printf("ERROR: Decryption failed! Wrong passkey or corrupted data.\n");
            free(extractedContent);
            freeImage(imageData);
            exit(3);
        }

        free(extractedContent);
        finalContent = decrypted;
    }

    computedHash = sha256FileContent(finalContent, contentSizeByte);

    if(DEBUG){
        printf("OUT HASH: ");
        for(int i = 0 ; i < 32; i++) printf("%c", extractedHash[i]);
        printf("\n");

        printf("HASH: ");
        for(int i = 0 ; i < 32; i++) printf("%c", computedHash[i]);
        printf("\n");
    }

    if (memcmp(computedHash, extractedHash, 32) != 0){
        printf("ERROR: The HASH Appears to be mismatched! Data possibly corrupted or wrong passkey.\n");
        printf("Proceed further?(y/n): ");

        char further;
        scanf(" %c", &further);

        if(further != 'y' && further != 'Y'){
            free(finalContent);
            freeImage(imageData);
            exit(4);
        }
    }

    char** parts = getFilename(inputPath);

    size_t outLen = strlen(parts[0]) + strlen(parts[1]) + strlen(header.FORMAT) + 3;
    char* finalOutputPath = malloc(outLen);
    sprintf(finalOutputPath, "%s%s.%s", parts[0], parts[1], header.FORMAT);

    printf("Output File: %s\n", finalOutputPath);
    
    createFile(finalOutputPath, (char*)finalContent, contentSizeByte);

    printf("Decryption successful. Saved to %s\n", finalOutputPath);

    free(finalContent);
    free(finalOutputPath);
    freeImage(imageData);

    free(parts[0]);
    free(parts[1]);
    free(parts[2]);
    free(parts);
}

void encrypt(){

    char inputPath[512];
    char outputPath[512];
    char passkey[256];
    char dataFilePath[512];
    char textData[2048];
    unsigned short BitsPerChannel = 0;
    char outFormat[5];

    unsigned char* content = NULL;
    unsigned char* dataFileHash = NULL;
    unsigned char* HashedPasskey = NULL;

    size_t FileSizeBytes = 0;
    size_t FileSizeBits = 0;
    size_t ImgSizeBits   = 0;
    size_t TotalSizeBytes = 0;

    int Width = 0, Height = 0, Channels = 0;

    printf("Enter Input Image Path: ");
    scanf(" %511[^\n]", inputPath);

    printf("Enter Output Image Path/Name (leave blank to auto-generate): ");
    getchar();
    fgets(outputPath, sizeof(outputPath), stdin);
    outputPath[strcspn(outputPath, "\n")] = 0;

    printf("Enter Passkey (optional): ");
    fgets(passkey, sizeof(passkey), stdin);
    passkey[strcspn(passkey, "\n")] = 0;

    printf("Enter File Path to Encrypt (leave blank for text input): ");
    fgets(dataFilePath, sizeof(dataFilePath), stdin);
    dataFilePath[strcspn(dataFilePath, "\n")] = 0;

    printf("Enter Text to Encrypt (leave blank if file is used): ");
    fgets(textData, sizeof(textData), stdin);
    textData[strcspn(textData, "\n")] = 0;

    printf("Enter Bits Per Channel (Lower = more stable)(default=1): ");

    char temp[3];
    fgets(temp, sizeof(temp), stdin);
    temp[strcspn(temp, "\n")] = 0;

    BitsPerChannel = atoi(temp);
    BitsPerChannel = BitsPerChannel == 0 ? 1 :
    (BitsPerChannel > 8 ? 8 : BitsPerChannel);

    char* finalOutputPath = NULL;

    if (strlen(outputPath) == 0){

        char** filePath = getFilename(inputPath);

        size_t outLen = strlen(filePath[0]) + strlen(filePath[1]) + strlen("-1.png") + 1;
        finalOutputPath = malloc(outLen);

        sprintf(finalOutputPath, "%s%s-1.png", filePath[0], filePath[1]);
        printf("Auto Output Path: %s\n", finalOutputPath);

        free(filePath[0]);
        free(filePath[1]);
        free(filePath[2]);
        free(filePath);
    }
    else{
        char* format = getFileFormat(outputPath);
        if (strcmp(format, "png") != 0){
            strcat(outputPath, ".png");
            printf("Invalid Output Format, Formating to png\n");
        }
        free(format);

        finalOutputPath = strdup(outputPath);
        if (!finalOutputPath){ printf("Memory allocation failed!\n"); exit(1); }
    }

    if (strlen(dataFilePath) && strlen(textData)){
        printf("ERROR: Provide either FILE or TEXT, not both!\n");
        exit(2);
    }

    if (strlen(dataFilePath)){

        content = loadFile(dataFilePath, &FileSizeBytes);
        if (!content){ printf("ERROR: Failed to load file!\n"); exit(3); }

        strcpy(outFormat, getFileFormat(dataFilePath));
    }
    else if (strlen(textData)){

        FileSizeBytes = strlen(textData);
        content = malloc(FileSizeBytes + 1);

        memcpy(content, textData, FileSizeBytes + 1);
        strcpy(outFormat, "txt");
    }
    else{
        printf("ERROR: No input data provided!\n");
        exit(4);
    }

    dataFileHash = sha256FileContent(content, FileSizeBytes);
    if (!dataFileHash){ printf("ERROR: SHA256 hashing failed!\n"); exit(5); }

    if (strlen(passkey)){

        HashedPasskey = sha256_bin((unsigned char*)passkey, strlen(passkey));

        size_t encryptedSize = FileSizeBytes;
        unsigned char* newContent = encryptFileContent(content, FileSizeBytes, HashedPasskey);
        if (!newContent){ printf("ERROR: Encryption failed!\n"); exit(6); }

        free(content);           
        content = newContent;
        FileSizeBytes = encryptedSize;
    }

    TotalSizeBytes = FileSizeBytes + sizeof(HEADER) + 32;
    FileSizeBits = FileSizeBytes * 8;

    if (TotalSizeBytes > MAXFILESIZE){
        printf("ERROR: Data exceeds supported size limit!\n");
        exit(7);
    }

    unsigned char* imageData = getImage(inputPath, &Width, &Height, &Channels);
    if (!imageData){ printf("ERROR: Failed to load input image!\n"); exit(8); }

    imageData = normalizeImage(imageData, Width, Height, &Channels, inputPath, finalOutputPath);

    short isText = (strcmp(outFormat, "txt") == 0);

    HEADER header = assignHeader(outFormat, FileSizeBits, BitsPerChannel, isText);

    ImgSizeBits = (size_t)(Width * Height * Channels * BitsPerChannel);

    if (ImgSizeBits < TotalSizeBytes * 8){
        printf("ERROR: Image too small to hold this data!\n");
        exit(9);
    }

    ImagePack(imageData, ImgSizeBits, header, content, FileSizeBytes, dataFileHash, BitsPerChannel);

    createImage(finalOutputPath, (char*)imageData, Width, Height, Channels);

    if(DEBUG){
        printf("\n\nImageSizeBits = %lld\n", ImgSizeBits);
        printf("TotalSizeBytes = %lld\n", TotalSizeBytes);

        unsigned short BitsPerChannel = bitGet(header.SIZE, 29, 3);
        size_t outFileSizeBit = bitGet(header.SIZE, 0, 28);

        printf("Actual FileSize (Bits): %lld\n", FileSizeBits);
        printf("Encoded FileSize (Bits): %lld\n", outFileSizeBit);
        printf("Bits Per Channel: %u", BitsPerChannel);
    }

    freeImage(imageData);
    free(finalOutputPath);
    free(content);
    free(HashedPasskey);
    free(dataFileHash);

    printf("\n\n Images are prone to Corruption, best pratice to check them yourself first\nIf invalid, try again at a lower Bit per Channel\n\n");
}
