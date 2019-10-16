#ifndef TESTS_TRAINING_EXAMPLE_H
#define TESTS_TRAINING_EXAMPLE_H

typedef struct {

    float* inputs;
    float* label;

} TrainingExample;

TrainingExample* newTrainingExample(float* inputs, float* label);
void destroyTrainingExample(TrainingExample* example);

#endif //TESTS_TRAINING_EXAMPLE_H
