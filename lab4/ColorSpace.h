//
// Created by Sherzod on 27.04.2020.
//

#ifndef LAB5_COLORSPACE_H
#define LAB5_COLORSPACE_H

namespace ColorSpace {
    typedef unsigned char uchar;

    enum Spaces {
        RGB, HSL, HSV, YCbCr601, YCbCr709, YCoCg, CMY
    };

    double Hue_2_RGB(double v1, double v2, double vh);

    struct IColorSpace {
        virtual void toRgb(struct Rgb* obj) const = 0;
    };

    const double gamma = 2.2;
    void linearize(uchar& a, uchar& b, uchar& c);
    void delinearize(uchar& a, uchar& b, uchar& c);

    struct Rgb : public IColorSpace {
        uchar r, g, b;
        Rgb();
        Rgb(uchar r, uchar g, uchar b);
        void toRgb(struct Rgb* color) const override;
        void toCmy(struct Cmy *color) const;
        void toHsv(struct Hsv* color) const;
        void toHsl(struct Hsl* color) const;
        void toYcbcr601(struct Ycbcr601* color) const;
        void toYcbcr709(struct Ycbcr709* color) const;
        void toYcocg(struct Ycocg* color) const;
    };

    struct Cmy : public IColorSpace {
        uchar c, m, y;
        Cmy();
        Cmy(uchar c, uchar m, uchar y);
        void toRgb(struct Rgb* color) const override;
    };

    struct Hsv : public IColorSpace {
        uchar h, s, v;
        Hsv();
        Hsv(uchar h, uchar s, uchar v);
        void toRgb(struct Rgb* color) const override;
    };

    struct Hsl : public IColorSpace {
        uchar h, s, l;
        Hsl();
        Hsl(uchar h, uchar s, uchar l);
        void toRgb(struct Rgb* color) const override;
    };

    struct Ycbcr601 : public IColorSpace {
        uchar y, cb, cr;
        Ycbcr601();
        Ycbcr601(uchar y, uchar cb, uchar cr);
        void toRgb(struct Rgb* color) const override;
    };

    struct Ycbcr709 : public IColorSpace {
        uchar y, cb, cr;
        Ycbcr709();
        Ycbcr709(uchar y, uchar cb, uchar cr);
        void toRgb(struct Rgb* color) const override;
    };

    struct Ycocg : public IColorSpace {
        uchar y, co, cg;
        Ycocg();
        Ycocg(uchar y, uchar co, uchar cg);
        void toRgb(struct Rgb* color) const override;
    };

}

#endif //LAB5_COLORSPACE_H
