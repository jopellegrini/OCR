#include "file_utils.h"

#include <stdlib.h>
#include <stdio.h>

//Adds an int to the buffer and returns a pointer to the first 
//char after the added int and the end character.
char* writeint(char* buffer, int i, char end) {
    int size = sprintf(buffer, "%i", i);
    buffer[size] = end;
    return &buffer[size+1];
}

//Adds an double to the buffer and returns a pointer to the 
//first char after the added double and the end character
char* writedouble(char* buffer, double f, char end) {
    int size = sprintf(buffer, "%lf", f);
    buffer[size] = end;
    return &buffer[size+1];
}

//Reads an int from the str and returns it
//str gets automaticaly moved to the end of the str +1 char
//ex: "12 78" -> "78"
int readint(char** str) {

    char* s = *str;

    //Puts a null terminator at the end so the atoi function works
    int separatorPos = 0;
    while(s[separatorPos] != 0 
            && s[separatorPos] != ' ' && s[separatorPos] != '\n')
        separatorPos++;
    char separator = s[separatorPos];
    s[separatorPos] = '\0';
    
    int res = atoi(s);
    
    //Puts back the separator and moves the str pointer
    s[separatorPos] = separator;
    *str = s+separatorPos+1;

    return res;
}

//Reads an double from the str and returns it
//str gets automaticaly moved to the end of the str +1 char
//ex: "12.0 78.0" -> "78.0"
double readdouble(char** str) {
        
    char* s = *str;

    //Puts a null terminator at the end so the atoi function works
    int separatorPos = 0;
    while(  s[separatorPos] != 0 && 
            s[separatorPos] != ' ' && 
            s[separatorPos] != '\n')
        separatorPos++;
    char separator = s[separatorPos];
    s[separatorPos] = '\0';
    
    double res = strtof(s, NULL);
    
    //Puts back the separator and moves the str pointer
    s[separatorPos] = separator;
    *str = s+separatorPos+1;

    return res;
}

//Reads the entire content of a file
char* readfile(char* path) {
    
    FILE* file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    int filelength = ftell(file);
    rewind(file);

    char* string = malloc(filelength+1);
    if(fread(string, 1, filelength, file) == 0)
        exit(1);
    fclose(file);

    string[filelength] = 0;

    return string;
}

//Writes the string in parameter to the file (overrides current content)
void writefile(char* path, char* content) {
    
    FILE* file = fopen(path, "w+");
    fputs(content, file);
    fclose(file);
}

//Reads the next line of the file, and places the read line in a new string
//The length of the allocated string is length
char* readline(FILE* file, int length) {

    char* line = malloc(sizeof(char) * length);

    int i = 0;
    int c = 0;
    while(i < length && c != '\n' && c != EOF) {
    
        //Reads the character
        c = fgetc(file);
        
        //If it's not EOF, writes it
        if(c != EOF)
            line[i] = (char) c;
    
        i++;   
    }
    line[i] = 0;

    return line;
}
