#include <stdio.h>
#include <stdlib.h>

#include "training_example.h"
#include "file_utils.h"

TrainingExample* newTrainingExample(double* inputs, double* label) {
    TrainingExample* ret = malloc(sizeof(double*)*2);
    (*ret).inputs = inputs;
    (*ret).label = label;
    return ret;
}

// Returns a random int between min included and max excluded
int randomInt(int min, int max) {
    //Don't forget to initialise the rand() function somewhere in the program:
    //srand((unsigned int)time(NULL));
    return min + (rand() % max);
}

// Creates a new TrainingExample array with nbDest random elements from source
// Doesn't copy the TrainingExamples, so don't free them
TrainingExample** getExamplesSubset(TrainingExample** source, int nbSource, int nbDest) {

    TrainingExample** res = malloc(sizeof(TrainingExample*) * nbDest);
    for(int i = 0; i < nbDest; i++)
        res[i] = source[randomInt(0, nbSource)];
    return res;
}

void destroyTrainingExample(TrainingExample* example) {
    free((*example).inputs);
    free((*example).label);
    free(example);
}

void destroyTrainingExamples(TrainingExample** examples, int nbExamples) {
    for(int i = 0; i < nbExamples; i++)
        destroyTrainingExample(examples[i]);
    free(examples);
}

// Creates a training example from a line in the database
TrainingExample* fromDatabaseLine(char* line, int nbInputs, int nbLabels) {

    double* inputs = malloc(sizeof(double)*nbInputs);
    double* labels = malloc(sizeof(double)*nbLabels);

    for(int i = 0; i < nbInputs; i++)
        inputs[i] = readdouble(&line);
 
    for(int i = 0; i < nbLabels; i++)
        labels[i] = readdouble(&line);

    return newTrainingExample(inputs, labels); 
}

// Reads the database at given location
// The nbExamples pointer will be set to the number of examples read.
// If it's <= 0, the function will read all the examples in the database.
// The database must be of this form (all numbers are doubles): for each example:
// input0 input1 ... inputn label0 label1 ... labeln\n
// The first line must be: nbExamples nbInputsPerEx nbLabelsPerEx
TrainingExample** readDatabase(char* path, int* nbExamples) {

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("File not found: %s\n", path);
        exit(1);
    }    

    //Processes the first line
    char* line = readline(file, 32);
    char* lineStart = line;
    int frstNbr = readint(&line);
    if(*nbExamples <= 0)
        *nbExamples = frstNbr;
    int nbInputs = readint(&line);
    int nbLabels = readint(&line);
    free(lineStart);

    //Calculates the max length of a line
    int length = (nbInputs + nbLabels) * 15;

    //Processes all the examples
    TrainingExample** examples = malloc(sizeof(TrainingExample*) * *nbExamples);
    for (int i = 0; i < *nbExamples; i++) {
        line = readline(file, length);
        lineStart = line;
        examples[i] = fromDatabaseLine(line, nbInputs, nbLabels);
        free(lineStart);
    }
    
    fclose(file);

    return examples;
}
