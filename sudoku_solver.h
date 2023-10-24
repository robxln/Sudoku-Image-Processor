#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "bmp_header.h"

#define JSON_BUFFER_SIZE 63000
#define OUTPUT_FILENAME_SIZE 35

#pragma pack(1)

typedef struct {
    unsigned char B,G,R;
} pixel;

typedef struct {
    bmp_fileheader fileHeader;
    bmp_infoheader infoHeader;
    pixel **image;
} BMP;

#pragma pack()
