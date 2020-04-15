#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <iostream>

#define ARGS_NUM 7
using namespace std;

typedef unsigned char uchar;
enum Errors {
    ARGUMENTS = 1, NO_INPUT, HEADER_PARSING, INPUT_BROKEN, NO_OUTPUT, OUTPUT_ERROR
};
enum ARGS {
    INPUT = 1, OUTPUT, GRADIENT, DITHERING, BITS, GAMMA
};

enum Dither {
    NO_DITHER, ORDERED, RANDOM, FLOYD_STEINBERG, JARVIS, SIERRA, ATKINSON, HALFTONE
};

int orderedMatrix[8][8] = {{0,  48, 12, 60, 3,  51, 15, 63},
                           {32, 16, 44, 28, 35, 19, 47, 31},
                           {8,  56, 4,  52, 11, 59, 7,  55},
                           {40, 24, 36, 20, 43, 27, 39, 23},
                           {2,  50, 14, 62, 1,  49, 13, 61},
                           {34, 18, 46, 30, 33, 17, 45, 29},
                           {10, 58, 6,  54, 9,  57, 5,  53},
                           {42, 26, 38, 22, 41, 25, 37, 21}};

int halftoneMatrix[4][4] = {{12, 5,  6, 13},
                            {4,  0,  1, 7},
                            {11, 3,  2, 8},
                            {15, 10, 9, 14}};

void plot(int x, int y, double c, uchar *data, int width, int brightness, double gamma) {
    data[y * width + x] = pow(double(brightness) / 255, gamma) * c * 255;
}

void ordered(uchar *data, int bits, int width, int height, double gamma) {
    int factor = pow(2, bits) - 1;
    uchar closest;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            closest = round(factor * data[i * width + j] / 255) * (255 / factor);
            if ((double) data[i * width + j] / 255 >= (double) orderedMatrix[i % 8][j % 8] / 63)
                closest = min(255, closest + 255 / factor);
            plot(j, i, 1, data, width, closest, gamma);
        }
    }
}

void halftone(uchar *data, int bits, int width, int height, double gamma) {
    int factor = pow(2, bits) - 1;
    uchar closest;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            closest = round(factor * data[i * width + j] / 255) * (255 / factor);
            if ((double) data[i * width + j] / 255 >= (double) halftoneMatrix[i % 4][j % 4] / 15)
                closest = min(255, closest + 255 / factor);
            plot(j, i, 1, data, width, closest, gamma);
        }
    }
}

void random(uchar *data, int bits, int width, int height, double gamma) {
    srand(std::time(nullptr));
    int factor = pow(2, bits) - 1;
    uchar closest;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            closest = round(factor * data[i * width + j] / 255) * (255 / factor);
            if (data[i * width + j] >= rand() % 255)
                closest = min(255, closest + 255 / factor);
            plot(j, i, 1, data, width, closest, gamma);
        }
    }
}

bool isValid(int y, int x, int height, int width) {
    return y > 0 && x > 0 && y < height && x < width;
}

void floydSteinberg(uchar *data, int bits, int width, int height, double gamma) {
    double distribution[6] = {0, 0, 7.0 / 16, 3.0 / 16, 5.0 / 16, 1.0 / 16};
    int factor = pow(2, bits) - 1;
    double *temp = new double[height * width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i * width + j] = (double) data[i * width + j] / 255;
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double oldPixel = temp[i * width + j];
            double newPixel = round(factor * oldPixel) / factor;
            temp[i * width + j] = newPixel;
            double error = oldPixel - newPixel;
            for (int k = 0; k < 2; k++) {
                for (int l = -1; l < 2; l++) {
                    if (isValid(i + k, j + l, height, width))
                        temp[(i + k) * width + j + l] += error * distribution[k * 3 + l + 1];
                }
            }
            plot(j, i, 1, data, width, temp[i * width + j] * 255, gamma);
        }
    }
    delete[] temp;
}

void jarvis(uchar *data, int bits, int width, int height, double gamma) {
    double distribution[] = {0, 0, 0, 7.0 / 48, 5.0 / 48, 3.0 / 48, 5.0 / 48, 7.0 / 48, 5.0 / 48, 3.0 / 48, 1.0 / 48,
                             3.0 / 48,
                             5.0 / 48, 3.0 / 48, 1.0 / 48};
    int factor = pow(2, bits) - 1;
    double *temp = new double[height * width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i * width + j] = (double) data[i * width + j] / 255;
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double oldPixel = temp[i * width + j];
            double newPixel = round(factor * oldPixel) / factor;
            temp[i * width + j] = newPixel;
            double error = oldPixel - newPixel;
            for (int k = 0; k < 2; k++) {
                for (int l = -2; l < 3; l++) {
                    if (isValid(i + k, j + l, height, width))
                        temp[(i + k) * width + j + l] += error * distribution[k * 5 + l + 2];
                }
            }
            plot(j, i, 1, data, width, temp[i * width + j] * 255, gamma);
        }
    }
    delete[] temp;
}

void sierra(uchar *data, int bits, int width, int height, double gamma) {
    double distribution[] = {0, 0, 0, 5.0 / 32, 3.0 / 32, 2.0 / 32, 4.0 / 32, 5.0 / 32, 4.0 / 32, 2.0 / 32, 0, 2.0 / 32,
                             3.0 / 32, 2.0 / 32, 0};
    int factor = pow(2, bits) - 1;
    double *temp = new double[height * width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i * width + j] = (double) data[i * width + j] / 255;
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double oldPixel = temp[i * width + j];
            double newPixel = round(factor * oldPixel) / factor;
            temp[i * width + j] = newPixel;
            double error = oldPixel - newPixel;
            for (int k = 0; k < 2; k++) {
                for (int l = -2; l < 3; l++) {
                    if (isValid(i + k, j + l, height, width))
                        temp[(i + k) * width + j + l] += error * distribution[k * 5 + l + 2];
                }
            }
            plot(j, i, 1, data, width, temp[i * width + j] * 255, gamma);
        }
    }
    delete[] temp;
}

void atkinson(uchar *data, int bits, int width, int height, double gamma) {
    double distribution[] = {0, 0, 0.125, 0.125, 0.125, 0.125, 0.125, 0, 0, 0.125, 0, 0};
    int factor = pow(2, bits) - 1;
    double *temp = new double[height * width];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp[i * width + j] = (double) data[i * width + j] / 255;
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double oldPixel = temp[i * width + j];
            double newPixel = round(factor * oldPixel) / factor;
            temp[i * width + j] = newPixel;
            double error = oldPixel - newPixel;
            for (int k = 0; k < 3; k++) {
                for (int l = -1; l < 3; l++) {
                    if (isValid(i + k, j + l, height, width))
                        temp[(i + k) * width + j + l] += error * distribution[k * 4 + l + 1];
                }
            }
            plot(j, i, 1, data, width, temp[i * width + j] * 255, gamma);
        }
    }
    delete[] temp;
}


void dither(uchar *data, int gradient, int bits, int width, int height, double gamma, int ditherType) {
    if (gradient) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                data[i * width + j] = j * 255 / height;
            }
        }
    }
    switch (ditherType) {
        case NO_DITHER: {
            return;
        }
        case ORDERED: {
            ordered(data, bits, width, height, gamma);
            break;
        }
        case RANDOM: {
            random(data, bits, width, height, gamma);
            break;
        }
        case FLOYD_STEINBERG: {
            floydSteinberg(data, bits, width, height, gamma);
            break;
        }
        case JARVIS: {
            jarvis(data, bits, width, height, gamma);
            break;
        }
        case SIERRA: {
            sierra(data, bits, width, height, gamma);
            break;
        }
        case ATKINSON: {
            atkinson(data, bits, width, height, gamma);
            break;
        }
        case HALFTONE: {
            halftone(data, bits, width, height, gamma);
            break;
        }
        default:
            break;
    }
}

void error(int errCode) {
    fprintf(stderr, "Error! Error code: %i", errCode);
}

int main(int argc, char **argv) {
    if (argc < ARGS_NUM) {
        error(ARGUMENTS);
        return 1;
    }
    FILE *input = fopen(argv[INPUT], "rb");
    if (!input) {
        error(NO_INPUT);
        return 1;
    } else {
        char format;
        int width, height, depth;
        int parsed = fscanf(input, "P%c\n%i %i\n%i\n", &format, &width, &height, &depth);
        if (parsed != 4 || format != '5') {
            error(HEADER_PARSING);
            fclose(input);
            return 1;
        }
        printf("%c %i %i %i\n", format, width, height, depth);
        uchar *data;
        data = new uchar[width * height];
        unsigned int size = fread(data, 1, width * height, input);
        if (size != width * height) {
            error(INPUT_BROKEN);
            delete[] data;
            fclose(input);
            return 1;
        }
        dither(data, atoi(argv[GRADIENT]), atoi(argv[BITS]), width, height, atof(argv[GAMMA]), atoi(argv[DITHERING]));
        FILE *output = fopen(argv[OUTPUT], "wb");
        if (!output) {
            error(NO_OUTPUT);
            delete[] data;
            fclose(input);
            return 1;
        } else {
            int writeBytes = fprintf(output, "P%c\n%i %i\n%i\n", format, width, height, depth);
            unsigned int writeMembers = fwrite(data, 1, width * height, output);
            if (writeMembers < width * height || writeBytes == -1) {
                error(OUTPUT_ERROR);
            }
            fclose(input);
            fclose(output);
            delete[] data;
        }
    }
    return 0;
}
