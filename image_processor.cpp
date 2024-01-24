#include "image_processor.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Filters available:\n -neg Negative,\n -gs Greyscale,\n -sharp Sharpening,\n -crop <width> "
                     "<height>,\n -edge <threshold>,\n ENJOY FILTERS!";
        return 0;
    }

    Image image = Image::ImageRead(argv[1]);
    for (int i = 3; i < argc; ++i) {
        if (static_cast<std::string>(argv[i]) == "-crop") {
            Crop ex1;
            ex1.CropFilter(image, std::stoi(argv[i + 1]), std::stoi(argv[i + 2]));
        } else if (static_cast<std::string>(argv[i]) == "-gs") {
            GrayScale ex2;
            ex2.FilterMethod(image);
        } else if (static_cast<std::string>(argv[i]) == "-neg") {
            Negative ex3;
            ex3.FilterMethod(image);
        } else if (static_cast<std::string>(argv[i]) == "-sharp") {
            Sharpening ex4;
            ex4.FilterMethod(image);
        } else if (static_cast<std::string>(argv[i]) == "-edge") {
            EdgeDetection ex5;
            ex5.EdgeMethod(image, std::stof(argv[i + 1]));
        }
    }
    Image::ImageWrite(image, argv[2]);
    return 0;
}