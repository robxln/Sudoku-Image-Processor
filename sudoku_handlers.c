#include "sudoku_handlers.h"

int abs_int(int val) {
    if (val >= 0)
        return val;
    return 0 - val;
}

pixel convertToPixel(char c, pixel Color1, pixel Color2) {
    if (c == '.')
        return Color1;
    else
        return Color2;
}

int isEqual_pixel(pixel a, pixel b) {
    if (a.R == b.R && a.G == b.G && a.B == b.B)
        return 1;
    return 0;
}

int checkNumber(BMP *sudoku, int row, int col, int nr) {
    pixel alb = {.B = 255, .G = 255, .R = 255};
    pixel roz = {.B = 175, .G = 175, .R = 255};
    for (int i = 0; i < NUMBER_BOARD_SIZE; i++) {
        for (int j = 0; j < NUMBER_BOARD_SIZE; j++) {
            pixel curr = convertToPixel(SudokuBox[nr][i][j], alb, roz);
            if (!isEqual_pixel(curr, sudoku->image[row + i][col + j]))
                return 0;
        }
    }
    return 1;
}

int convertToIntMatrix_sudoku(BMP *sudoku, int **sudokuMat) {
    int col = 0, row = 0;
    for (int i = 1; i < sudoku->infoHeader.height; i += 8) {
        for (int j = 1; j < sudoku->infoHeader.width; j += 8) {
            int nr;
            for (nr = 0; nr <= 9; nr++)
                if (checkNumber(sudoku, i, j, nr) == 1)
                    break;
            if (nr == 10)
                return 0;
            sudokuMat[row][col] = nr;
            col++;
            if (col == SUDOKU_BOARD_SIZE) {
                row++;
                col = 0;
            }
        }
    }
    return 1;
}

int checkLines(int **sudokuMat) {
    int ok = 1;
    int *f = (int *)calloc(SUDOKU_BOARD_SIZE, sizeof(int));
    if (f == NULL) {
        fprintf(stderr, "Eraore alocere vector de frecventa! (Lines)");
        exit(EXIT_FAILURE);
    }
    //check lines
    for (int i = 0; i < SUDOKU_BOARD_SIZE && ok; i++) {
        for (int j = 1; j <= 9; j++)
            f[j] = 0;
        for (int j = 0; j < SUDOKU_BOARD_SIZE && ok; j++) {
            if (sudokuMat[i][j] == 0 || f[ sudokuMat[i][j] ] == 1)
                ok = 0;
            f[ sudokuMat[i][j] ] = 1;
        }
    }
    free(f);
    return ok;
}

int checkColumns(int ** sudokuMat) {
    int ok = 1;
    int *f = (int *)calloc(SUDOKU_BOARD_SIZE, sizeof(int));
    if (f == NULL) {
        fprintf(stderr, "Eraore alocere vector de frecventa! (Lines)");
        exit(EXIT_FAILURE);
    }
    //check coloums
    for (int j = 0; j < SUDOKU_BOARD_SIZE && ok; j++) {
        for (int i = 1; i <= 9; i++)
            f[i] = 0;
        for (int i = 0; i < SUDOKU_BOARD_SIZE && ok; i++) {
            if (sudokuMat[i][j] == 0 || f[ sudokuMat[i][j] ] == 1)
                ok = 0;
            f[ sudokuMat[i][j] ] = 1;
        }
    }
    free(f);
    return ok;
}

int checkSquare(int **sudokuMat) {
    int ok = 1;
    int *f = (int *)calloc(SUDOKU_BOARD_SIZE, sizeof(int));
    if (f == NULL) {
        fprintf(stderr, "Eraore alocere vector de frecventa! (Lines)");
        exit(EXIT_FAILURE);
    }
    //check 3x3 squares
    for (int row = 0; row < SUDOKU_BOARD_SIZE && ok; row += 3) {
        for (int col = 0; col < SUDOKU_BOARD_SIZE && ok; col += 3) {
            for (int i = 0; i <= 9; i++)
                f[i] = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (sudokuMat[row + i][col + j] == 0 || 
                        f[ sudokuMat[row + i][col + j] ] == 1)
                        ok = 0;
                    f[ sudokuMat[row + i][col + j] ] = 1;
                }
            }
        }
    }
    free(f);
    return ok;
}

int checkWinSudoku(BMP *sudoku) {
    int **sudokuMat = (int **) calloc(SUDOKU_BOARD_SIZE, sizeof(int *));
    if (sudokuMat == NULL) {
        fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        sudokuMat[i] = (int *) calloc(SUDOKU_BOARD_SIZE, sizeof(int));
        if (sudokuMat[i] == NULL) {
            fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
            for (int j = 0; j <= i; j++)
                free(sudokuMat[j]);
            free(sudokuMat);
            exit(EXIT_FAILURE);
        }
    }
    int ok = convertToIntMatrix_sudoku(sudoku, sudokuMat);
    if (ok == 0) {
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
            free(sudokuMat[i]);
        free(sudokuMat);
        return 0;
    }
    
    ok = 1;
    ok = (ok & checkLines(sudokuMat));
    ok = (ok & checkColumns(sudokuMat));
    ok = (ok & checkSquare(sudokuMat));
    
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
        free(sudokuMat[i]);
    free(sudokuMat);
    return ok;
}

void solveSudoku(int **sudokuMat) {
    //numar cate cifre trebuie sa completez
    int count = 0;
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
        for (int j = 0; j < SUDOKU_BOARD_SIZE; j++)
            if (sudokuMat[i][j] == 0)
                count++;
    while (count != 0) {
        //completez cifrele unic determinate pe linii
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
            int nr = 0, sum = 0;
            for (int j = 0; j < SUDOKU_BOARD_SIZE; j++) 
                if (sudokuMat[i][j] == 0)
                    nr++;
                else
                    sum += abs_int(sudokuMat[i][j]);
            if (nr == 1) {
                for (int j = 0; j < SUDOKU_BOARD_SIZE; j++)
                    if (sudokuMat[i][j] == 0) {
                        sudokuMat[i][j] = sum - MAX_SUM;
                        break;
                    }
                count--;
            }
        }
        //completez cifrele unic determinate pe coloane
        for (int j = 0; j < SUDOKU_BOARD_SIZE; j++) {
            int nr = 0, sum = 0;
            for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
                if (sudokuMat[i][j] == 0)
                    nr++;
                else
                    sum += abs_int(sudokuMat[i][j]);
            if (nr == 1) {
                for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
                    if (sudokuMat[i][j] == 0) {
                        sudokuMat[i][j] = sum - MAX_SUM;
                        break;
                    }
                count--;
            }
        }
        //completez cifrele unic determinate pe chenar 3x3
        for (int row = 0; row < SUDOKU_BOARD_SIZE; row += 3) {
            for (int col = 0; col < SUDOKU_BOARD_SIZE; col += 3) {
                int nr = 0, sum = 0;
                for (int i = 0; i < 3; i++)
                    for (int j = 0; j < 3; j++)
                        if (sudokuMat[row + i][col + j] == 0)
                            nr++;
                        else
                            sum += abs_int(sudokuMat[row + i][col + j]);
                if (nr == 1) {
                     for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            if (sudokuMat[row + i][col + j] == 0) {
                                sudokuMat[row + i][col + j] = sum - MAX_SUM;
                                break;
                            }
                    count--;
                }
            }
        }
    }
}

void putNumber(BMP *sudoku, int row, int col, int nr, pixel Clr1, pixel Clr2) {
    for (int i = 0; i < NUMBER_BOARD_SIZE; i++) {
        for (int j = 0; j < NUMBER_BOARD_SIZE; j++) {
            if (SudokuBox[nr][i][j] == '.')
                sudoku->image[row + i][col + j] = Clr1;
            else
                sudoku->image[row + i][col + j] = Clr2;
        }
    }
}

int getRelPos(int index) {
    return 1 + index * 8;
}

void solveSudokuUniq(BMP *sudoku) {
    int **sudokuMat = (int **) calloc(SUDOKU_BOARD_SIZE, sizeof(int *));
    if (sudokuMat == NULL) {
        fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        sudokuMat[i] = (int *) calloc(SUDOKU_BOARD_SIZE, sizeof(int));
        if (sudokuMat[i] == NULL) {
            fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
            for (int j = 0; j < i; j++)
                free(sudokuMat[j]);
            exit(EXIT_FAILURE);
        }
    }
    int ok = convertToIntMatrix_sudoku(sudoku, sudokuMat);
    //0 inseamna ca sunt caractere nepermise in bmp cu jocul de sudoku
    if (ok == 0){
        fprintf(stderr, "La task4 nu avem un joc de sudoku valid");
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
            free(sudokuMat[i]);
        free(sudokuMat);
        exit(EXIT_FAILURE);
    }
    solveSudoku(sudokuMat);
    pixel alb = {.B = 255, .G = 255, .R = 255};
    pixel magenta = {.B = 255, .G = 0, .R = 255};
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        for (int j = 0; j < SUDOKU_BOARD_SIZE; j++) {
            //numerele negative sunt cele noi adaugate
            if (sudokuMat[i][j] < 0) {
                putNumber(sudoku, getRelPos(i), getRelPos(j), 
                        -sudokuMat[i][j], alb, magenta);
            }
        }
    }
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
        free(sudokuMat[i]);
    free(sudokuMat);
}

int canPutVal(int **matrix, int row, int col) {
    //check element on line
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        if (col != i) {
            if (abs_int(matrix[row][i]) == abs_int(matrix[row][col])) 
                return 0;
        }
    }
    //check element on column
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        if (row != i) {
            if (abs_int(matrix[i][col]) == abs_int(matrix[row][col]))
                return 0;
        }
    }
    //check element on square
    //calculez coltul stanga sus al patratului
    int x = (row / 3) * 3, y = (col / 3) * 3;
    for (int i = x; i < x + 3; i++)
        for (int j = y; j < y + 3; j++)
            if (row != i && col != j)
                if (abs_int(matrix[i][j]) == abs_int(matrix[row][col]))
                    return 0;
    return 1;
}

int backtracking(int **matrix, int row, int col) {
    if (row == SUDOKU_BOARD_SIZE && col == 0)
        return 1;
    if (matrix[row][col] != 0) {
        if (col + 1 < SUDOKU_BOARD_SIZE)
            return backtracking(matrix, row, col + 1);
        else
            return backtracking(matrix, row + 1, 0);
    } else {
        int ok = 0;
        for (int val = 1; val <= 9; val++) {
            matrix[row][col] = -val;
            if (canPutVal(matrix, row, col) == 1) {
                if (col + 1 < SUDOKU_BOARD_SIZE)
                    ok = backtracking(matrix, row, col + 1);
                else
                    ok = backtracking(matrix, row + 1, 0);
                if (ok == 1)
                    return 1;
            }
            //resetez valorea la intoarcerea din apelul recursiv
            matrix[row][col] = 0;
        }
    }
    return 0;
}

void solveSudokuBkt(BMP *sudoku) {
    pixel alb = {.B = 255, .G = 255, .R = 255};
    pixel magenta = {.B = 255, .G = 0, .R = 255};
    pixel rosu = {.B = 0, .G = 0, .R = 255};
    int **sudokuMat = (int **) calloc(SUDOKU_BOARD_SIZE, sizeof(int *));
    if (sudokuMat == NULL) {
        fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
        sudokuMat[i] = (int *) calloc(SUDOKU_BOARD_SIZE, sizeof(int));
        if (sudokuMat[i] == NULL) {
            fprintf(stderr, "Eroare alocare matrice de sudoku (int)!");
            for (int j = 0; j < i; j++)
                free(sudokuMat[j]);
            exit(EXIT_FAILURE);
        }
    }
    int ok = convertToIntMatrix_sudoku(sudoku, sudokuMat);
    //0 inseamna ca sunt caractere nepermise in bmp cu jocul de sudoku
    if (ok == 0){
        fprintf(stderr, "La task4 nu avem un joc de sudoku valid");
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
            free(sudokuMat[i]);
        free(sudokuMat);
        exit(EXIT_FAILURE);
    }
    ok = backtracking(sudokuMat, 0, 0);
    if (ok == 1) {
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
            for (int j = 0; j < SUDOKU_BOARD_SIZE; j++) {
                //numerele negative sunt cele noi adaugate
                if (sudokuMat[i][j] < 0) {
                    putNumber(sudoku, getRelPos(i), getRelPos(j), 
                            -sudokuMat[i][j], alb, magenta);
                }
            }
        }
    } else {
        for (int i = 0; i < SUDOKU_BOARD_SIZE; i++) {
            for (int j = 0; j < SUDOKU_BOARD_SIZE; j++) {
                //peste tot pun x rosu
                putNumber(sudoku, getRelPos(i), getRelPos(j), 10, alb, rosu);
            }
        }
    }
    for (int i = 0; i < SUDOKU_BOARD_SIZE; i++)
        free(sudokuMat[i]);
    free(sudokuMat);
}