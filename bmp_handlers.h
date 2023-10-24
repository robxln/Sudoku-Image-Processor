#include "sudoku_solver.h"

void allocBMP_Image(BMP *game);
void freeBMP(BMP *game);

void readJSON(FILE *input, BMP *game);
void printImage(FILE *output, BMP sudoku);
void readBmp(FILE *in, BMP *sudoku);

void mirrorImage(BMP *sudoku);
void generateAndPrintJSON(FILE *out, int ok, char *fileName);