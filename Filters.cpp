#include "image_processor.h"

Image::Image(std::unique_ptr<unsigned char> rgb, BITMAPFILEHEADER file, BITMAPINFOHEADER info) {
    this->file_header = file;
    this->info_header = info;
    int width_real = RealWidth();
    unsigned char *rgb_s = rgb.release();
    image_ = std::vector<std::vector<BGR>>(info_header.biHeight, std::vector<BGR>(info_header.biWidth));
    for (int i = 0; i < info_header.biHeight; ++i) {
        for (int k = 0; k < info_header.biWidth; ++k) {
            image_[i][k].blue = static_cast<float>(rgb_s[i * width_real + 3 * k]) / 255.0f;       // NOLINT
            image_[i][k].green = static_cast<float>(rgb_s[i * width_real + 3 * k + 1]) / 255.0f;  // NOLINT
            image_[i][k].red = static_cast<float>(rgb_s[i * width_real + 3 * k + 2]) / 255.0f;    // NOLINT;
        }
    }
    free(rgb_s);
    std::cout << "File have been read successfully" << std::endl;
}

unsigned char *Image::IzRGB() {
    unsigned char *iz_rgb = nullptr;
    iz_rgb = static_cast<unsigned char *>(malloc(file_header.bfSize - file_header.bfOffBits));
    for (size_t i = 0; i < file_header.bfSize - file_header.bfOffBits; ++i) {
        iz_rgb[i] = 0;
    }
    int width_real = RealWidth();
    for (size_t i = 0; i < info_header.biHeight; ++i) {
        for (size_t j = 0; j < info_header.biWidth; ++j) {
            iz_rgb[i * width_real + 3 * j] = static_cast<unsigned char>(image_[i][j].blue * 255.0f);       // NOLINT
            iz_rgb[i * width_real + 3 * j + 1] = static_cast<unsigned char>(image_[i][j].green * 255.0f);  // NOLINT
            iz_rgb[i * width_real + 3 * j + 2] = static_cast<unsigned char>(image_[i][j].red * 255.0f);    // NOLINT
        }
    }
    return iz_rgb;
}

int Image::RealWidth() const {
    return ((4 - (info_header.biWidth * 3) % 4) % 4) + info_header.biWidth * 3;
}
Image Image::ImageRead(const char *file) {
    unsigned char *iz_rgb = nullptr;
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    FILE *stream = fopen(file, "rb");
    fread(&file_header, sizeof(file_header), 1, stream);
    fread(&info_header, sizeof(info_header), 1, stream);
    fseek(stream, file_header.bfOffBits, SEEK_SET);
    iz_rgb = static_cast<unsigned char *>(malloc(file_header.bfSize - file_header.bfOffBits));
    fread(iz_rgb, file_header.bfSize - file_header.bfOffBits, 1, stream);
    fclose(stream);
    return Image(std::unique_ptr<unsigned char>(iz_rgb), file_header, info_header);
}
void Image::ImageWrite(Image &image, const char *file) {
    FILE *outfile = fopen(file, "wb");
    fwrite(&image.file_header, sizeof(char), sizeof(BITMAPFILEHEADER), outfile);
    fwrite(&image.info_header, sizeof(char), sizeof(BITMAPINFOHEADER), outfile);
    unsigned char *iz_rgb = image.IzRGB();
    fwrite(iz_rgb, sizeof(unsigned char), image.file_header.bfSize - image.file_header.bfOffBits, outfile);
    free(iz_rgb);
    fclose(outfile);
    std::cout << "File have been written successfully";
}

void Negative::FilterMethod(Image &sample) {
    for (int i = 0; i < sample.info_header.biHeight; ++i) {
        for (int k = 0; k < sample.info_header.biWidth; ++k) {
            sample.image_[i][k].blue = 1 - sample.image_[i][k].blue;
            sample.image_[i][k].green = 1 - sample.image_[i][k].green;
            sample.image_[i][k].red = 1 - sample.image_[i][k].red;
        }
    }
}

void Filter::FilterMethod(Image &sample) {
}
void Filter::MatrixProduct(Image &sample, std::vector<std::vector<float>> working_vec) {
    float first = working_vec[0][0];
    float second = working_vec[0][1];
    float third = working_vec[0][2];
    float fourth = working_vec[1][0];
    float fifth = working_vec[1][1];
    float sixth = working_vec[1][2];
    float seven = working_vec[2][0];
    float eight = working_vec[2][1];
    float nine = working_vec[2][2];
    std::vector<std::vector<BGR>> result(sample.info_header.biHeight, std::vector<BGR>(sample.info_header.biWidth));
    for (int i = 0; i < sample.info_header.biHeight; ++i) {
        for (int j = 0; j < sample.info_header.biWidth; ++j) {
            if (i == 0 && j == 0) {
                result[i][j].blue = std::min(
                    1.0f, std::max(0.0f, first * sample.image_[i + 1][j].blue + second * sample.image_[i + 1][j].blue +
                                             third * sample.image_[i + 1][j + 1].blue +
                                             fourth * sample.image_[i][j].blue + fifth * sample.image_[i][j].blue +
                                             sixth * sample.image_[i][j + 1].blue + seven * sample.image_[i][j].blue +
                                             eight * sample.image_[i][j].blue + nine * sample.image_[i][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j].green + second * sample.image_[i + 1][j].green +
                                       third * sample.image_[i + 1][j + 1].green + fourth * sample.image_[i][j].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j + 1].green +
                                       seven * sample.image_[i][j].green + eight * sample.image_[i][j].green +
                                       nine * sample.image_[i][j + 1].green));
                result[i][j].red = std::min(
                    1.0f, std::max(0.0f, first * sample.image_[i + 1][j].red + second * sample.image_[i + 1][j].red +
                                             third * sample.image_[i + 1][j + 1].red +
                                             fourth * sample.image_[i][j].red + fifth * sample.image_[i][j].red +
                                             sixth * sample.image_[i][j + 1].red + seven * sample.image_[i][j].red +
                                             eight * sample.image_[i][j].red + nine * sample.image_[i][j + 1].red));
            } else if (i == 0 && j == sample.info_header.biWidth - 1) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].blue + second * sample.image_[i + 1][j].blue +
                                       third * sample.image_[i + 1][j].blue + fourth * sample.image_[i][j - 1].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j].blue +
                                       seven * sample.image_[i][j - 1].blue + eight * sample.image_[i][j].blue +
                                       nine * sample.image_[i][j].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].green + second * sample.image_[i + 1][j].green +
                                       third * sample.image_[i + 1][j].green + fourth * sample.image_[i][j - 1].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j].green +
                                       seven * sample.image_[i][j - 1].green + eight * sample.image_[i][j].green +
                                       nine * sample.image_[i][j].green));
                result[i][j].red = std::min(
                    1.0f, std::max(0.0f, first * sample.image_[i + 1][j - 1].red +   // NOLINT
                                             second * sample.image_[i + 1][j].red +  // NOLINT
                                             third * sample.image_[i + 1][j].red +
                                             fourth * sample.image_[i][j - 1].red + fifth * sample.image_[i][j].red +
                                             sixth * sample.image_[i][j].red + seven * sample.image_[i][j - 1].red +
                                             eight * sample.image_[i][j].red + nine * sample.image_[i][j].red));
            } else if (j == 0 && i == sample.info_header.biHeight - 1) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j].blue + second * sample.image_[i][j].blue +  // NOLINT
                                       third * sample.image_[i][j + 1].blue + fourth * sample.image_[i][j].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j + 1].blue +
                                       seven * sample.image_[i - 1][j].blue + eight * sample.image_[i - 1][j].blue +
                                       nine * sample.image_[i - 1][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j].green + second * sample.image_[i][j].green +  // NOLINT
                                       third * sample.image_[i][j + 1].green + fourth * sample.image_[i][j].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j + 1].green +
                                       seven * sample.image_[i - 1][j].green + eight * sample.image_[i - 1][j].green +
                                       nine * sample.image_[i - 1][j + 1].green));
                result[i][j].red = std::min(
                    1.0f, std::max(0.0f, first * sample.image_[i][j].red + second * sample.image_[i][j].red +  // NOLINT
                                             third * sample.image_[i][j + 1].red + fourth * sample.image_[i][j].red +
                                             fifth * sample.image_[i][j].red + sixth * sample.image_[i][j + 1].red +
                                             seven * sample.image_[i - 1][j].red + eight * sample.image_[i - 1][j].red +
                                             nine * sample.image_[i - 1][j + 1].red));
            } else if (i == sample.info_header.biHeight - 1 && j == sample.info_header.biWidth - 1) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j - 1].blue + second * sample.image_[i][j].blue +
                                       third * sample.image_[i][j].blue + fourth * sample.image_[i][j - 1].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j].blue +
                                       seven * sample.image_[i - 1][j - 1].blue + eight * sample.image_[i - 1][j].blue +
                                       nine * sample.image_[i - 1][j].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j - 1].green + second * sample.image_[i][j].green +
                                       third * sample.image_[i][j].green + fourth * sample.image_[i][j - 1].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j].green +
                                       seven * sample.image_[i - 1][j - 1].green +
                                       eight * sample.image_[i - 1][j].green + nine * sample.image_[i - 1][j].green));
                result[i][j].red = std::min(
                    1.0f, std::max(0.0f, first * sample.image_[i][j - 1].red + second * sample.image_[i][j].red +
                                             third * sample.image_[i][j].red + fourth * sample.image_[i][j - 1].red +
                                             fifth * sample.image_[i][j].red + sixth * sample.image_[i][j].red +
                                             seven * sample.image_[i - 1][j - 1].red +
                                             eight * sample.image_[i - 1][j].red + nine * sample.image_[i - 1][j].red));
            } else if (j == 0) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j].blue + second * sample.image_[i + 1][j].blue +
                                       third * sample.image_[i + 1][j + 1].blue + fourth * sample.image_[i][j].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j + 1].blue +
                                       seven * sample.image_[i - 1][j].blue + eight * sample.image_[i - 1][j].blue +
                                       nine * sample.image_[i - 1][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j].green + second * sample.image_[i + 1][j].green +
                                       third * sample.image_[i + 1][j + 1].green + fourth * sample.image_[i][j].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j + 1].green +
                                       seven * sample.image_[i - 1][j].green + eight * sample.image_[i - 1][j].green +
                                       nine * sample.image_[i - 1][j + 1].green));
                result[i][j].red = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j].red + second * sample.image_[i + 1][j].red +
                                       third * sample.image_[i + 1][j + 1].red + fourth * sample.image_[i][j].red +
                                       fifth * sample.image_[i][j].red + sixth * sample.image_[i][j + 1].red +
                                       seven * sample.image_[i - 1][j].red + eight * sample.image_[i - 1][j].red +
                                       nine * sample.image_[i - 1][j + 1].red));
            } else if (i == sample.info_header.biHeight - 1) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j - 1].blue + second * sample.image_[i][j].blue +
                                       third * sample.image_[i][j + 1].blue + fourth * sample.image_[i][j - 1].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j + 1].blue +
                                       seven * sample.image_[i - 1][j - 1].blue + eight * sample.image_[i - 1][j].blue +
                                       nine * sample.image_[i - 1][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j - 1].green + second * sample.image_[i][j].green +
                                       third * sample.image_[i][j + 1].green + fourth * sample.image_[i][j - 1].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j + 1].green +
                                       seven * sample.image_[i - 1][j - 1].green +
                                       eight * sample.image_[i - 1][j].green +
                                       nine * sample.image_[i - 1][j + 1].green));
                result[i][j].red = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i][j - 1].red + second * sample.image_[i][j].red +
                                       third * sample.image_[i][j + 1].red + fourth * sample.image_[i][j - 1].red +
                                       fifth * sample.image_[i][j].red + sixth * sample.image_[i][j + 1].red +
                                       seven * sample.image_[i - 1][j - 1].red + eight * sample.image_[i - 1][j].red +
                                       nine * sample.image_[i - 1][j + 1].red));
            } else if (i == 0) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].blue + second * sample.image_[i + 1][j].blue +
                                       third * sample.image_[i + 1][j + 1].blue +
                                       fourth * sample.image_[i][j - 1].blue + fifth * sample.image_[i][j].blue +
                                       sixth * sample.image_[i][j + 1].blue + seven * sample.image_[i][j - 1].blue +
                                       eight * sample.image_[i][j].blue + nine * sample.image_[i][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].green + second * sample.image_[i + 1][j].green +
                                       third * sample.image_[i + 1][j + 1].green +
                                       fourth * sample.image_[i][j - 1].green + fifth * sample.image_[i][j].green +
                                       sixth * sample.image_[i][j + 1].green + seven * sample.image_[i][j - 1].green +
                                       eight * sample.image_[i][j].green + nine * sample.image_[i][j + 1].green));
                result[i][j].red = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].red + second * sample.image_[i + 1][j].red +
                                       third * sample.image_[i + 1][j + 1].red + fourth * sample.image_[i][j - 1].red +
                                       fifth * sample.image_[i][j].red + sixth * sample.image_[i][j + 1].red +
                                       seven * sample.image_[i][j - 1].red + eight * sample.image_[i][j].red +
                                       nine * sample.image_[i][j + 1].red));
            } else if (j == sample.info_header.biWidth - 1) {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].blue + second * sample.image_[i + 1][j].blue +
                                       third * sample.image_[i + 1][j].blue + fourth * sample.image_[i][j - 1].blue +
                                       fifth * sample.image_[i][j].blue + sixth * sample.image_[i][j].blue +
                                       seven * sample.image_[i - 1][j - 1].blue + eight * sample.image_[i - 1][j].blue +
                                       nine * sample.image_[i - 1][j].blue));
                result[i][j].green = std::min(
                    1.0f,  // NOLINT
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].green + second * sample.image_[i + 1][j].green +
                                       third * sample.image_[i + 1][j].green + fourth * sample.image_[i][j - 1].green +
                                       fifth * sample.image_[i][j].green + sixth * sample.image_[i][j].green +
                                       seven * sample.image_[i - 1][j - 1].green +
                                       eight * sample.image_[i - 1][j].green + nine * sample.image_[i - 1][j].green));
                result[i][j].red = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i + 1][j - 1].red + second * sample.image_[i + 1][j].red +
                                       third * sample.image_[i + 1][j].red + fourth * sample.image_[i][j - 1].red +
                                       fifth * sample.image_[i][j].red + sixth * sample.image_[i][j].red +
                                       seven * sample.image_[i - 1][j - 1].red + eight * sample.image_[i - 1][j].red +
                                       nine * sample.image_[i - 1][j].red));
            } else {
                result[i][j].blue = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i - 1][j - 1].blue + second * sample.image_[i][j - 1].blue +
                                       third * sample.image_[i + 1][j - 1].blue +
                                       fourth * sample.image_[i - 1][j].blue + fifth * sample.image_[i][j].blue +
                                       sixth * sample.image_[i + 1][j].blue + seven * sample.image_[i - 1][j + 1].blue +
                                       eight * sample.image_[i][j + 1].blue + nine * sample.image_[i + 1][j + 1].blue));
                result[i][j].green = std::min(
                    1.0f,
                    std::max(0.0f,
                             first * sample.image_[i - 1][j - 1].green + second * sample.image_[i][j - 1].green +
                                 third * sample.image_[i + 1][j - 1].green + fourth * sample.image_[i - 1][j].green +
                                 fifth * sample.image_[i][j].green + sixth * sample.image_[i + 1][j].green +
                                 seven * sample.image_[i - 1][j + 1].green + eight * sample.image_[i][j + 1].green +
                                 nine * sample.image_[i + 1][j + 1].green));
                result[i][j].red = std::min(
                    1.0f,
                    std::max(0.0f, first * sample.image_[i - 1][j - 1].red + second * sample.image_[i][j - 1].red +
                                       third * sample.image_[i + 1][j - 1].red + fourth * sample.image_[i - 1][j].red +
                                       fifth * sample.image_[i][j].red + sixth * sample.image_[i + 1][j].red +
                                       seven * sample.image_[i - 1][j + 1].red + eight * sample.image_[i][j + 1].red +
                                       nine * sample.image_[i + 1][j + 1].red));
            }
        }
    }
    sample.image_ = result;
}

void Sharpening::FilterMethod(Image &sample) {
    std::vector<std::vector<float>> working_vec = {{0.0f, -1.0f, 0.0f}, {-1.0f, 5.0f, -1.0f}, {0.0f, -1.0f, 0.0f}};
    MatrixProduct(sample, working_vec);  // NOLINT
}
void GrayScale::FilterMethod(Image &sample) {
    for (int i = 0; i < sample.info_header.biHeight; ++i) {
        for (int k = 0; k < sample.info_header.biWidth; ++k) {
            float tmp = sample.image_[i][k].red * 0.299f + sample.image_[i][k].green * 0.587f +  // NOLINT
                        sample.image_[i][k].blue * 0.114f;                                       // NOLINT
            sample.image_[i][k].blue = tmp;
            sample.image_[i][k].green = tmp;
            sample.image_[i][k].red = tmp;
        }
    }
}
void Crop::CropFilter(Image &sample, int new_width, int new_height) {
    new_width = std::min(new_width, sample.info_header.biWidth);
    new_height = std::min(new_height, sample.info_header.biHeight);
    std::vector<std::vector<BGR>> result(new_height, std::vector<BGR>(new_width));
    for (int i = 0; i < sample.info_header.biHeight; ++i) {
        for (int k = 0; k < new_width; ++k) {
            if (i >= sample.info_header.biHeight - new_height) {
                result[i - (sample.info_header.biHeight - new_height)][k] = sample.image_[i][k];
            }
        }
    }

    sample.image_ = result;
    sample.info_header.biWidth = new_width;
    sample.info_header.biHeight = new_height;
    sample.info_header.biSizeImage = sample.RealWidth() * sample.info_header.biHeight;
    sample.file_header.bfSize = sample.info_header.biSizeImage + 54;  // NOLINT
}
void EdgeDetection::EdgeMethod(Image &sample, float threshold) {
    GrayScale ex;
    ex.FilterMethod(sample);
    MatrixProduct(sample, {{0.0f, -1.0f, 0.0f}, {-1.0f, 4.0f, -1.0f}, {0.0f, -1.0f, 0.0f}});  // NOLINT
    for (int y = 0; y < sample.info_header.biHeight; ++y) {
        for (int x = 0; x < sample.info_header.biWidth; ++x) {
            if (sample.image_[y][x].blue > threshold) {
                sample.image_[y][x].blue = 1.0f;
                sample.image_[y][x].green = 1.0f;
                sample.image_[y][x].red = 1.0f;
            } else {
                sample.image_[y][x].blue = 0.0f;
                sample.image_[y][x].green = 0.0f;
                sample.image_[y][x].red = 0.0f;
            }
        }
    }
}
