//
// Created by Sherzod on 27.04.2020.
//

#include "ColorSpace.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace ColorSpace  {
    double Hue_2_RGB(double v1, double v2, double vh) {
        if (vh < 0) vh += 1;
        if (vh > 1) vh -= 1;
        if (6 * vh < 1) return v1 + (v2 - v1) * 6 * vh;
        if (2 * vh < 1) return v2;
        if (3 * vh < 2) return v1 + (v2 - v1)*(2.0 / 3.0 - vh) * 6;
        return v1;
    }

    /**Rgb definitions*/
    Rgb::Rgb(uchar r, uchar g, uchar b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void Rgb::toRgb(struct Rgb *color) const {
        color->r = r;
        color->g = g;
        color->b = b;
    }

    void Rgb::toCmy(struct Cmy *color) const {
        color->c = 255 - r;
        color->m = 255 - g;
        color->y = 255 - b;
    }

    void Rgb::toHsv(struct Hsv *color) const {
        double rAux = r / 255.0;
        double gAux = g / 255.0;
        double bAux = b / 255.0;

        double max = std::max(std::max(rAux, gAux), bAux);
        double min = std::min(std::min(rAux, gAux), bAux);
        double delta = max - min;

        double sAux = delta / max;
        double vAux = max;

        if (sAux == 0) {
            color->h = 0;
            color->s = 0;
            color->v = vAux * 255;
        }
        else {
            double hAux = 0;
            if (rAux == max) hAux = (gAux - bAux) / delta;
            else if (gAux == max) hAux = 2 + (bAux - rAux) / delta;
            else if (bAux == max) hAux = 4 + (rAux - gAux) / delta;

            hAux *= 60;
            color->h = ((int)std::round(hAux + 360) % 360) / 360.0 * 255;
            color->s = sAux * 255;
            color->v = vAux * 255;
        }
    }

    void Rgb::toHsl(struct Hsl *color) const {
        double rAux = r / 255.0;
        double gAux = g / 255.0;
        double bAux = b / 255.0;

        double max = std::max(std::max(rAux, gAux), bAux);
        double min = std::min(std::min(rAux, gAux), bAux);
        double delta = max - min;
        double lAux = (max + min) / 2;

        if (delta == 0) {
            color->h = 0;
            color->s = 0;
            color->l = lAux * 255;
        }
        else {
            double sAux = delta / (1 - std::abs(2 * lAux  - 1));
            double hAux = 0;
            if (rAux == max) hAux = (gAux - bAux) / delta;
            else if (gAux == max) hAux = 2 + (bAux - rAux) / delta;
            else if (bAux == max) hAux = 4 + (rAux - gAux) / delta;

            hAux *= 60;
            color->h = ((int)std::round(hAux + 360) % 360) / 360.0 * 255;
            color->s = sAux * 255;
            color->l = lAux * 255;
        }
    }

    void Rgb::toYcbcr601(struct Ycbcr601 *color) const {
        color->y = 16 + (65.738 * r + 129.057 * g + 25.064 * b) / 256;
        color->cb = 128 + (-37.945 * r - 74.494 * g + 112.439 * b) / 256;
        color->cr = 128 + (112.439 * r - 94.154 * g - 18.285 * b) / 256;
    }

    void Rgb::toYcbcr709(struct Ycbcr709 *color) const {
        color->y = 0 + 0.299 * r + 0.587 * g + 0.114 * b;
        color->cb = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b;
        color->cr = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b;
    }

    void linearize(uchar& a, uchar& b, uchar& c) {
        a = pow(a / 255.0, 1 / gamma) * 255;
        b = pow(b / 255.0, 1 / gamma) * 255;
        c = pow(c / 255.0, 1 / gamma) * 255;
    }

    void delinearize(uchar& a, uchar& b, uchar& c) {
        a = pow(a / 255.0, gamma) * 255;
        b = pow(b / 255.0, gamma) * 255;
        c = pow(c / 255.0, gamma) * 255;
    }

    Rgb::Rgb() {
        r = 0;
        g = 0;
        b = 0;
    }

    void Rgb::toYcocg(struct Ycocg *color) const {
        color->co = r - b;
        uchar temp = b + (color->co >> 1);
        color->cg = g - temp;
        color->y = temp + (color->cg >> 1);
    }


    /**Cmy definitions*/
    Cmy::Cmy(uchar c, uchar m, uchar y) {
        this->c = c;
        this->m = m;
        this->y = y;
    }

    void Cmy::toRgb(struct Rgb *color) const {
        color->r = 1 - c;
        color->g = 1 - m;
        color->b = 1 - y;
    }

    Cmy::Cmy() {
        c = 0;
        m = 0;
        y = 0;
    }

    /**Hsv definitions*/
    Hsv::Hsv(uchar h, uchar s, uchar v) {
        this->h = h;
        this->s = s;
        this->v = v;
    }

    void Hsv::toRgb(struct Rgb *color) const {
        double hex = h / 255.0 * 360.0 / 60.0;
        double hAux = h / 255.0;
        double sAux = s / 255.0;
        double vAux = v / 255.0;

        int primary = floor(hex);
        double secondary = hex - primary;

        double a = (1 - sAux) * vAux;
        double b = (1 - (sAux * secondary)) * vAux;
        double c = (1 - (sAux * (1 - secondary))) * vAux;

        switch (primary) {
            case 0: {
                color->r = vAux * 255;
                color->g = c * 255;
                color->b = a * 255;
                break;
            }
            case 1: {
                color->r = b * 255;
                color->g = vAux * 255;
                color->b = a * 255;
                break;
            }
            case 2: {
                color->r = a * 255;
                color->g = vAux * 255;
                color->b = c * 255;
                break;
            }
            case 3: {
                color->r = a * 255;
                color->g = b * 255;
                color->b = vAux * 255;
                break;
            }
            case 4: {
                color->r = c * 255;
                color->g = a * 255;
                color->b = vAux * 255;
                break;
            }
            case 5: {
                color->r = vAux * 255;
                color->g = a * 255;
                color->b = b * 255;
                break;
            }
            default: break;
        }
    }

    Hsv::Hsv() {
        h = 0;
        s = 0;
        v = 0;
    }

    /**Hsl definitions*/
    Hsl::Hsl(uchar h, uchar s, uchar l) {
        this->h = h;
        this->s = s;
        this->l = l;
    }

    void Hsl::toRgb(struct Rgb *color) const {
        double H = h / 255.0;
        double S = s / 255.0;
        double L = l / 255.0;

        if (s == 0)
            color->r = color->g = color->b = l;
        else {
            double temp1, temp2;
            temp2 = (L < 0.5) ? (L*(1 + S)) : (L + S - (S*L));
            temp1 = 2 * L - temp2;
            color->r = 255 * Hue_2_RGB(temp1, temp2, H + 1.0 / 3.0);
            color->g = 255 * Hue_2_RGB(temp1, temp2, H);
            color->b = 255 * Hue_2_RGB(temp1, temp2, H - 1.0 / 3.0);
        }
    }

    Hsl::Hsl() {
        h = 0;
        s = 0;
        l = 0;
    }

    /**Ycbcr601 definitions*/
    Ycbcr601::Ycbcr601(uchar y, uchar cb, uchar cr) {
        this->y = y;
        this->cb = cb;
        this->cr = cr;
    }

    void Ycbcr601::toRgb(struct Rgb *color) const {
        color->r = std::max((298.082 * y + 408.583 * cr) / 256 - 222.921, 0.0);
        color->g = std::max((298.082 * y - 100.291 * cb - 208.120 * cr) / 256 + 135.576, 0.0);
        color->b = std::max((298.082 * y + 516.412 * cb) / 256 - 276.836, 0.0);
    }

    Ycbcr601::Ycbcr601() {
        y = 0;
        cb = 0;
        cr = 0;
    }

    /**Ycbcr709 definitions*/
    Ycbcr709::Ycbcr709(uchar y, uchar cb, uchar cr) {
        this->y = y;
        this->cb = cb;
        this->cr = cr;
    }

    void Ycbcr709::toRgb(struct Rgb *color) const {
        color->r = std::max(y + 1.402 * (cr - 128), 0.0);
        color->g = std::max(y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128), 0.0);
        color->b = std::max(y + 1.772 * (cb - 128), 0.0);
    }

    Ycbcr709::Ycbcr709() {
        y = 0;
        cb = 0;
        cr = 0;
    }

    Ycocg::Ycocg() {
        y = 0;
        co = 0;
        cg = 0;
    }

    Ycocg::Ycocg(uchar y, uchar co, uchar cg) {
        this->y = y;
        this->co = co;
        this->cg = cg;
    }

    void Ycocg::toRgb(struct Rgb *color) const {
        uchar temp = y - (cg >> 1);
        color->g = cg + temp;
        color->b = temp - (co >> 1);
        color->r = co + color->b;
    }
}

