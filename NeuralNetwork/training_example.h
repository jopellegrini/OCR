#ifndef TRAINING_EXAMPLE_H
#define TRAINING_EXAMPLE_H

typedef struct {

    double* inputs;
    double* label;

} TrainingExample;

TrainingExample* newTrainingExample(double* inputs, double* label);
TrainingExample** getExamplesSubset(TrainingExample** source, int nbSource, int nbDest);
TrainingExample** readDatabase(char* path, int* nbExamples);

void destroyTrainingExample(TrainingExample* example);
void destroyTrainingExamples(TrainingExample** examples, int nbExamples);

#endif //TRAINING_EXAMPLE_H
