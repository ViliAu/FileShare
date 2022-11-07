#include "filehandler.h"

long calc_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    long size = (long)ftell(f);
    rewind(f);
    return size;
}

char* trunc_file_size(long file_size, char** str_ptr) {
    char* extensions[4] = {"KB", "MB", "GB", "TB"};
    double calc_size = (double)file_size;
    int i = 0;
    while ((calc_size = calc_size / 1024) > 1024) {
        i++;
    }

    int len = snprintf(NULL, 0, "%.2f%s", calc_size, extensions[i]);
    *str_ptr = malloc(len + 1);
    snprintf(*str_ptr, len + 1, "%.2f%s", calc_size, extensions[i]);
}