#ifndef FILEHANDLER_H
	#define FILEHANDLER_H
    #include <stdio.h>
    #include <stdlib.h>
    #include <time.h>

    #define DEFAULT_WRITE_PATH "C:\\Users\\\%USERPROFILE\%\\Downloads\\"

    long calc_file_size(FILE *f);
    char* trunc_file_size(long file_size, char** str_ptr);
    int check_file_exists(char* fname);

#endif