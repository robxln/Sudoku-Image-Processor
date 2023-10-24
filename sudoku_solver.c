#include "sudoku_handlers.h"

void get_outputFileName(char *output, char *arr, int task) {
    char *pos;
    int len;
    strcpy(output, "output_");
    //put task number
    if (task == 5) {
        strcat(output, "bonus");
    } else {
        strcat(output, "task");
        len = strlen(output);
        output[len] = task + '0';
        output[len + 1] = '\0';
    }
    //get board number
    strcat(output, "_board");
    pos = strstr(arr, "board");
    pos += 5;
    len = strlen(output);
    output[len] = *pos;
    output[len + 1] = '\0';
    pos++;
    len = strlen(output);
    output[len] = *pos;
    output[len + 1] = '\0';
    //put extension depeding on task
    if (task == 3)
        strcat(output, ".json");
    else
        strcat(output, ".bmp");
}

void solve(int task, char *inputFileName, BMP *sudoku) {
    int ok;
    char *outputFileName = (char *)calloc(OUTPUT_FILENAME_SIZE, sizeof(char));
    if (outputFileName == NULL) {
        fprintf(stderr, "Eroare alocare outputFileName");
        exit(EXIT_FAILURE);
    }

    FILE *in, *out;
    in = out = NULL;
    get_outputFileName(outputFileName, inputFileName, task);
    //open files
    if (task == 1)
        in = fopen(inputFileName, "rt");
    if (task == 4 || task == 5)
        in  = fopen(inputFileName, "rb");
    if (in == NULL && (task == 1 || task == 4 || task == 5)) {
        fprintf(stderr, "Eroare la deschidere fisiere input!");
        exit(EXIT_FAILURE);
    }
    if (task == 3)
        out = fopen(outputFileName, "wt");
    else
        out = fopen(outputFileName, "wb");
    if (out == NULL) {
        fprintf(stderr, "Eroare la deschidere fisiere output!");
        if (task == 1 || task == 4 || task == 5)
            fclose(in);
        exit(EXIT_FAILURE);
    }

    switch (task) {
        case 1:
            readJSON(in, sudoku);
            printImage(out, *sudoku);
            break;
        case 2:
            mirrorImage(sudoku);
            printImage(out, *sudoku);
            break;
        case 3:
            ok = checkWinSudoku(sudoku);
            generateAndPrintJSON(out, ok, outputFileName);
            break;
        case 4:
            readBmp(in, sudoku);
            solveSudokuUniq(sudoku);
            printImage(out, *sudoku);
            break;
        case 5:
            readBmp(in, sudoku);
            solveSudokuBkt(sudoku);
            printImage(out, *sudoku);
        default:
            break;
    }

    free(outputFileName);
    if (task == 1 || task == 4 || task == 5)
        fclose(in);
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Numar de argumente incorect");
        return -1;
    }
    BMP sudoku;
    char *task = strdup(argv[2]);
    int len = strlen(task);
    if (len <= 3) {
        for (int i = 0; i < len; i++)
            solve(task[i] - '0', argv[1], &sudoku);
    } else {
        //solve bonus
        solve(5, argv[1], &sudoku);
    }

    free(task);
    freeBMP(&sudoku);
    return 0;
}
