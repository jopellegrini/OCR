#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

char* writeint(char* buffer, int i, char end) ;
char* writedouble(char* buffer, double f, char end) ;
int readint(char** str) ;
double readdouble(char** str) ;

char* readfile(char* path) ;
void writefile(char* path, char* content) ;
char* readline(FILE* file, int length) ;

#endif //FILE_UTILS_H
