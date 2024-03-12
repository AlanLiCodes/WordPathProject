#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Declarations of the two functions you will implement
// Feel free to declare any helper functions or global variables
void printPuzzle(char **arr);
void searchPuzzle(char **arr, char *word);
int bSize;
int size = 8;


// Main function, DO NOT MODIFY
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <puzzle file name>\n", argv[0]);
    return 2;
  }
  int i, j;
  FILE *fptr;

  // Open file for reading puzzle
  fptr = fopen(argv[1], "r");
  if (fptr == NULL) {
    printf("Cannot Open Puzzle File!\n");
    return 0;
  }

  // Read the size of the puzzle block
  fscanf(fptr, "%d\n", &bSize);

  // Allocate space for the puzzle block and the word to be searched
  char **block = (char **)malloc(bSize * sizeof(char *));
  char *word = (char *)malloc(20 * sizeof(char));

  // Read puzzle block into 2D arrays
  for (i = 0; i < bSize; i++) {
    *(block + i) = (char *)malloc(bSize * sizeof(char));
    for (j = 0; j < bSize - 1; ++j) {
      fscanf(fptr, "%c ", *(block + i) + j);
    }
    fscanf(fptr, "%c \n", *(block + i) + j);
  }
  fclose(fptr);

  printf("Enter the word to search: ");
  scanf("%s", word);

  // Print out original puzzle grid
  printf("\nPrinting puzzle before search:\n");
  printPuzzle(block);

  // Call searchPuzzle to the word in the puzzle
  searchPuzzle(block, word);

  return 0;
}

void printPuzzle(char **arr) {
  // This function will print out the complete puzzle grid (arr).
  // It must produce the output in the SAME format as the samples
  // in the instructions.
  // Your implementation here...
  for (int row = 0; row < bSize; row++) {
    for (int col = 0; col < bSize; col++) {
      printf("%c ", *(*(arr + row) + col));
    }
    printf("\n");
  }
  printf("\n");
}



//pointer notation for the 8 possible moves



// possible moves at a certain location can be [-1 ,0], [-1, 1], [0, 1], [1, 1],
// [1, 0], [1, -1], [0, -1], [-1, -1]

bool ignoreCase(char a, char b) {
   int offset = 'a' - 'A';

    char lowA = (a >= 'A' && a <= 'Z') ? a + offset : a;
    char lowB = (b >= 'A' && b <= 'Z') ? b + offset : b;

    return lowA == lowB;
}

bool taken(int **path, int x, int y) {
  return *(*(path + x) + y) > 0;
}

bool searchWord(char **arr, int x, int y, char *word, int index, int **path, int step, int n, int *startStep) {
    int *dx = (int*) malloc(8 * sizeof(int));
    int *dy = (int*) malloc(8 * sizeof(int));

    *(dx + 0) = -1;
    *(dy + 0) = 0;
    *(dx + 1) = -1;
    *(dy + 1) = 1;
    *(dx + 2) = 0;
    *(dy + 2) = 1;
    *(dx + 3) = 1;
    *(dy + 3) = 1;
    *(dx + 4) = 1;
    *(dy + 4) = 0;
    *(dx + 5) = 1;
    *(dy + 5) = -1;
    *(dx + 6) = 0;
    *(dy + 6) = -1;
    *(dx + 7) = -1;
    *(dy + 7) = -1;

    if (index == strlen(word))
        return true;
    if (x < 0 || x >= n || y < 0 || y >= n ||
        !ignoreCase(*(*(arr + x) + y), *(word + index))) { // check if out of bounds
        return false;
    }

    int z = path[x][y];
    if (z > 0) {
        // Cell has been visited before, concatenate the current step
        int digit = 1;
        while(step - *startStep >= digit){
            digit *= 10;
        }
        z = z * digit + (step - *startStep);
    } else {
        // First visit to the cell, set the step number
        z = step - *startStep + 1;
    }
    *(*(path + x) + y) = z;
    for (int dir = 0; dir < 8; dir++) {
        if (searchWord(arr, x + *(dx + dir), y + *(dy + dir), word, index + 1, path, step + 1, n, startStep)) {
            return true;
        }
    }

    *(*(path + x) + y) = 0; // reset path
    return false;
}

void searchPuzzle(char **arr, char *word) {
    int **path = (int **)malloc(bSize * sizeof(int *));

    for (int i = 0; i < bSize; i++) {
        *(path + i) = (int *)malloc(bSize * sizeof(int));
        memset(*(path + i), 0, bSize * sizeof(int)); // initialize with zeros
    }

    bool found = false;
    bool secondPathFound = false;
    int firstPathStartX = -1, firstPathStartY = -1;
    int startStep = 1; // Start step for the first path

    // Find the first path
    for (int i = 0; i < bSize && !found; i++) {
        for (int j = 0; j < bSize && !found; j++) {
            if (ignoreCase(*(*(arr + i) + j), *(word + 0))) {
                if (searchWord(arr, i, j, word, 0, path, startStep, bSize, &startStep)) {
                    found = true;
                    firstPathStartX = i;
                    firstPathStartY = j;
                    break;
                }
                *(*(path + i) + j) = 0;
            }
        }
    }

    if (found) {
        printf("Word found!\n");
    } else {
        printf("Word not found!\n");
        goto cleanup;
    }

    startStep = 1; // Reset start step for the second path

    // Find the second path, excluding cells from the first path
    for (int i = 0; i < bSize && !secondPathFound; i++) {
        for (int j = 0; j < bSize && !secondPathFound; j++) {
            if (ignoreCase(*(*(arr + i) + j), *(word + 0)) && !(i == firstPathStartX && j == firstPathStartY)) {
                if (searchWord(arr, i, j, word, 0, path, startStep, bSize, &startStep)) {
                    secondPathFound = true;
                    break;
                }
                *(*(path + i) + j) = 0;
            }
        }
    }

    printf("Printing the search path:\n");
    for (int i = 0; i < bSize; i++) {
        for (int j = 0; j < bSize; j++) {
            if (*(*(path + i) + j) > 0) {
                printf("%-5d\t", *(*(path + i) + j)); // Print the path number with a fixed width
            } else {
              printf("%-5d\t", 0);
            }
        }
        printf("\n");
    }
    printf("\n");

cleanup:
    // Free the allocated memory for path
    for (int i = 0; i < bSize; i++) {
        free(*(path + i));
    }
    free(path);
}
