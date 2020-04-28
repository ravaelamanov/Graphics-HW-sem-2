#include <stdio.h>
#include <math.h>
#include <utility>
#include <stdlib.h>
#include <algorithm>

typedef unsigned char uchar;
enum Errors {
    ARGUMENTS = 1, NO_INPUT, HEADER_PARSING, INPUT_BROKEN, NO_OUTPUT, OUTPUT_ERROR
};
enum ARGS {
    INPUT = 1, OUTPUT, BRIGHTNESS, THICKNESS, X_BEGIN, Y_BEGIN, X_END, Y_END, GAMMA
};

void plot(int x, int y, double c, uchar *data, int width, double brightness, double gamma) {
    if (gamma == 0) { //sRGB gamma
        if (brightness <= 0.0031308)
            data[y * width + x] = 12.92 * brightness * c * 255;
        else
            data[y * width + x] = (1.055 * pow(brightness, 1 / 2.4) - 0.055) * c * 255;
    } else // user-defined gamma
        data[y * width + x] = pow(brightness, gamma) * c * 255;
}

void plotAA(int x, int y, double alpha, uchar *data, int width, double brightness, double gamma) {
    double back = data[y * width + x] / 255.0;
    if (gamma == 0) { //sRGB gamma
        if (brightness <= 0.0031308)
            data[y * width + x] = 12.92 * brightness * 255 * alpha + back * 255 * (1 - alpha);
        else
            data[y * width + x] = (1.055 * pow(brightness, 1 / 2.4) - 0.055) * 255 * alpha +
                                  back * (1 - alpha) * 255;
    } else // user-defined gamma
        data[y * width + x] = pow(brightness, gamma) * 255 * alpha + back * (1 - alpha) * 255;
}

int iPart_(double x) {
    return floor(x);
}

int round_(double x) {
    return iPart_(x + 0.5);
}

double fPart_(double x) {
    if (x < 0) return x - (iPart_(x) + 1);
    return x - floor(x);
}

double rfPart_(double x) {
    return 1 - fPart_(x);
}

void drawLineWuNoAA(double x0, double y0, double x1, double y1, double brightness, uchar *data, int height, int width,
                    int thickness, double gamma) { //Отрисовка линий без сглаживания (NoAA = No anti-aliasing)
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = dy / dx;
    if (dx == 0)
        gradient = 1;

    int xEnd = round_(x0);
    double yEnd = y0 + gradient * (xEnd - x0);
    double xGap = rfPart_(x0 + 0.5);

    int xpxl1 = xEnd;
    int ypxl1 = iPart_(yEnd);

    if (steep) {
        plotAA(ypxl1, xpxl1, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(ypxl1 + 1, xpxl1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    } else {
        plotAA(xpxl1, ypxl1, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(xpxl1, ypxl1 + 1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    }

    double interY = yEnd + gradient;

    xEnd = round_(x1);
    yEnd = y1 + gradient * (xEnd - x1);
    xGap = fPart_(x1 + 0.5);

    int xpxl2 = xEnd;
    int ypxl2 = iPart_(yEnd);

    if (steep) {
        plotAA(ypxl2, xpxl2, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(ypxl2 + 1, xpxl2, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    } else {
        plotAA(xpxl2, ypxl2, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(xpxl2, ypxl2 + 1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    }

    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            plot(iPart_(interY), x, 1, data, width, brightness, gamma);
            plot(iPart_(interY) + 1, x, 1, data, width, brightness, gamma);
            interY += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            plot(x, iPart_(interY), 1, data, width, brightness, gamma);
            plot(x, iPart_(interY) + 1, 1, data, width, brightness, gamma);
            interY += gradient;
        }
    }
}

void drawLineWu(double x0, double y0, double x1, double y1, double brightness, uchar *data, int height, int width,
                int thickness, double gamma) {
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    double dx = x1 - x0;
    double dy = y1 - y0;
    double gradient = dy / dx;
    if (dx == 0)
        gradient = 1;

    int xEnd = round_(x0);
    double yEnd = y0 + gradient * (xEnd - x0);
    double xGap = rfPart_(x0 + 0.5);

    int xpxl1 = xEnd;
    int ypxl1 = iPart_(yEnd);

    if (steep) {
        plotAA(ypxl1, xpxl1, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(ypxl1 + 1, xpxl1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    } else {
        plotAA(xpxl1, ypxl1, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(xpxl1, ypxl1 + 1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    }
    double interY = yEnd + gradient;

    xEnd = round_(x1);
    yEnd = y1 + gradient * (xEnd - x1);
    xGap = fPart_(x1 + 0.5);

    int xpxl2 = xEnd;
    int ypxl2 = iPart_(yEnd);

    if (steep) {
        plotAA(ypxl2, xpxl2, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(ypxl2 + 1, xpxl2, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    } else {
        plotAA(xpxl2, ypxl2, rfPart_(yEnd) * xGap, data, width, brightness, gamma);
        plotAA(xpxl2, ypxl2 + 1, fPart_(yEnd) * xGap, data, width, brightness, gamma);
    }

    if (steep) {
        for (int x = xpxl1 + 1; x < xpxl2 - 1; x++) {
            double rf_part = rfPart_(interY);
            double f_part = fPart_(interY);
            plotAA(iPart_(interY), x, rf_part, data, width, brightness, gamma);
            plotAA(iPart_(interY) + 1, x, f_part, data, width, brightness, gamma);
            interY += gradient;
        }
    } else {
        for (int x = xpxl1 + 1; x < xpxl2 - 1; x++) {
            double rf_part = rfPart_(interY);
            double f_part = fPart_(interY);
            plotAA(x, iPart_(interY), rf_part, data, width, brightness, gamma);
            plotAA(x, iPart_(interY) + 1, f_part, data, width, brightness, gamma);
            interY += gradient;
        }
    }
}


void drawRectangle(double x0, double y0, double x1, double y1, double brightness, uchar *data, int height, int width,
                   int thickness, double gamma) {
    double dx = x1 - x0;
    double dy = y1 - y0;
    double dist = sqrt(dx * dx + dy * dy);
    dx /= dist;
    dy /= dist;
    double a0, b0, a1, b1, a2, b2, a3, b3;
    a0 = x0 - thickness * dy / 2;
    b0 = y0 + thickness * dx / 2;
    a1 = x0 + thickness * dy / 2;
    b1 = y0 - thickness * dx / 2;
    a2 = x1 - thickness * dy / 2;
    b2 = y1 + thickness * dx / 2;
    a3 = x1 + thickness * dy / 2;
    b3 = y1 - thickness * dx / 2;
    drawLineWu(a0, b0, a2, b2, brightness, data, height, width, thickness, gamma);
    drawLineWu(a1, b1, a3, b3, brightness, data, height, width, thickness, gamma);
    for (int i = thickness - 2; i >= 0; i--) {
        a0 = x0 - i * dy / 2;
        b0 = y0 + i * dx / 2;
        a1 = x0 + i * dy / 2;
        b1 = y0 - i * dx / 2;
        a2 = x1 - i * dy / 2;
        b2 = y1 + i * dx / 2;
        a3 = x1 + i * dy / 2;
        b3 = y1 - i * dx / 2;
        drawLineWuNoAA(a0, b0, a2, b2, brightness, data, height, width, thickness, gamma);
        drawLineWuNoAA(a1, b1, a3, b3, brightness, data, height, width, thickness, gamma);
    }
}

void drawLine(double x0, double y0, double x1, double y1, double brightness, uchar *data, int height, int width,
              int thickness, double gamma) {
    if (thickness == 1) {
        drawLineWu(x0, y0, x1, y1, brightness, data, height, width, thickness, gamma);
        return;
    }
    drawRectangle(x0, y0, x1, y1, brightness, data, height, width, thickness, gamma);
}

void error(int errCode) {
    fprintf(stderr, "Error! Error code: %i", errCode);
}

int main(int argc, char **argv) {
    if (argc < 9) {
        error(ARGUMENTS);
        return 1;
    }
    double gamma;
    if (argc == 9)
        gamma = 0;
    if (argc == 10)
        gamma = atof(argv[GAMMA]);
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
        drawLine(atof(argv[X_BEGIN]), atof(argv[Y_BEGIN]), atof(argv[X_END]), atof(argv[Y_END]),
                 atof(argv[BRIGHTNESS]) / 255.0,
                 data, height, width, atof(argv[THICKNESS]), gamma);

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
