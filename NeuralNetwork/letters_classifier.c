#include "letters_classifier.h"
#include "network.h"
#include "saving.h"

#include <stdlib.h>
#include <err.h>
#include <math.h>
#include <stdio.h>

Network* network = NULL;

double max(double a, double b) { return a > b ? a : b; }

// Scales the given image to a 28x28 image that the neural network can use
#define SIDELENGTH 28
#define THRESHOLD 0.5
double* scaleForNetwork(int* source, int width, int height) {

    double* res = calloc(SIDELENGTH*SIDELENGTH, sizeof(double));

    double scale = (double)SIDELENGTH/max(width,height);
    int offsetX = 0;
    int offsetY = 0;

    if(width > height)
        offsetY = ceil((width-height)*0.5*scale);
    else if(height > width)
        offsetX = ceil((height-width)*0.5*scale);

    // DOWN SCALING
    if(width > SIDELENGTH || height > SIDELENGTH) {
        int* counts = calloc(SIDELENGTH*SIDELENGTH, sizeof(int));
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                int destX = x*scale + offsetX;
                int destY = y*scale + offsetY;
                res[destY*SIDELENGTH + destX] += source[y*width + x];
                counts[destY*SIDELENGTH + destX]++;
            }
        }
        for(int i = 0; i < SIDELENGTH*SIDELENGTH; i++) {
            if(counts[i] > 1)
                res[i] /= counts[i];
            res[i] = res[i] > THRESHOLD ? 1.0 : 0.0;
        }
        free(counts);
    }
    // UP SCALING
    else {
        for(int y = 0; y < SIDELENGTH; y++) {
            for(int x = 0; x < SIDELENGTH; x++) {
                if(x >= offsetX && x < SIDELENGTH-offsetX
                && y >= offsetY && y < SIDELENGTH-offsetY) {
                    int destX = (x - offsetX)/scale;
                    int destY = (y - offsetY)/scale;
                    res[y*SIDELENGTH + x] = source[destY*width + destX];
                }
            }
        }
    }

    return res;
}

char charFromNeuronIndex(int index) {

    // Numbers
    if(index <= 9)
        return '0' + index;

    // Maj letters
    if(index <= 35)
        return 'A' + index -10;

    // Min letters
    if(index <= 61)
        return 'a' + index -36;

    // Ponctuation
    switch(index) {
        case 62: return '.';
        case 63: return ':';
        case 64: return '!';
        case 65: return '?';
        case 66: return ';';
        case 67: return ',';
        case 68: return '(';
        case 69: return ')';
    }

    return ' ';
}

int neuronIndexFromChar(char c) {

    // Numbers
    if(c >= '0' && c <= '9')
        return c - '0';

    // Maj letters
    if(c >= 'A' && c <= 'Z')
        return c - 'A' + 10;

    // Min letters
    if(c >= 'a' && c <= 'z')
        return c - 'a' + 36;

    // Ponctuation
    switch(c) {
        case '.': return 62;
        case ':': return 63;
        case '!': return 64;
        case '?': return 65;
        case ';': return 66;
        case ',': return 67;
        case '(': return 68;
        case ')': return 69;
    }

    return 0;
}

void initClassifier(char* networkPath) {
    network = readNetwork(networkPath);
}

char classifyImage(int* image, int imageWidth, int imageHeight) {

    if(network == NULL)
        errx(1, "Please call initClassifier before trying to classify images");

    double* scaledImage = scaleForNetwork(image, imageWidth, imageHeight);
    double* rawAnswer = getNetworkAnswer(network, scaledImage);
    free(scaledImage);
    int answer = getMaxIndex(rawAnswer, getOutputNumber(network));
    free(rawAnswer);

    return charFromNeuronIndex(answer);
}
