#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POLL_RATE 1

int main(int argc, char **argv) {
    unsigned char buffer[512];
    FILE *input = NULL;
    FILE *output = NULL;
    
    if((input = fopen(argv[1], "rb")) == NULL) {
        perror("Error opening input file");
        getchar();
        return 1;
    }

    if((output = fopen(argv[2], "wb")) == NULL) {
        perror("Error opening output file");
        getchar();
        return 1;
    }

    // Calculate size in buffers
    fseek(input, 0, SEEK_END); // seek to end of file
    long size = (int)ftell(input);
    rewind(input);

    long progress = 0;
    long start = time(NULL);
    while (fread(buffer, 1, sizeof(buffer), input) > 0) {
        progress += fwrite(buffer, 1, sizeof(buffer), output);
        if (time(NULL) > start) {
            printf("\rCopying files, %.2f%%", (float)(progress / size * 100));
            start = time(NULL);
        }
    }
    printf("\rCopying files, 100%%    \n");

    fclose(input);
    fclose(output);

    
    return 0;
}