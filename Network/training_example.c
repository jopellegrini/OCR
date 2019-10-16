#include "training_example.h"
#include <stdlib.h>

TrainingExample* newTrainingExample(float* inputs, float* label) {
    TrainingExample* ret = malloc(sizeof(float*)*2);
    (*ret).inputs = inputs;
    (*ret).label = label;
    return ret;
}

void destroyTrainingExample(TrainingExample* example) {
    free((*example).inputs);
    free((*example).label);
    free(example);
}