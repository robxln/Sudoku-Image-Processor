#include "bmp_handlers.h"
#include "cJSON.h"

void allocBMP_Image(BMP *game) {
    game->image = (pixel **)calloc(game->infoHeader.height, sizeof(pixel *));
    if (game->image == NULL) {
        free(game->image);
        fprintf(stderr, "Eroare alocare matrice de pixeli!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < game->infoHeader.height; i++) {
        game->image[i] = (pixel *)calloc(game->infoHeader.width, sizeof(pixel));
        if (game->image[i] == 0) {
            for (int j = 0; j <= i; j++)
                free(game->image[j]);
            free(game->image);
            fprintf(stderr, "Eroare alocare matrice de pixeli!");
            exit(EXIT_FAILURE);
        }
    }
}

void freeBMP(BMP *game) {
    for (int i = 0; i < game->infoHeader.height; i++)
        free( game->image[i] );
    free(game->image);
    return;
}

//JSON handlers

//JSON readers
void readFileHeader(char *buffer, BMP *game) {
    cJSON *parsed_json, *offset, *signature, *reserved, *file_zise, *file_header;
    parsed_json = cJSON_Parse(buffer);
    if (parsed_json == NULL) {
        fprintf(stderr, "Eroare la parsare json!");
        cJSON_Delete(parsed_json);
        exit(EXIT_FAILURE);
    }
    file_header = cJSON_GetObjectItemCaseSensitive(parsed_json, "file_header");

    offset = cJSON_GetObjectItemCaseSensitive(file_header, "offset");
    game->fileHeader.imageDataOffset = (unsigned int)offset->valueint;

    signature = cJSON_GetObjectItemCaseSensitive(file_header, "signature");
    game->fileHeader.fileMarker1 = (unsigned char) *(signature->valuestring);
    game->fileHeader.fileMarker2 = (unsigned char) *(signature->valuestring + 1);

    reserved = cJSON_GetObjectItemCaseSensitive(file_header, "reserved");
    game->fileHeader.unused1 = (unsigned short)(reserved->valueint & (0xFFFF0000));
    game->fileHeader.unused2 = (unsigned short)(reserved->valueint & (0xFFFF));

    file_zise = cJSON_GetObjectItemCaseSensitive(file_header, "file_size");
    game->fileHeader.bfSize = (unsigned int)file_zise->valueint;

    cJSON_Delete(parsed_json);
}

void readInfoHeader(char *buffer, BMP *game) {
    cJSON *parsed_json, *colors_used, *size, *image_size, *yppm, *xppm, *planes,
     *clrs_imp, *width, *bit_count, *height, *compression, *info_header;
    parsed_json = cJSON_Parse(buffer);
    if (parsed_json == NULL) {
        fprintf(stderr, "Eroare la parsare json!");
        cJSON_Delete(parsed_json);
        exit(EXIT_FAILURE);
    }
    info_header = cJSON_GetObjectItemCaseSensitive(parsed_json, "info_header");

    size = cJSON_GetObjectItemCaseSensitive(info_header, "size");
    game->infoHeader.biSize = (unsigned int)size->valueint;

    width = cJSON_GetObjectItemCaseSensitive(info_header, "width");
    game->infoHeader.width = (signed int)width->valueint;

    height = cJSON_GetObjectItemCaseSensitive(info_header, "height");
    game->infoHeader.height = (signed int)height->valueint;

    planes = cJSON_GetObjectItemCaseSensitive(info_header, "planes");
    game->infoHeader.planes = (unsigned short)planes->valueint;

    bit_count = cJSON_GetObjectItemCaseSensitive(info_header, "bit_count");
    game->infoHeader.bitPix = (unsigned short)bit_count->valueint;

    compression = cJSON_GetObjectItemCaseSensitive(info_header, "compression");
    game->infoHeader.biCompression = (unsigned int)compression->valueint;

    image_size = cJSON_GetObjectItemCaseSensitive(info_header, "image_size");
    game->infoHeader.biSizeImage = (unsigned int)image_size->valueint;

    xppm = cJSON_GetObjectItemCaseSensitive(info_header, "x_pixels_per_meter");
    game->infoHeader.biXPelsPerMeter = xppm->valueint;

    yppm = cJSON_GetObjectItemCaseSensitive(info_header, "y_pixels_per_meter");
    game->infoHeader.biYPelsPerMeter =yppm->valueint;

    colors_used = cJSON_GetObjectItemCaseSensitive(info_header, "colors_used");
    game->infoHeader.biClrUsed = (unsigned int)colors_used->valueint;

    clrs_imp = cJSON_GetObjectItemCaseSensitive(info_header, "colors_important");
    game->infoHeader.biClrImportant = (unsigned int)clrs_imp->valueint;

    cJSON_Delete(parsed_json);
}

void readBitMap(char *buffer, BMP *game) {
    cJSON *parsed_json, *bitmap, *bit;
    parsed_json = cJSON_Parse(buffer);
    if (parsed_json == NULL) {
        fprintf(stderr, "Eroare la parsare json!");
        cJSON_Delete(parsed_json);
        exit(EXIT_FAILURE);
    }
    bitmap = cJSON_GetObjectItemCaseSensitive(parsed_json, "bitmap");

    int i = 0, j = 0, size;
    size = cJSON_GetArraySize(bitmap);
    for (int p = 0; p < size; p += 3) {
        bit = cJSON_GetArrayItem(bitmap, p);
        game->image[i][j].B = (unsigned char)bit->valueint;
        bit = cJSON_GetArrayItem(bitmap, p + 1);
        game->image[i][j].G = (unsigned char)bit->valueint;
        bit = cJSON_GetArrayItem(bitmap, p + 2);
        game->image[i][j].R = (unsigned char)bit->valueint;
        j++;
        if (j == game->infoHeader.width) {
            i++;
            j = 0;
        }
    }

    cJSON_Delete(parsed_json);
}

void readJSON(FILE *input, BMP *game) {
    char *buffer = (char *)calloc(JSON_BUFFER_SIZE, sizeof(char));
    if (buffer == NULL) {
        free(buffer);
        fprintf(stderr, "Eroare alocare buffer!");
        exit(EXIT_FAILURE);
    }
    
    fread(buffer, JSON_BUFFER_SIZE, 1, input);
    readFileHeader(buffer, game);
    readInfoHeader(buffer, game);
    allocBMP_Image(game);
    readBitMap(buffer, game);
    free(buffer);
}

//JSON printers
int findPadding(int bytes) {
    if(bytes % 4 == 0)
        return 0;
    int next4Multiple = (bytes / 4 + 1) * 4;
    return next4Multiple - bytes;
}

void printPadding(int bytes, FILE *output) {
    unsigned char padding = 0;
    for (int i = 1; i <= bytes; i++)
        fwrite(&padding, sizeof(unsigned char), 1, output);
}

//Wrtie BMP
void printImage(FILE *output, BMP sudoku) {
    fwrite(&sudoku.fileHeader, sizeof(bmp_fileheader), 1, output);
    fwrite(&sudoku.infoHeader, sizeof(bmp_infoheader), 1, output);
    for (int i = sudoku.infoHeader.height - 1; i >= 0; i--) {
        int printedBytes = sudoku.infoHeader.width * sizeof(pixel);
        fwrite(sudoku.image[i], sizeof(pixel), sudoku.infoHeader.width, output);
        printPadding(findPadding(printedBytes), output);
    }
}

//Read BMP
void readBmp(FILE *in, BMP *sudoku) {
    fread(&sudoku->fileHeader,sizeof(bmp_fileheader), 1, in);
    fread(&sudoku->infoHeader, sizeof(bmp_infoheader), 1, in);
    allocBMP_Image(sudoku);
    int offset = sudoku->fileHeader.imageDataOffset;
    for (int i = sudoku->infoHeader.height - 1; i >= 0; i--) {
        fseek(in, offset, SEEK_SET);
        fread(sudoku->image[i], sizeof(pixel), sudoku->infoHeader.width, in);
        offset += sizeof(pixel) * sudoku->infoHeader.width;
        offset += findPadding(sizeof(pixel) * sudoku->infoHeader.width);
    }
}

//Mirror for BMP - Task2
void swap_uchar(unsigned char *a, unsigned char *b) {
    unsigned char aux = *a;
    *a = *b;
    *b = aux;
}

void swap_pixel(pixel *a, pixel *b) {
    swap_uchar(&a->R, &b->R);
    swap_uchar(&a->G, &b->G);
    swap_uchar(&a->B, &b->B);
}

void mirror(int row_up, int row_dn, int col_le, int col_rg, BMP *sudoku) {
    if (row_dn > sudoku->infoHeader.height || 
        col_rg > sudoku->infoHeader.width) 
            return;

    for (int i = row_up; i < row_dn; i++) {
        int j1 = col_le, j2 = col_rg;
        while (j1 < j2) {
            swap_pixel(&sudoku->image[i][j1], &sudoku->image[i][j2]);
            j1++;
            j2--;
        }
    }
}

void mirrorImage(BMP *sudoku) {
    for (int i = 1; i < sudoku->infoHeader.height; i += 8)
        for (int j = 1; j < sudoku->infoHeader.width; j += 8)
            mirror(i, i + 6, j, j + 6, sudoku);
}

//Task 3 -printer
void generateAndPrintJSON(FILE *out, int ok, char *fileName) {
    char *board = strstr(fileName, "board"), *str;
    str = NULL;
    cJSON *answer = cJSON_CreateObject(), *state = NULL, *input = NULL;
    if (answer == NULL) {
        fprintf(stderr, "Eroare la creare object json");
        cJSON_Delete(answer);
        exit(EXIT_FAILURE);
    }

    input = cJSON_CreateString(board);
    if (input == NULL) {
        fprintf(stderr, "Eroare la creare json object input");
        cJSON_Delete(answer);
        exit(EXIT_FAILURE);
    }
    cJSON_AddItemToObject(answer, "input_file", input);

    if (ok == 1)
        state = cJSON_CreateString("Win!");
    else
        state = cJSON_CreateString("Loss :(");
    if (state  == NULL) {
        fprintf(stderr, "Eroare la creare json object state");
        cJSON_Delete(answer);
        exit(EXIT_FAILURE);
    }
    cJSON_AddItemToObject(answer, "game_state", state);

    str = cJSON_Print(answer);
    if (str == NULL) {
        fprintf(stderr, "Eroare la printare task 3");
        cJSON_Delete(answer);
        exit(EXIT_FAILURE);
    }

    fprintf(out, "%s\n", str);
    free(str);
    cJSON_Delete(answer);
}