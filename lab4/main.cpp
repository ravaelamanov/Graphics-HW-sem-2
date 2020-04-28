#include <iostream>
#include "ColorSpace.h"
#include <cstring>
#include <vector>

typedef unsigned char uchar;
typedef unsigned long long ull;
typedef unsigned int uint;
enum Errors {
    ARGUMENTS = 1, NO_INPUT, HEADER_PARSING, INPUT_BROKEN, NO_OUTPUT, OUTPUT_ERROR
};
std::vector<FILE*> input;
std::vector<FILE*> output;
std::vector<uchar*> inputData;
std::vector<uchar*> outputData;
uchar* picture;

void error(int errCode) {
    fprintf(stderr, "Error! Error code: %i", errCode);
}

void closeFiles() {
    for (int i = 0; i < input.size(); i++) {
        if (input[i])
            fclose(input[i]);
    }
    for (int i = 0; i < output.size(); i++) {
        if (output[i])
            fclose(output[i]);
    }
}

void freeData() {
    for (int i = 0; i < inputData.size(); i++)
        delete [] inputData[i];
    for (int i = 0; i < outputData.size(); i++) {
        delete [] outputData[i];
    }
    delete picture;
}

void compose(int width, int height) {
    if (inputData.size() > 1) {
        picture = new uchar[width * height * 3];
        int k = 0;
        for (int i = 0; i < height * width; i++) {
            picture[k++] = inputData[0][i];
            picture[k++] = inputData[1][i];
            picture[k++] = inputData[2][i];
        }
    }
    else
        picture = inputData[0];
}

void decompose(int width, int height) {
    outputData.resize(3);
    for (int i = 0; i < 3; i++) {
        outputData[i] = new uchar[width * height];
    }
    int k = 0;
    for (int i = 0; i < height * 3; i++) {
        for (int j = 0; j < width; j += 3) {
            outputData[0][k] = picture[i * width + j];
            outputData[1][k] = picture[i * width + j + 1];
            outputData[2][k] = picture[i * width + j + 2];
            k++;
        }
    }
}

void write(int width, int height, int format, int depth) {
    if (output.size() == 1) {
        int writeBytes = fprintf(output[0], "P%c\n%i %i\n%i\n", '6', width, height, depth);
        unsigned int writeMembers = fwrite(picture, 1, width * height * 3, output[0]);
        if (writeMembers < width * height * 3 || writeBytes == -1) {
            error(OUTPUT_ERROR);
        }
        closeFiles();
        freeData();
        exit(1);
    }
    else {
        for (int i = 0; i < output.size(); i++) {
            int writeBytes = fprintf(output[i], "P%c\n%i %i\n%i\n", '5', width, height, depth);
            if (writeBytes == -1) {
                error(OUTPUT_ERROR);
                closeFiles();
                freeData();
                exit(1);
            }
        }
        decompose(width, height);
        for (int i = 0; i < outputData.size(); i++) {
            unsigned int writeMembers = fwrite(outputData[i], 1, width * height, output[i]);
            if (writeMembers < width * height) {
                error(OUTPUT_ERROR);
                closeFiles();
                freeData();
                exit(1);
            }
        }
    }
}

void convert(const std::string& from, const std::string& to, int height, int width) {
    for (int i = 0; i < height * 3; i++) {
        for (int j = 0; j < width; j += 3) {
            uchar comp1, comp2, comp3;
            comp1 = picture[i * width + j];
            comp2 = picture[i * width + j + 1];
            comp3 = picture[i * width + j + 2];

            ColorSpace::Rgb rgb;

            if (from == "RGB") {
                rgb.r = comp1;
                rgb.g = comp2;
                rgb.b = comp3;
            }
            else if (from == "HSL") {
                ColorSpace::Hsl hsl(comp1, comp2, comp3);
                hsl.toRgb(&rgb);
            }
            else if (from == "HSV") {
                ColorSpace::Hsv hsv(comp1, comp2, comp3);
                hsv.toRgb(&rgb);
            }
            else if (from == "YCbCr.601") {
                ColorSpace::Ycbcr601 ycbcr601(comp1, comp2, comp3);
                ycbcr601.toRgb(&rgb);
            }
            else if (from == "YCbCr.709") {
                ColorSpace::Ycbcr709 ycbcr709(comp1, comp2, comp3);
                ycbcr709.toRgb(&rgb);
            }
            else if (from == "CMY") {
                ColorSpace::Cmy cmy(comp1, comp2, comp3);
                cmy.toRgb(&rgb);
            }
            else if (from == "YCoCg") {
                ColorSpace::Ycocg ycocg(comp1, comp2, comp3);
                ycocg.toRgb(&rgb);
            }


            if (to == "RGB") {
                picture[i * width + j] = rgb.r;
                picture[i * width + j + 1] = rgb.g;
                picture[i * width + j + 2] = rgb.b;
            }
            else if (to == "HSV") {
                ColorSpace::Hsv hsv;
                rgb.toHsv(&hsv);
                picture[i * width + j] = hsv.h;
                picture[i * width + j + 1] = hsv.s;
                picture[i * width + j + 2] = hsv.v;
            }
            else if (to == "HSL") {
                ColorSpace::Hsl hsl;
                rgb.toHsl(&hsl);
                picture[i * width + j] = hsl.h;
                picture[i * width + j + 1] = hsl.s;
                picture[i * width + j + 2] = hsl.l;
            }
            else if (to == "YCbCr.601") {
                ColorSpace::Ycbcr601 ycbcr601;
                rgb.toYcbcr601(&ycbcr601);
                picture[i * width + j] = ycbcr601.y;
                picture[i * width + j + 1] = ycbcr601.cb;
                picture[i * width + j + 2] = ycbcr601.cr;
            }
            else if (to == "YCbCr.709") {
                ColorSpace::Ycbcr709 ycbcr709;
                rgb.toYcbcr709(&ycbcr709);
                picture[i * width + j] = ycbcr709.y;
                picture[i * width + j + 1] = ycbcr709.cb;
                picture[i * width + j + 2] = ycbcr709.cr;
            }
            else if (to == "CMY") {
                ColorSpace::Cmy cmy;
                rgb.toCmy(&cmy);
                picture[i * width + j] = cmy.c;
                picture[i * width + j + 1] = cmy.m;
                picture[i * width + j + 2] = cmy.y;
            }
            else if (to == "YCoCg") {
                ColorSpace::Ycocg ycocg;
                rgb.toYcocg(&ycocg);
                picture[i * width + j] = ycocg.y;
                picture[i * width + j + 1] = ycocg.co;
                picture[i * width + j + 2] = ycocg.cg;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::string from;
    std::string to;
    int inputCount, outputCount;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-f")) {
            i++;
            from = argv[i];
        }
        else if (!strcmp(argv[i], "-t")) {
            i++;
            to = argv[i];
        }
        else if (!strcmp(argv[i], "-i")) {
            i++;
            inputCount = atoi(argv[i]);
            i++;
            int cur = i;
            for (; i < cur + inputCount; i++) {
                input.emplace_back(fopen(argv[i], "rb"));
                if (!input[input.size() - 1]) {
                    closeFiles();
                    error(NO_INPUT);
                    return 1;
                }
            }
            i--;
        }
        else if (!strcmp(argv[i], "-o")) {
            i++;
            outputCount = atoi(argv[i]);
            i++;
            int cur = i;
            for (; i < cur + outputCount; i++) {
                output.emplace_back(fopen(argv[i], "wb"));
                std::string out = argv[i];
                std::cout << out << "\n";
                if (!output[output.size() - 1]) {
                    closeFiles();
                    error(NO_OUTPUT);
                    return 1;
                }
            }
        }
    }
    char format;
    int width, height, depth;
    for (auto file : input) {
        int parsed = fscanf(file, "P%c\n%i %i\n%i\n", &format, &width, &height, &depth);
        if (parsed != 4 || (format == '5' && inputCount == 1) || (format == '6' && inputCount == 3)) {
            error(HEADER_PARSING);
            closeFiles();
            return 1;
        }
    }
    std::cout << from << " " << to << " " << inputCount << " " << outputCount << "\n";
    std::cout << height << " " << width << " " << format << " " << depth << "\n";
    int inputChannels = (inputCount == 1 ? 3 : 1);
    int outputChannels = (outputCount == 1 ? 3 : 1);
    inputData.resize(inputCount);
    for (int i = 0; i < input.size(); i++) {
        inputData[i] = new uchar[height * width * inputChannels];
        ull size = fread(inputData[i], 1, width * height * inputChannels, input[i]);
        if (size != width * height * inputChannels) {
            error(INPUT_BROKEN);
            freeData();
            closeFiles();
            return 1;
        }
    }
    compose(width, height);
    convert(from, to, height, width);
    write(width, height, format, depth);
    closeFiles();
    freeData();
    return 0;
}
