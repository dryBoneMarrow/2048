#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLEAR_SCREEN() printf("\e[H\e[2J");

char* numberColored(unsigned int number)
{
    switch (number) {
    case 2:
        return "\e[31m2\e[0m";
    case 4:
        return "\e[32m4\e[0m";
    case 8:
        return "\e[33m8\e[0m";
    case 16:
        return "\e[34m16\e[0m";
    case 32:
        return "\e[35m32\e[0m";
    case 64:
        return "\e[36m64\e[0m";
    case 128:
        return "\e[38;5;227;m128\e[0m";
    case 256:
        return "\e[38;5;183;m256\e[0m";
    case 512:
        return "\e[38;5;205;m512\e[0m";
    case 1024:
        return "\e[38;5;32;m1024\e[0m";
    case 2048:
        return "\e[38;5;35;m2048\e[0m";
    case 4096:
        return "\e[38;5;174;m4096\e[0m";
    case 8192:
        return "\e[38;5;202;m256\e[0m";
    default:
        char* string;
        sprintf(string, "\e[38;5;122%u\e[0m", number);
        return string;
    }
}

void drawGrid(unsigned int* grid)
{
    puts("Move using wasd, confirm move with enter. q to quit\n");
    unsigned int* pos = grid;
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            *pos ? printf("\e[1m%s\e[0m\t", numberColored(*pos)) : printf("x\t");
            pos++;
        }
        printf("\n\n\n");
    }
}

int iCondWA(int i) { return i < 4; }
int iCondSD(int i) { return i >= 0; }
int iUpdateWA(int i) { return ++i; }
int iUpdateSD(int i) { return --i; }
int shiftLimitWA(int i) { return i; }
int shiftLimitSD(int i) { return 3 - i; }

int updateGrid(unsigned int* grid, char move)
{
    int changed = 0;
    int i, j, k;
    unsigned int* mergedCells[16] = { 0 };
    int mergedCellsCounter = 0;

    typedef struct {
        int i_start;
        int (*iCond)(int);
        int (*iUpdate)(int);
        int move_one;
        int (*shiftLimit)(int);
        int shiftMult;
        int jMult;
        int iMult;
    } directionParams;

    directionParams dir;

    switch (move) {
    case 'w':
        dir.i_start = 1;
        dir.iCond = iCondWA;
        dir.iUpdate = iUpdateWA;
        dir.move_one = -4;
        dir.shiftLimit = shiftLimitWA;
        dir.shiftMult = -4;
        dir.jMult = 1;
        dir.iMult = 4;
        break;
    case 'a':
        dir.i_start = 1;
        dir.iCond = iCondWA;
        dir.iUpdate = iUpdateWA;
        dir.move_one = -1;
        dir.shiftLimit = shiftLimitWA;
        dir.shiftMult = -1;
        dir.jMult = 4;
        dir.iMult = 1;
        break;
    case 's':
        dir.i_start = 2;
        dir.iCond = iCondSD;
        dir.iUpdate = iUpdateSD;
        dir.move_one = 4;
        dir.shiftLimit = shiftLimitSD;
        dir.shiftMult = 4;
        dir.jMult = 1;
        dir.iMult = 4;
        break;
    case 'd':
        dir.i_start = 2;
        dir.iCond = iCondSD;
        dir.iUpdate = iUpdateSD;
        dir.move_one = 1;
        dir.shiftLimit = shiftLimitSD;
        dir.shiftMult = 1;
        dir.jMult = 4;
        dir.iMult = 1;
        break;
    default:
        return changed;
    }
    for (i = dir.i_start; dir.iCond(i); i = dir.iUpdate(i)) {
        for (j = 0; j < 4; j++) {
            unsigned int* currCell = grid + i * dir.iMult + j * dir.jMult;
            unsigned int currCellValue = *currCell;
            unsigned int* newCell;
            int shift = 0;
            if (*currCell) {
                *currCell = 0;
                while (shift < dir.shiftLimit(i)
                    && !*(currCell + shift * dir.shiftMult + dir.move_one)) {
                    shift++;
                    changed = 1;
                }
                if (shift < dir.shiftLimit(i)
                    && currCellValue == *(currCell + shift * dir.shiftMult + dir.move_one)) {
                    int alreadymergerged = 0;
                    for (k = 0; k < mergedCellsCounter; k++) {
                        if ((currCell + shift * dir.shiftMult + dir.move_one) == mergedCells[k]) {
                            alreadymergerged = 1;
                            break;
                        }
                    }
                    if (!alreadymergerged) {
                        shift++;
                        changed = 1;
                        currCellValue *= 2;
                        mergedCells[mergedCellsCounter] = currCell + dir.move_one * shift;
                        mergedCellsCounter++;
                    }
                }
                newCell = currCell + dir.move_one * shift;
                *newCell = currCellValue;
            }
        }
    }

    if (!changed) puts("Can't move in this direction");
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
    double unweightedRand = (double)rand() / RAND_MAX;
    int cellContent = (unweightedRand < 0.75) ? 2 : 4;
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
    insertNumber(grid);
    for (;;) {
        insertNumber(grid);
        updateScore(grid, &score);
        CLEAR_SCREEN();
        drawGrid(grid);
        if (!validMoves(grid)) {
            printf("SCORE: %u\n", score);
            return 0;
        }
        do {
            while (!strchr("wasd", input = getchar())) {
                if (input == 'q') {
                    printf("SCORE: %u\n", score);
                    return 0;
                }
            }
        } while (!updateGrid(grid, input));
    }
}
