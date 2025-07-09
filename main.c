#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLEAR_SCREEN() printf("\e[H\e[2J");

void drawGrid(unsigned int* grid)
{
    unsigned int* pos = grid;
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            *pos ? printf("\e[1m%u\e[0m\t", *pos) : printf("x\t");
            pos++;
        }
        printf("\n");
    }
}

int updateGrid(unsigned int* grid, char move)
{
    int changed = 0;
    int i, j;
    switch (move) {
    case 'w':
        for (i = 1; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                unsigned int* currCell = grid + i * 4 + j;
                unsigned int currCellValue = *currCell;
                unsigned int* newCell;
                int shift = 0;
                if (*currCell) {
                    *currCell = 0;
                    while (shift <= i && !*(currCell - 4 * shift - 4)) {
                        shift++;
                        changed = 1;
                    }
                    if (shift <= i && currCellValue == *(currCell - shift * 4 - 4)) {
                        shift++;
                        changed = 1;
                        currCellValue *= 2;
                    }
                    newCell = currCell - 4 * shift;
                    *newCell = currCellValue;
                }
            }
        }
        break;
    case 'a':
        for (i = 1; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                unsigned int* currCell = grid + i + 4 * j;
                unsigned int currCellValue = *currCell;
                unsigned int* newCell;
                int shift = 0;
                if (*currCell) {
                    *currCell = 0;
                    while (shift < i && !*(currCell - shift - 1)) {
                        shift++;
                        changed = 1;
                    }
                    if (shift < i && currCellValue == *(currCell - shift - 1)) {
                        shift++;
                        changed = 1;
                        currCellValue *= 2;
                    }
                    newCell = currCell - shift;
                    *newCell = currCellValue;
                }
            }
        }
        break;
    case 's':
        for (i = 2; i >= 0; i--) {
            for (j = 0; j < 4; j++) {
                unsigned int* currCell = grid + i * 4 + j;
                unsigned int currCellValue = *currCell;
                unsigned int* newCell;
                int shift = 0;
                if (*currCell) {
                    *currCell = 0;
                    while (shift < 3 - i && !*(currCell + 4 * shift + 4)) {
                        shift++;
                        changed = 1;
                    }
                    if (shift < 3 - i && currCellValue == *(currCell + shift * 4 + 4)) {
                        shift++;
                        changed = 1;
                        currCellValue *= 2;
                    }
                    newCell = currCell + 4 * shift;
                    *newCell = currCellValue;
                }
            }
        }
        break;
    case 'd':
        for (i = 2; i >= 0; i--) {
            for (j = 0; j < 4; j++) {
                unsigned int* currCell = grid + i + 4 * j;
                unsigned int currCellValue = *currCell;
                unsigned int* newCell;
                int shift = 0;
                if (*currCell) {
                    *currCell = 0;
                    while (shift < 3 - i && !*(currCell + shift + 1)) {
                        shift++;
                        changed = 1;
                    }
                    if (shift < 3 - i && currCellValue == *(currCell + shift + 1)) {
                        shift++;
                        changed = 1;
                        currCellValue *= 2;
                    }
                    newCell = currCell + shift;
                    *newCell = currCellValue;
                }
            }
        }
        break;
    }
    return changed;
}

int insertNumber(unsigned int* grid)
{
    int counter = 0;
    int emptyFields[16] = { 0 };
    int i;
    // Collect empty fields
    for (i = 0; i < 16; i++) {
        if (!grid[i]) {
            emptyFields[counter] = i;
            counter++;
        }
    }
    // fill random field
    if (counter == 0) {
        return 1;
    }
    int randField = rand() / ((RAND_MAX + 1u) / counter);
    unsigned int chosenField = emptyFields[randField];
    int cellContent = 1;
    double unweightedRand = (double)rand() / RAND_MAX;
    double comp = 0.5;
    counter = 1;
    while (unweightedRand > comp) {
        cellContent *= 2;
        counter++;
        comp += (double)1 / (1 << counter);
    }
    grid[chosenField] = cellContent;
    return 0;
}

void updateScore(unsigned int* grid, unsigned int* score)
{
    int i = 0;
    for (i = 0; i < 16; i++) {
        *score += grid[i];
    }
}

int validMoves(unsigned int* grid)
{
    int i, j;
    int lastNumber = 0;

    // Do 0 occur
    for (i = 0; i < 16; i++)
        if (!*(grid + i)) return 1;

    // X-axis
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (*(grid + 4 * i + j) == lastNumber) {
                return 1;
            }
            lastNumber = *(grid + 4 * i + j);
        }
        lastNumber = 0;
    }

    // Y-axis
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (*(grid + i + 4 * j) == lastNumber) {
                return 1;
            }
            lastNumber = *(grid + i + 4 * j);
        }
        lastNumber = 0;
    }
    return 0;
}

int main(int argc, char** argv)
{
    unsigned int grid[16] = { 0u };
    int input;
    unsigned int score = 0;
    srand(time(NULL));
    for (;;) {
        CLEAR_SCREEN();
        insertNumber(grid);
        updateScore(grid, &score);
        drawGrid(grid);
        if (!validMoves(grid)) {
            printf("SCORE: %u\n", score);
            return 0;
        }
        do {
            while (!strchr("wasd", input = getchar())) { }
        } while (!updateGrid(grid, input));
    }
}
