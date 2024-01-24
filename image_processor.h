#ifndef CPP_HSE_IMAGE_PROCESSOR_H
#define CPP_HSE_IMAGE_PROCESSOR_H
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <string>

using WORD = uint16_t;
using DWORD = uint32_t;
using LONG = int32_t;

typedef struct __attribute__((packed)) tagBITMAPFILEHEADER {  // NOLINT
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;  // NOLINT

typedef struct __attribute__((packed)) tagBITMAPINFOHEADER {  // NOLINT
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;  // NOLINT

struct BGR {
    float blue = 0.0f;
    float green = 0.0f;
    float red = 0.0f;
    BGR() {
    }
    BGR(unsigned char b, unsigned char g, unsigned char r) : blue(b), green(g), red(r) {
    }
    ~BGR() {
    }
};

class Image {
public:
    BITMAPINFOHEADER info_header;
    BITMAPFILEHEADER file_header;
    std::vector<std::vector<BGR>> image_;
    Image(std::unique_ptr<unsigned char> rgb_s, BITMAPFILEHEADER file, BITMAPINFOHEADER info);
    static Image ImageRead(const char* file);
    static void ImageWrite(Image& image, const char* file);
    unsigned char* IzRGB();
    int RealWidth() const;
};

class Filter {
public:
    void MatrixProduct(Image&, std::vector<std::vector<float>>);
    virtual void FilterMethod(Image& sample);
    std::vector<std::vector<int>> filter_vec;
    ~Filter() {
    }
};

class Negative : public Filter {
public:
    void FilterMethod(Image& sample) override;
};

class GrayScale : public Filter {
public:
    void FilterMethod(Image& sample) override;
};

class Sharpening : public Filter {
public:
    void FilterMethod(Image& sample) override;
};

class Crop : public Filter {
public:
    void CropFilter(Image& sample, int new_width, int new_height);
};

class EdgeDetection : public Filter {
public:
    void EdgeMethod(Image& sample, float threshold);
};
#endif  // CPP_HSE_IMAGE_PROCESSOR_H
