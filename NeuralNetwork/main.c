#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <err.h>

#include "letters_classifier.h"
#include "saving.h"
#include "training_example.h"

int ___main___(int argc, char** argv) {

    srand((unsigned int)time(NULL));
    
    if(argc < 2 || argc > 3)
        err(1, "Please pass a network and a database in parameter");

    int nbEx = 0;
    TrainingExample** exArray = readDatabase(argv[2], &nbEx);
    TrainingExample* ex = getExamplesSubset(exArray, nbEx, 1)[0];
    
    int width = 245;
    int height = 286;

    int* img = malloc(sizeof(int)*width*height);
    for(int i = 0; i < width*height; i++)
        img[i] = (int)ex->inputs[i];

    //Displays the image in the terminal
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++)
            printf("%c", img[y*width+x] ? 'O' : '.');
        printf("\n");
    }

    initClassifier(argv[1]);
    printf("Answer: %c\n", classifyImage(img, width, height));

    return 0;
}
